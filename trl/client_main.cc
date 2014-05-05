#include <algorithm>
#include <signal.h>

#include "base/all.h"
#include "rpc/client.h"
#include "rlog/rlog.h"
#include "services.h"
#include "INIReader.h"
#include "utils.h"

using namespace base;
using namespace rpc;
using namespace rlog;
using namespace std;
using namespace trl;

static bool g_stop_flag = false;

static void signal_handler(int sig) {
    if (g_stop_flag == false) {
        Log::info("caught signal %d, stopping server now", sig);
        g_stop_flag = true;
    } else {
        Log::info("caught signal %d for the second time, kill server now", sig);
        exit(-sig);
    }
}

static void install_signal_handler() {
    signal(SIGPIPE, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);

    signal(SIGALRM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);
    signal(SIGTERM, signal_handler);
}

static int conf_active_servers = -1;
static int conf_active_clients = -1;
static int conf_records_per_server = -1;
static int conf_records_per_transaction = -1;

static void load_config() {
    INIReader ini("conf/config.ini");
    verify(ini.ParseError() == 0);
    int old;

    old = conf_active_servers;
    conf_active_servers = ini.GetInteger("", "active_servers", -1);
    if (conf_active_servers != old) {
        Log::info("set active_servers = %d", conf_active_servers);
    }

    old = conf_active_clients;
    conf_active_clients = ini.GetInteger("", "active_clients", -1);
    if (conf_active_clients != old) {
        Log::info("set active_clients = %d", conf_active_clients);
    }

    old = conf_records_per_server;
    conf_records_per_server = ini.GetInteger("", "records_per_server", -1);
    if (conf_records_per_server != old) {
        Log::info("set records_per_server = %d", conf_records_per_server);
    }

    old = conf_records_per_transaction;
    conf_records_per_transaction = ini.GetInteger("", "records_per_transaction", -1);
    if (conf_records_per_transaction != old) {
        Log::info("set records_per_transaction = %d", conf_records_per_transaction);
    }

    verify(conf_active_servers * conf_records_per_server <= max_record_id);
}

ClientPool* client_pool;
Rand rnd;
int commit_count;
int abort_count;

static void do_one_transaction() {
    auto servers = server_list();

    verify(conf_active_servers > 0);
    verify((size_t) conf_active_servers <= servers.size());
    verify(conf_records_per_server > 0);
    verify(conf_records_per_transaction > 0);

    i64 txn_id = (i64(rnd.next()) << 32) | rnd.next();
//    Log::debug("txn_id = %lld", txn_id);

    vector<int> used_server_id;
    for (int i = 0; i < conf_records_per_transaction; i++) {
        int record_id = rnd.next(0, conf_active_servers * conf_records_per_server);
        int server_id = record_id / conf_records_per_server;
//        Log::debug("server_id = %d, record_id = %d", server_id, record_id);

        TRLProxy proxy(client_pool->get_client(servers[server_id]));

        if (std::find(used_server_id.begin(), used_server_id.end(), server_id) == used_server_id.end()) {
            proxy.begin_transaction(txn_id);
            used_server_id.push_back(server_id);
        }

        if (i == 0) {
            proxy.update_account(txn_id, record_id, conf_records_per_transaction - 1);
        } else {
            proxy.update_account(txn_id, record_id, -1);
        }
    }

    bool should_abort = false;
    for (int i = 0; !should_abort && i < (int) used_server_id.size(); i++) {
        int server_id = used_server_id[i];
        TRLProxy proxy(client_pool->get_client(servers[server_id]));
        i8 ok;
        proxy.commit_prepare(txn_id, &ok);
//        Log::debug("ok = %d", ok);
        if (ok != 0) {
            should_abort = true;
        }
    }

    for (auto& server_id : used_server_id) {
        TRLProxy proxy(client_pool->get_client(servers[server_id]));
        if (should_abort) {
            proxy.abort_transaction(txn_id);
            abort_count++;
        } else {
            proxy.commit_confirm(txn_id);
            commit_count++;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("usage: %s <client-id>\n", argv[0]);
        exit(1);
    }

    int client_id = atoi(argv[1]);

    RLog::init();
    install_signal_handler();
    load_config();

    const double interval = 0.2;
    client_pool = new ClientPool;

    Timer timer;
    timer.start();
    while (!g_stop_flag) {
        if (client_id <= conf_active_clients) {
            do_one_transaction();
        } else {
            usleep(100 * 1000);
        }

        if (timer.elapsed() > interval) {
            load_config();
            // RLog::info("commit_count = %d, abort_count = %d", commit_count, abort_count);
            RLog::aggregate_qps("commit_count", commit_count);
            RLog::aggregate_qps("abort_count", abort_count);
            abort_count = 0;
            commit_count = 0;
            timer.reset();
            timer.start();
        }
    }

    RLog::finalize();
    delete client_pool;

    return 0;
}

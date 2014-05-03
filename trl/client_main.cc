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
}

ClientPool* client_pool;
Rand rnd;

static void do_one_transaction() {
    auto servers = server_list();

    verify(conf_active_servers > 0);
    verify((size_t) conf_active_servers <= servers.size());
    verify(conf_records_per_server > 0);
    verify(conf_records_per_transaction > 0);

    i64 txn_id = (i64(rnd.next()) << 32) | rnd.next();
    Log::debug("txn_id = %lld", txn_id);

    for (int i = 0; i < conf_records_per_transaction; i++) {
        int record_id = rnd.next(0, conf_active_servers * conf_records_per_server);
        int server_id = record_id / conf_records_per_server;
        Log::debug("server_id = %d, record_id = %d", server_id, record_id);
    }
}

int main() {
    RLog::init();
    install_signal_handler();
    load_config();

    const double interval = 0.2;
    client_pool = new ClientPool;

    Timer timer;
    timer.start();
    while (!g_stop_flag) {
        do_one_transaction();

        if (timer.elapsed() > interval) {
            load_config();
            RLog::info("TODO: report statistics");
            timer.reset();
            timer.start();
        }
    }

    RLog::finalize();
    delete client_pool;

    return 0;
}

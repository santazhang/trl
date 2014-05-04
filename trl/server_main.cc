#include <signal.h>

#include "base/all.h"
#include "services.h"
#include "utils.h"

using namespace base;
using namespace rpc;
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

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("usage: %s <server-id>\n", argv[0]);
        exit(1);
    }

    signal(SIGPIPE, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);

    signal(SIGALRM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);
    signal(SIGTERM, signal_handler);

    size_t server_id = atoi(argv[1]);
    verify(server_id < server_list().size());
    string bind_addr = server_list()[server_id];

    TRLService* svc = create_trl_service();
    PollMgr* poll = new PollMgr(1);
    ThreadPool* thrpool = new ThreadPool(1);
    Server *server = new Server(poll, thrpool);
    server->reg(svc);

    int ret_code = 0;
    if (server->start(bind_addr.c_str()) != 0) {
        ret_code = 1;
        g_stop_flag = true;
    }

    while (g_stop_flag == false) {
        sleep(1);
    }
    delete server;
    delete svc;
    poll->release();
    thrpool->release();

    return 0;
}

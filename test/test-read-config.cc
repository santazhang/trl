#include "base/all.h"
#include "INIReader.h"
#include "utils.h"

using namespace base;
using namespace std;
using namespace trl;

TEST(read_config, config_ini) {
    INIReader ini("conf/config.ini");
    EXPECT_EQ(ini.ParseError(), 0);
    Log::debug("config.ini: active_servers = %d", ini.GetInteger("", "active_servers", -1));
    Log::debug("config.ini: records_per_server = %d", ini.GetInteger("", "records_per_server", -1));
}

TEST(read_config, servers_txt) {
    for (auto& it : server_list()) {
        Log::debug("servers.txt: %s", it.c_str());
    }
}

#include <fstream>

#include "utils.h"

using namespace std;

namespace trl {

std::vector<std::string> server_list() {
    vector<string> servers;
    ifstream fin("conf/servers.txt");
    string line;
    while (getline(fin, line)) {
        if (line == "" || line[0] == '#') {
            continue;
        }
        servers.push_back(line);
    }
    verify(servers.size() > 0);
    return servers;
}

} // namespace trl

#include "include/network.h"

network::network(uint32_t network_addr, uint32_t network_mask)
     : net_addr_(network_addr), net_mask_(network_mask)
{

}

network network::from_string(const std::string& net_str)
{
    std::cmatch cm;
    std::regex_match(net_str.c_str(), cm, network_regexp);

    if(cm.size() != 6)
        throw std::invalid_argument("invalid network address");

    uint32_t net_addr = static_cast<uint32_t>(std::stoi(cm[1]) << 24 | std::stoi(cm[2]) << 16 |
                                              std::stoi(cm[3]) << 8 | std::stoi(cm[4]));
    uint32_t net_mask = 0;
    int i;
    for(i = 0; i < std::stoi(cm[5]); ++i)
    {
        net_mask |= 1 << i;
    }
    net_mask <<= 32 - i;
    return network(net_addr & net_mask, net_mask);
}

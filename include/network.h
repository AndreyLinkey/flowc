#ifndef NETWORK_H
#define NETWORK_H

#include <cstdint>
#include <regex>

const std::regex network_regexp("^(25[0-5]|2[0-4][0-9]|[01]?[0-9]{1,2})\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9]{1,2})\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9]{1,2})\\."
                                 "(25[0-5]|2[0-4][0-9]|[01]?[0-9]{1,2})\\/([0-2]?[0-9]|3[0-2])$");

class network
{
public:
    network() = delete;
    static network from_string(const std::string& net_str);
    uint32_t net_addr() const {return net_addr_;}
    uint32_t net_mask() const {return net_mask_;}

private:
    network(uint32_t network_addr, uint32_t network_mask);

    uint32_t net_addr_;
    uint32_t net_mask_;
};



#endif // NETWORK_H

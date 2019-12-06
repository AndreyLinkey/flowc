#ifndef FILTER_H
#define FILTER_H

#include <algorithm>
#include <functional>
#include <unordered_map>
#include <mutex>
#include <string>
#include <vector>

#include "field.h"
#include "network.h"

class filter
{
public:
    filter(std::vector<network> networks, time_t timeout);
    bool check_flow(const flow_data& flow);

private:
    filter();
    void erase_(uint32_t timestamp);
    using map_cache = std::unordered_map<flow_data, uint32_t, flow_hasher>;
    map_cache flow_cache_;
    uint32_t last_erase_time_;
    std::vector<network> networks_;
    time_t timeout_;
    mutable std::mutex mtx_;
};

#endif // FILTER_H

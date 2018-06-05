#ifndef FILTER_H
#define FILTER_H

#include <functional>
#include <set>
#include <tuple>
#include <vector>

#include "field.h"

const unsigned char DUPLICATE_TIMEOUT = 12;

typedef std::tuple<uint32_t, uint32_t, uint32_t> cache_item;

class filter
{
public:
    filter();
    bool check_flow(flow_data flow);

private:
    std::uint32_t last_timestamp_;
    std::set<cache_item> flow_cache_;
    std::vector<std::vector<cache_item>> flow_timeouts_;
};

#endif // FILTER_H

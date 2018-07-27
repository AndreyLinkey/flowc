#include "include/filter.h"

filter::filter(std::vector<network> networks, time_t timeout)
    : networks_(networks), timeout_(timeout), last_erase_time_(0)
{

}

bool filter::check_flow(const flow_data& flow)
{
    if(find_if(networks_, [&flow](const network& net){return net.net_addr() == (flow.ip_src_addr & net.net_mask());}) == networks_.end())
    {
        return false;
    }

    std::lock_guard<std::mutex> lock(mtx_);
    if(last_erase_time_ < flow.timestamp)
    {
        erase_(flow.timestamp);
        last_erase_time_ = flow.timestamp;
    }

    if(flow_cache_.left.find(flow) != flow_cache_.left.end())
    {
        return false;
    }

    flow_cache_.left.insert(bimap_cache::left_value_type(flow, flow.timestamp));
    return true;
}

filter::filter()
    : last_erase_time_(0)
{

}

void filter::erase_(uint32_t timestamp)
{
    std::vector<uint32_t> erased;
    copy(
        flow_cache_.left
            | filtered([this, timestamp](const bimap_cache::left_value_type lval){return timestamp - lval.second > timeout_;})
            | transformed([](const bimap_cache::left_value_type lval){return lval.second;})
            | uniqued,
        std::back_inserter(erased));

    for_each(erased, [this](int item){flow_cache_.right.erase(item);});
}

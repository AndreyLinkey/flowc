#include "include/filter.h"

filter::filter(std::vector<network> networks, time_t timeout)
    : last_erase_time_(0), networks_(networks), timeout_(timeout)
{

}

filter::filter()
    : last_erase_time_(0)
{

}

bool filter::check_flow(const flow_data& flow)
{
    if(std::none_of(networks_.begin(), networks_.end(),
                    [&flow](const network& net){return net.net_addr() == (flow.ip_src_addr & net.net_mask());}))
    {
        return false;
    }

//    if(find_if(networks_, [&flow](const network& net){return net.net_addr() == (flow.ip_src_addr & net.net_mask());}) == networks_.end())
//    {
//        return false;
//    }

    std::lock_guard<std::mutex> lock(mtx_);
    if(last_erase_time_ < flow.timestamp)
    {
        erase_(flow.timestamp);
        last_erase_time_ = flow.timestamp;
    }

//    if(std::none_of(flow_cache_.begin(), flow_cache_.end(),
//                    [&flow](const map_cache::value_type& val){return val.first == flow;}))
//    {
//        flow_cache_.insert(map_cache::value_type(flow, flow.timestamp));
//        return true;
//    }
//    return false;

    if(flow_cache_.find(flow) != flow_cache_.end())
    {
        return false;
    }

    flow_cache_.insert(map_cache::value_type(flow, flow.timestamp));
    return true;
}

void filter::erase_(uint32_t timestamp)
{
    time_t min_timestamp = timestamp - timeout_;

//    std::vector<uint32_t> erased;
//    copy(
//        flow_cache_.left
//            | filtered([this, min_timestamp](const bimap_cache::left_value_type lval){return lval.second < min_timestamp;})
//            | transformed([](const bimap_cache::left_value_type lval){return lval.second;})
//            | uniqued,
//        std::back_inserter(erased));

//    for_each(erased, [this](int item){flow_cache_.right.erase(item);});

    map_cache::iterator it = flow_cache_.begin();
    while(it != flow_cache_.end())
    {
        if(it->second < min_timestamp)
            it = flow_cache_.erase(it);
        else
            ++it;
    }
}

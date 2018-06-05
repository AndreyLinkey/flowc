#include "include/filter.h"

filter::filter()
    : last_timestamp_(0), flow_timeouts_(DUPLICATE_TIMEOUT)//, hits(0)
{

}

bool filter::check_flow(flow_data flow)
{
    std::uint32_t new_timestamp = flow.timestamp % DUPLICATE_TIMEOUT;

    for(uint32_t idx = (last_timestamp_ + 1) % DUPLICATE_TIMEOUT; idx <= new_timestamp; ++idx)
    {
        for(cache_item& it : flow_timeouts_[idx])
        {
            flow_cache_.erase(it);
        }
        flow_timeouts_[idx].clear();
    }
    last_timestamp_ = new_timestamp;

    cache_item item{flow.ip_src_addr, flow.ip_dst_addr, flow.postnat_src_addr};
    if(flow_cache_.find(item) != flow_cache_.end())
    {
        return false;
    }
    flow_cache_.insert(item);
    flow_timeouts_[last_timestamp_].push_back(item);
    return true;
}

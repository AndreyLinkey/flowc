#ifndef FILTER_H
#define FILTER_H

#include <algorithm>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <vector>
//#include <boost/bimap.hpp>
//#include <boost/bimap/set_of.hpp>
//#include <boost/bimap/multiset_of.hpp>
//#include <boost/range/algorithm.hpp>
//#include <boost/range/adaptors.hpp>

#include "field.h"
#include "network.h"

//using boost::adaptors::filtered;
//using boost::adaptors::transformed;
//using boost::adaptors::uniqued;
//using boost::range::copy;
//using boost::range::find_if;
//using boost::range::for_each;
//using boost::range::remove_if;

class filter
{
public:
    filter(std::vector<network> networks, time_t timeout);
    bool check_flow(const flow_data &flow);

private:
    filter();
    void erase_(uint32_t timestamp);
//    using bimap_cache = boost::bimap<boost::bimaps::set_of<flow_data>,
//                                     boost::bimaps::multiset_of<uint32_t>>;
//    bimap_cache flow_cache_;
    using map_cache = std::map<flow_data, uint32_t>;
    map_cache flow_cache_;
    uint32_t last_erase_time_;
    std::vector<network> networks_;
    time_t timeout_;
    mutable std::mutex mtx_;

};

#endif // FILTER_H

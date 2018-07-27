#include "include/field.h"

//bool flow_data::operator==(const flow_data& other) const
//{
//    return (ip_dst_addr == other.ip_dst_addr &&
//            ip_src_addr == other.ip_src_addr &&
//            postnat_src_addr == other.postnat_src_addr);
//}

bool flow_data::operator<(const flow_data& other) const
{
    return std::tie(ip_src_addr, ip_dst_addr, postnat_src_addr) <
           std::tie(other.ip_src_addr, other.ip_dst_addr, other.postnat_src_addr);

//    return (ip_dst_addr < other.ip_dst_addr ||
//            ip_src_addr < other.ip_src_addr ||
//            postnat_src_addr < other.postnat_src_addr);
}

uint16_t packtwo2int(raw_data::const_iterator data_it)
{
    return *data_it << 8 | *(data_it + 1);
}

uint32_t packfour2int(raw_data::const_iterator data_it)
{
    return *data_it << 24 | *(data_it + 1) << 16 | *(data_it + 2) << 8 | *(data_it + 3);
}

void ip_src_addr_parser(raw_data::const_iterator data_it, flow_data &values)
{
    values.ip_src_addr = packfour2int(data_it);
}

void ip_dst_addr_parser(raw_data::const_iterator data_it, flow_data &values)
{
    values.ip_dst_addr = packfour2int(data_it);
}

void postnat_src_addr_parser(raw_data::const_iterator data_it, flow_data &values)
{
    values.postnat_src_addr = packfour2int(data_it);
}

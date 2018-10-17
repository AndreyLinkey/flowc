#include "include/field.h"

bool flow_data::operator==(const flow_data& other) const
{
    return (ip_dst_addr == other.ip_dst_addr &&
            ip_src_addr == other.ip_src_addr &&
            postnat_src_addr == other.postnat_src_addr);
}

bool flow_data::operator<(const flow_data& other) const
{
//    return std::tie(ip_src_addr, ip_dst_addr, postnat_src_addr) <
//           std::tie(other.ip_src_addr, other.ip_dst_addr, other.postnat_src_addr);

    if(ip_src_addr != other.ip_src_addr)
    {
        return ip_src_addr < other.ip_src_addr;
    }
    if(ip_dst_addr != other.ip_dst_addr)
    {
        return ip_dst_addr < other.ip_dst_addr;
    }
    if(postnat_src_addr != other.postnat_src_addr)
    {
        return postnat_src_addr < other.postnat_src_addr;
    }
    return false;
}

void ip_src_addr_parser(raw_data::const_iterator data_it, flow_data& values)
{
    values.ip_src_addr = pack_be_to_uint32(data_it);
}

void ip_dst_addr_parser(raw_data::const_iterator data_it, flow_data& values)
{
    values.ip_dst_addr = pack_be_to_uint32(data_it);
}

void postnat_src_addr_parser(raw_data::const_iterator data_it, flow_data& values)
{
    values.postnat_src_addr = pack_be_to_uint32(data_it);
}

#include "include/field.h"

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

#ifndef FIELD_H
#define FIELD_H

#include <cstdint>
#include <map>
#include <tuple>
#include <vector>

#include "defaults.h"

enum field_type
{
    ip_src_addr = 8,
    ip_dst_addr = 12,
    first_switched = 22,
    ip_protocol_version = 60,
    postnat_src_addr = 225,
    postnat_dst_addr = 226
};

struct flow_data
{
    bool operator==(const flow_data& other) const;
    bool operator<(const flow_data& other) const;

    uint32_t timestamp;
    uint32_t ip_src_addr;
    uint32_t ip_dst_addr;
    uint32_t postnat_src_addr;
};

using raw_data = std::vector<unsigned char>;
using field_handler = void (&)(raw_data::const_iterator, flow_data&);

uint16_t packtwo2int(raw_data::const_iterator data_it);
uint32_t packfour2int(raw_data::const_iterator data_it);

void ip_src_addr_parser(raw_data::const_iterator data_it, flow_data& values);
void ip_dst_addr_parser(raw_data::const_iterator data_it, flow_data& values);
void postnat_src_addr_parser(raw_data::const_iterator data_it, flow_data& values);

using fields_t = std::map<field_type, field_handler>;
static const fields_t fields
{
    {ip_src_addr, ip_src_addr_parser},
    {ip_dst_addr, ip_dst_addr_parser},
    {postnat_src_addr, postnat_src_addr_parser}
};

#endif // FIELD_H

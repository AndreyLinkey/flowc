#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "field.h"
#include "templates.h"

const uint16_t IPFIX_VERSION = 10;
const uint16_t IPFIX_HEADER_LENGTH = 16;
const uint16_t IPFIX_VERSION_OFFSET = 0;
const uint16_t IPFIX_LENGTH_OFFSET = 2;
const uint16_t IPFIX_TIMESTAMP_OFFSET = 4;
const uint16_t IPFIX_SET_HEADER_LENGTH = 4;
const uint16_t IPFIX_SET_ID_OFFSET = 0;
const uint16_t IPFIX_SET_LENGTH_OFFSET = 2;

const uint16_t IPFIX_TEMPLATE_HEADER_LENGTH = 4;
const uint16_t IPFIX_TEMPLATE_ID_OFFSET = 0;
const uint16_t IPFIX_TEMPLATE_COUNT_OFFSET = 2;

const uint16_t IPFIX_TEMPLATE_FIELD_LENGTH = 4;
const uint16_t IPFIX_TEMPLATE_FIELD_TYPE_OFFSET = 0;
const uint16_t IPFIX_TEMPLATE_FIELD_LENGTH_OFFSET = 2;

const std::uint16_t TEMPLATE_SET_ID = 2;


class parser
{
public:
    parser(raw_data& data);
    std::vector<flow_data> flows();

private:
    struct set_info
    {
        uint16_t id;
        uint16_t length;
        uint16_t offset;
        flow_template_ptr templ_ptr;
    };

    static template_storage templates_;
    void parse_templates_();

    raw_data& data_;
    uint32_t timestamp_;
    std::vector<set_info> sets_;

};

#endif // PARSER_H

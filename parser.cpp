#include "include/parser.h"

parser::parser(raw_data& data)
    : data_(data)
{
    raw_data::const_iterator data_it = data_.cbegin();
    if(packtwo2int(data_it + IPFIX_VERSION_OFFSET) != IPFIX_VERSION)
        throw std::invalid_argument("version invalid");

    uint16_t data_length = packtwo2int(data_it + IPFIX_LENGTH_OFFSET);
    if(data_length > data_.size())
        throw std::invalid_argument("buffer too short");
    timestamp_ = packfour2int(data_it + IPFIX_TIMESTAMP_OFFSET);

    std::cout << "length=" << data_length << " time=" << timestamp_ << std::endl;

    uint16_t set_offset = IPFIX_HEADER_LENGTH;
    while(set_offset < data_length)
    {
        set_info set;
        set.offset = set_offset;
        set.id = packtwo2int(data_it + set_offset + IPFIX_SET_ID_OFFSET);
        set.length = packtwo2int(data_it + set_offset + IPFIX_SET_LENGTH_OFFSET);
        set.templ_ptr = templates_.template_ptr(set.id);
        sets_.push_back(set);
        set_offset += set.length;
        std::cout << "new set!!! id=" << set.id << " length=" << set.length << " offset=" << set.offset << std::endl;
    }

    if(set_offset != data_length)
        throw std::invalid_argument("message length is not equal sum of sets length");

    parse_templates_();
}

template_storage parser::templates_;

void parser::parse_templates_()
{
    std::cout << "parsing template" << std::endl;
    for(set_info set : sets_)
    {
        std::cout << "processing set: id=" << set.id << " length=" << set.length << " offset=" << set.offset << std::endl;
        if(set.id != 2)
        {
            std::cout << "isn't template" << std::endl;
            continue;
        }

        std::cout << "parsing template " << set.offset << " offset" << std::endl;
        raw_data::const_iterator data_it = data_.cbegin() + set.offset;
        std::cout << "iteraror set to " << set.offset << std::endl;
        uint16_t template_offset = IPFIX_SET_HEADER_LENGTH;
        std::cout << "offset variable " << template_offset << std::endl;
        while(template_offset < set.length)
        {
            uint16_t templ_id = packtwo2int(data_it + template_offset + IPFIX_TEMPLATE_ID_OFFSET);
            std::cout << "new flow template: descriptor " << templ_id << std::endl;
            uint16_t field_count = packtwo2int(data_it + template_offset + IPFIX_TEMPLATE_COUNT_OFFSET);
            std::cout << "field count " << field_count << std::endl;

            template_offset += IPFIX_TEMPLATE_HEADER_LENGTH;
            std::cout << "offset variable " << template_offset << std::endl;
            if(template_offset + IPFIX_TEMPLATE_FIELD_LENGTH * field_count > set.length)
                throw std::invalid_argument("template field count error");

            std::cout << "processing fields" << std::endl;
            flow_template ftemplate;
            ftemplate.length = 0;
            for(uint16_t idx = 0; idx < field_count; ++idx)
            {
                field_type type = static_cast<field_type>(packtwo2int(data_it + template_offset + IPFIX_TEMPLATE_FIELD_TYPE_OFFSET));
                uint16_t length = packtwo2int(data_it + template_offset + IPFIX_TEMPLATE_FIELD_LENGTH_OFFSET);
                std::cout << "field type is " << type << std::endl;

                if(fields.find(type) != fields.end())
                {
                    std::cout << "new field" << std::endl;
                    template_field field;
                    field.type = type;
                    field.length = length;
                    field.offset = ftemplate.length;
                    ftemplate.fields.push_back(field);
                    std::cout << "field added: type=" << field.type << " length=" << field.length << " offset=" << field.offset << std::endl;
                }
                template_offset += IPFIX_TEMPLATE_FIELD_LENGTH;
                ftemplate.length += length;
                std::cout << "offset variable " << template_offset << std::endl;
            }
            if(fields.size() == ftemplate.fields.size())
            {
                templates_.add_template(templ_id, ftemplate);
                std::cout << "template " << templ_id << " added, length is " << ftemplate.length << std::endl;
            }
        }
    }
}

std::vector<flow_data> parser::flows()
{
    std::vector<flow_data> sets_data;

    for(set_info set : sets_)
    {
        if(set.id < 256)
            continue;
        if(!set.templ_ptr)
            continue;
        flow_template templ = *set.templ_ptr;

        raw_data::const_iterator data_it = data_.cbegin() + set.offset;
        uint16_t flow_offset = IPFIX_SET_HEADER_LENGTH;
        while(flow_offset < set.length)
        {
            flow_data data;
            data.timestamp = timestamp_;
            for(template_field field: templ.fields)
            {
                uint16_t offset = flow_offset + field.offset;
                fields.at(field.type)(data_it + offset, data);
            }
            sets_data.push_back(data);
            flow_offset += templ.length;
        }
    }
    return sets_data;
}

/*
bool parser::check_temlate(flow_data& data)
{
    if(data.size() < (IPFIX_HEADER_LENGTH + IPFIX_SET_HEADER_LENGTH))
        throw std::invalid_argument("message too short");

    flow_data::const_iterator set_id_it = data.cbegin() + IPFIX_HEADER_LENGTH + IPFIX_SET_ID_OFFSET;
    const uint16_t set_id = packtwo2int(set_id_it);
    if(set_id != TEMPLATE_SET_ID)
        return false;
    return true;
}
*/

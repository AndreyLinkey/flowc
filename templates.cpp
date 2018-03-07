
#include "include/templates.h"

template_storage::template_storage()
{

}

void template_storage::add_template(uint16_t id, flow_template ftemplate)
{
    templates_[id] = std::make_shared<flow_template>(ftemplate);
}

flow_template_ptr template_storage::template_ptr(uint16_t id) const
{
    if(templates_.find(id) != templates_.end())
        return templates_.at(id);
    return {};
}

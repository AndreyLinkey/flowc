#ifndef TEMPLATES_H
#define TEMPLATES_H

#include <vector>
#include <map>
#include <memory>
#include <utility>
#include <boost/circular_buffer.hpp>

#include "defaults.h"
#include "field.h"

struct template_field
{
    field_type type;
    uint16_t length;
    uint16_t offset;
};

struct flow_template
{
    std::vector<template_field> fields;
    uint16_t length;
};

using flow_template_ptr = std::shared_ptr<flow_template>;
using templates = std::map<uint16_t, flow_template_ptr>;

class template_storage
{
public:
    template_storage();
    void add_template(uint16_t id, flow_template ftemplate);
    flow_template_ptr template_ptr(uint16_t id) const;

private:
    templates templates_;

};

#endif // TEMPLATES_H

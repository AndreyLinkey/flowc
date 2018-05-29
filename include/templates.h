#ifndef TEMPLATES_H
#define TEMPLATES_H

#include <vector>
#include <map>
#include <memory>
#include <utility>
#include <boost/circular_buffer.hpp>

#include "field.h"

static const size_t TEMPLATE_STORAGE_SIZE = 10;

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

typedef std::shared_ptr<flow_template> flow_template_ptr;
typedef std::map<uint16_t, flow_template_ptr> templates;


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

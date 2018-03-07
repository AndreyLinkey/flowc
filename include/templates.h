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





/*
void print(boost::circular_buffer<std::string>& cb)
{
    std::cout << cb.size() << " front: " << cb.front() << " / back " << cb.back() << " / is_full " << cb.full() << std::endl;
}

int main(int argc, char* argv[])
{
    std::string test("0_rrey33th");

    boost::circular_buffer<std::string> cb;
    cb.set_capacity(3);
    cb.push_front(std::string("1_dsgsgsg"));
    print(cb);
    cb.push_front(std::string("2_ewjw232"));
    print(cb);

    std::swap(cb.back(), test);

    cb.pop_back();
    print(cb);
    cb.push_front(std::string("3_rfgwgwb"));
    print(cb);
    cb.push_front(std::string("4_dvfdbgb"));
    print(cb);
    cb.push_front(std::string("5_hnghmmg"));
    print(cb);

    std::cout << test << std::endl;
}*/

#endif // TEMPLATES_H

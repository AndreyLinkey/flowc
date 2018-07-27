#ifndef HANDLER_H
#define HANDLER_H

#include <iomanip>
#include <iostream>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <boost/asio.hpp>

#include "container.h"
#include "field.h"
#include "filter.h"
#include "network.h"
#include "parser.h"
#include "settings.h"
#include "swappable_circular_buffer.h"
#include "transaction.h"

typedef swappable_circular_buffer<raw_data> flow_buffer;

const uint32_t RECORDS_COUNT = 128;

class handler
{
public:
    handler(flow_buffer& buffer, filter& flt, container& cont, const settings& conf_opts, int idx = 0);
    void run(std::mutex& buffer_access, std::condition_variable& data_ready);
    void set_table(const std::string &table_name);
    void terminate();

private:
    flow_buffer& buffer_;
    filter& flt_;
    container& cont_;
    const settings& conf_opts_;
    raw_data data_;
    volatile bool process_;
    int idx_;
    std::string table_;
};

#endif // HANDLER_H

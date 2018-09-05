#ifndef HANDLER_H
#define HANDLER_H

#include <iomanip>
#include <iostream>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <boost/asio.hpp>

#include "container.h"
#include "defaults.h"
#include "field.h"
#include "filter.h"
#include "logger.h"
#include "network.h"
#include "parser.h"
#include "settings.h"
#include "swappable_circular_buffer.h"

using flow_buffer = swappable_circular_buffer<raw_data>;

class handler
{
public:
    handler(flow_buffer& buffer, filter& flt, container& cont, const settings& conf_opts, int idx = 0);
    void run(std::mutex& buffer_access, std::condition_variable& data_ready);
    void terminate();

private:
    flow_buffer& buffer_;
    filter& flt_;
    container& cont_;
    const settings& conf_opts_;
    raw_data data_;
    volatile bool process_;
    int idx_;
};

#endif // HANDLER_H

#ifndef HANDLER_H
#define HANDLER_H

#include <iomanip>
#include <iostream>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <boost/asio.hpp>

#include "field.h"
#include "parser.h"
#include "swappable_circular_buffer.h"
#include "transaction.h"

typedef swappable_circular_buffer<raw_data> flow_buffer;

class handler
{
public:
    handler(std::reference_wrapper<flow_buffer> buffer, std::reference_wrapper<std::mutex> buffer_access,
            std::reference_wrapper<std::condition_variable> data_ready, std::reference_wrapper<connection_info> conn_info,
            size_t data_length, int idx = 0);
    void run();
    void terminate();

private:
    bool check_addr_(uint32_t addr);
    std::reference_wrapper<flow_buffer> buffer_;
    std::reference_wrapper<std::mutex> buffer_access_;
    std::reference_wrapper<std::condition_variable> data_ready_;
    std::reference_wrapper<connection_info> conn_info_;
    raw_data data_;
    volatile bool process_;
    int idx_;
};

#endif // HANDLER_H

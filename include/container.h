#ifndef CONTAINER_H
#define CONTAINER_H

#include <condition_variable>
#include <fstream>
#include <mutex>
#include <vector>
#include <stdexcept>
#include <swappable_circular_buffer.h>

#include "byte_order.h"
#include "field.h"
#include "logger.h"

const size_t BEG_TIMESTAMP_OFFSET = 0;
const size_t IP_SRC_ADDR_OFFSET = 4;
const size_t IP_DST_ADDR_OFFSET = 8;
const size_t POSTNAT_SRC_ADDR_OFFSET = 12;
const size_t FLOW_RECORD_LENGTH = 16;

class container
{
public:
    container(size_t queue_length = 0);
    ~container();
    void open_file(std::string &&file_name, char mode);
    void store_flows(std::vector<flow_data>& flows);
    std::vector<flow_data> read_flows(size_t count);
    void run();
    void terminate();
    size_t buff_size();


private:
    void flush_buffer_();
    void store_flow_(flow_data& flow);
    flow_data read_flow_();

    using buffer = swappable_circular_buffer<std::vector<flow_data>>;
    buffer buff_;
    std::condition_variable cond_;
    std::mutex mtx_;
    std::fstream file_;
    std::streamoff file_len_;
    volatile bool process_;
};


#endif // CONTAINER_H

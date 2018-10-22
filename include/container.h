#ifndef CONTAINER_H
#define CONTAINER_H

#include <condition_variable>
#include <iterator>
#include <fstream>
#include <mutex>
#include <vector>
#include <stdexcept>
#include <swappable_circular_buffer.h>

#include "byte_order.h"
#include "defaults.h"
#include "field.h"
#include "logger.h"

const size_t BEG_TIMESTAMP_OFFSET = 0;
const size_t IP_SRC_ADDR_OFFSET = 4;
const size_t IP_DST_ADDR_OFFSET = 8;
const size_t POSTNAT_SRC_ADDR_OFFSET = 12;
const std::streamoff FLOW_RECORD_LENGTH = 16;

struct container_settings
{
    container_settings()
    {
        signature = DUMP_SIGNATURE;
        time_delta = 0;
    }
    container_settings(const struct flow_data& other)
    {
        signature = other.timestamp;
        time_delta = other.ip_src_addr;
    }
    operator flow_data() const {return {signature, time_delta, 0, 0};}

    uint32_t signature;
    uint32_t time_delta;
    uint32_t reserved_0;
    uint32_t reserved_1;
};

class container
{
public:
    container(size_t queue_length = 0);
    ~container();
    void close_file();
    void create_file(std::string &&file_name);
    void open_file(std::string &&file_name);
    void run();
    void terminate();
    std::vector<flow_data> read_flows(size_t count, uint32_t start_from, uint32_t end_at,
                                      uint32_t ip_src_addr, uint32_t ip_dst_addr, uint32_t postnat_src_addr);
    void store_flows(std::vector<flow_data>& flows, uint32_t time_delta = 0);


private:
    void store_flow_(flow_data& flow);
    size_t read_data_(size_t flow_count, raw_data &buffer);
    flow_data read_flow_();
    void seek_by_timestamp_(uint32_t timestamp);

    using buffer = swappable_circular_buffer<std::vector<flow_data>>;
    buffer buff_;
    std::condition_variable cond_;
    std::mutex mtx_;
    std::fstream file_;
    volatile bool process_;
    container_settings settings_;
    std::streamoff file_len_;
    std::streamoff seeked_to_;
};


#endif // CONTAINER_H

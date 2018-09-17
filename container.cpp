#include "include/container.h"

container::container(size_t queue_length)
    : buff_(queue_length, 0), process_(true)
{

}

container::~container()
{
    file_.close();
}

void container::create_file(std::string &&file_name)
{
    std::lock_guard<std::mutex> lock(mtx_);
    flush_buffer_();
    file_.close();
    file_.open(file_name, std::ios::out | std::ios::binary);

    container_settings cont_sett;
    cont_sett.signature = DUMP_SIGNATURE;
    flow_data fd(cont_sett);
    store_flow_(fd);
}

void container::open_file(std::string &&file_name)
{
    file_.open(file_name, std::ios::in | std::ios::binary);
    file_.seekg(0, file_.end);
    file_len_ = file_.tellg();
    file_.seekg(0, file_.beg);

    container_settings cont_sett(read_flow_());
    if(cont_sett.signature != DUMP_SIGNATURE)
    {
        throw std::runtime_error(file_name + "is not flowc dump");
    }
}

void container::store_flows(std::vector<flow_data>& flows)
{
    std::lock_guard<std::mutex> lock(mtx_);
    if(!file_.is_open())
    {
        throw std::invalid_argument("output file closed");
    }

    buff_.swap_head(flows);
    cond_.notify_all();
}

std::vector<flow_data> container::read_flows(size_t count)
{
    if(!file_.is_open())
    {
        throw std::invalid_argument("source file closed");
    }

    std::vector<flow_data> flows;
    flows.reserve(count);

    //std::streamoff pos = file_.tellg();

     while(file_.tellg() < file_len_ && flows.size() < count)
     {
         flows.push_back(read_flow_());
         //pos = file_.tellg();
     }

     return flows;
}

void container::run() try
{
    std::vector<flow_data> flows;
    std::unique_lock<std::mutex> lock(mtx_);
    while(true)
    {
        while(buff_.size() > 0)
        {
            buff_.swap_tail(flows);
            lock.unlock();
            for(flow_data flow: flows)
            {
                store_flow_(flow);
            }

            lock.lock();
        }
        if(!process_)
        {
            break;
        }
        cond_.wait(lock);
    }
}
catch (const std::exception &e)
{
    log_writer.write_log(e.what());
    throw e;
}

void container::terminate()
{
    std::lock_guard<std::mutex> lock(mtx_);
    process_ = false;
    cond_.notify_all();
}

size_t container::buff_size()
{
    return buff_.size();
}


void container::flush_buffer_()
{
    std::vector<flow_data> flows;
    while(buff_.size() > 0)
    {
        buff_.swap_tail(flows);
        for(flow_data flow: flows)
        {
            store_flow_(flow);
        }
    }
}

void container::store_flow_(flow_data& flow)
{
//    flow.timestamp = ByteOrder::to_little_endian(flow.timestamp);
//    flow.ip_src_addr = ByteOrder::to_little_endian(flow.ip_src_addr);
//    flow.ip_dst_addr = ByteOrder::to_little_endian(flow.ip_dst_addr);
//    flow.postnat_src_addr = ByteOrder::to_little_endian(flow.postnat_src_addr);

//    file_.write(reinterpret_cast<char*>(&flow.timestamp), 4);
//    file_.write(reinterpret_cast<char*>(&flow.ip_src_addr), 4);
//    file_.write(reinterpret_cast<char*>(&flow.ip_dst_addr), 4);
//    file_.write(reinterpret_cast<char*>(&flow.postnat_src_addr), 4);

    uint32_t out[] = {ByteOrder::to_little_endian(flow.timestamp), ByteOrder::to_little_endian(flow.ip_src_addr),
                      ByteOrder::to_little_endian(flow.ip_dst_addr), ByteOrder::to_little_endian(flow.postnat_src_addr)};
    file_.write(reinterpret_cast<char*>(out), 16);

    //file_.flush();
}

flow_data container::read_flow_()
{
    flow_data flow;

    file_.read(reinterpret_cast<char*>(&flow.timestamp), 4);
    file_.read(reinterpret_cast<char*>(&flow.ip_src_addr), 4);
    file_.read(reinterpret_cast<char*>(&flow.ip_dst_addr), 4);
    file_.read(reinterpret_cast<char*>(&flow.postnat_src_addr), 4);
    flow.timestamp = ByteOrder::from_little_endian(flow.timestamp);
    flow.ip_src_addr = ByteOrder::from_little_endian(flow.ip_src_addr);
    flow.ip_dst_addr = ByteOrder::from_little_endian(flow.ip_dst_addr);
    flow.postnat_src_addr = ByteOrder::from_little_endian(flow.postnat_src_addr);

    return flow;
}

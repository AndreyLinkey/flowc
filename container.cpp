#include "include/container.h"

//container::container()
//{


//}

container::~container()
{
    file_.close();
}

void container::open_file(std::string& file_name, char mode)
{
    std::lock_guard<std::mutex> lock(mtx_);
    file_.close();

    switch (mode) {
    case 'w':
        file_.open(file_name, std::ios::out | std::ios::binary);
        break;
    case 'r':
        file_.open(file_name, std::ios::in | std::ios::binary);
        file_.seekg(0, file_.end);
        file_len_ = file_.tellg();
        file_.seekg(0, file_.beg);
        break;
    default:
        throw std::invalid_argument("unknown mode");
        break;
    }
}

void container::store_flows(std::vector<flow_data>& flows)
{
    std::lock_guard<std::mutex> lock(mtx_);
    if(!file_.is_open())
    {
        throw std::invalid_argument("output file closed");
    }

//    buff_.swap_tail(flows);

    for(flow_data flow: flows)
    {
        store_flow_(flow);
    }
//    cond_.notify_one();
}

std::vector<flow_data> container::read_flows(size_t count)
{
    if(!file_.is_open())
    {
        throw std::invalid_argument("source file closed");
    }

    std::vector<flow_data> flows;
    flows.reserve(count);

    size_t pos = file_.tellg();

     while(file_.tellg() < file_len_ && flows.size() < count)
     {
         flows.push_back(read_flow_());
         pos = file_.tellg();

     }

     return flows;
}

void container::store_flow_(flow_data& flow)
{
    flow.timestamp = ByteOrder::to_little_endian(flow.timestamp);
    flow.ip_src_addr = ByteOrder::to_little_endian(flow.ip_src_addr);
    flow.ip_dst_addr = ByteOrder::to_little_endian(flow.ip_dst_addr);
    flow.postnat_src_addr = ByteOrder::to_little_endian(flow.postnat_src_addr);
    file_.write((char*)(&flow.timestamp), 4);
    file_.write((char*)(&flow.ip_src_addr), 4);
    file_.write((char*)(&flow.ip_dst_addr), 4);
    file_.write((char*)(&flow.postnat_src_addr), 4);
}

flow_data container::read_flow_()
{
    flow_data flow;

    file_.read((char*)(&flow.timestamp), 4);
    file_.read((char*)(&flow.ip_src_addr), 4);
    file_.read((char*)(&flow.ip_dst_addr), 4);
    file_.read((char*)(&flow.postnat_src_addr), 4);
    flow.timestamp = ByteOrder::from_little_endian(flow.timestamp);
    flow.ip_src_addr = ByteOrder::from_little_endian(flow.ip_src_addr);
    flow.ip_dst_addr = ByteOrder::from_little_endian(flow.ip_dst_addr);
    flow.postnat_src_addr = ByteOrder::from_little_endian(flow.postnat_src_addr);

    return flow;
}



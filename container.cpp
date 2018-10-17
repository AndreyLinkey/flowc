#include "include/container.h"

container::container(size_t queue_length)
    : buff_(queue_length, 0), process_(true), seeked_to_(0)
{

}

container::~container()
{
    close_file();
}

void container::close_file()
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
    file_.seekg(0, file_.beg);
    flow_data fd(settings_);
    store_flow_(fd);

    file_.close();
}

void container::create_file(std::string &&file_name)
{
    std::lock_guard<std::mutex> lock(mtx_);
    close_file();
    file_.open(file_name, std::ios::out | std::ios::binary);

    settings_.time_delta = 0;
    flow_data fd(settings_);
    store_flow_(fd);
}

void container::open_file(std::string &&file_name)
{
    file_.open(file_name, std::ios::in | std::ios::binary);
    file_.seekg(0, file_.end);
    file_len_ = file_.tellg();
    file_.seekg(0, file_.beg);

    if(file_len_ < FLOW_RECORD_LENGTH)
    {
        throw std::runtime_error(file_name + " has no data");
    }

    container_settings cont_sett(read_flow_());
    if(cont_sett.signature != DUMP_SIGNATURE)
    {
        throw std::runtime_error(file_name + " is not flowc dump");
    }
    settings_ = cont_sett;
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

std::vector<flow_data> container::read_flows(size_t count, uint32_t start_from, uint32_t end_at)
{
    if(!file_.is_open())
    {
        throw std::invalid_argument("source file closed");
    }

    if(seeked_to_ != start_from)
    {
        seek_by_timestamp_(start_from);
        seeked_to_ = start_from;
    }

    std::vector<flow_data> flows;
    flows.reserve(count);

    raw_data buffer(count * FLOW_RECORD_LENGTH);
    size_t readed = 0;
    do
    {
        size_t readed = read_data_(count, buffer);
        //std::cout << "readed " << readed << " buffer size " << buffer.size() << std::endl;

        for(raw_data::const_iterator it = buffer.begin(); it < buffer.begin() + readed * FLOW_RECORD_LENGTH; it += FLOW_RECORD_LENGTH)
        {
            //std::cout << std::hex << (int)*it << (int)*(it + 1) << (int)*(it + 2) << (int)*(it + 3) << std::dec << std::endl;

            uint32_t timestamp = ByteOrder::from_little_endian(pack_le_to_uint32(it + BEG_TIMESTAMP_OFFSET));
            //std::cout << "timestamp=" << timestamp << " start_from=" << start_from << " end_at=" << end_at << std::endl;
            if(timestamp < start_from and timestamp >= end_at)
                continue;

            if(timestamp >= end_at + settings_.time_delta)
                break;

            flow_data fd;
            fd.timestamp = timestamp;
            fd.ip_src_addr = ByteOrder::from_little_endian(pack_le_to_uint32(it + IP_SRC_ADDR_OFFSET));
            fd.ip_dst_addr = ByteOrder::from_little_endian(pack_le_to_uint32(it + IP_DST_ADDR_OFFSET));
            fd.postnat_src_addr = ByteOrder::from_little_endian(pack_le_to_uint32(it + POSTNAT_SRC_ADDR_OFFSET));
            flows.push_back(fd);
        }
    }
    while(readed > 0 && flows.size() < count);

    return flows;
}

void container::store_flows(std::vector<flow_data>& flows, std::uint32_t time_delta)
{
    std::lock_guard<std::mutex> lock(mtx_);
    if(!file_.is_open())
    {
        throw std::invalid_argument("output file closed");
    }
    if(time_delta > settings_.time_delta)
    {
        settings_.time_delta = time_delta;
    }
    buff_.swap_head(flows);
    cond_.notify_all();
}

void container::store_flow_(flow_data& flow)
{
    uint32_t out[] = {ByteOrder::to_little_endian(flow.timestamp), ByteOrder::to_little_endian(flow.ip_src_addr),
                      ByteOrder::to_little_endian(flow.ip_dst_addr), ByteOrder::to_little_endian(flow.postnat_src_addr)};
    file_.write(reinterpret_cast<char*>(out), 16);
}

size_t container::read_data_(size_t flow_count, raw_data& buffer)
{
    std::size_t data_len = flow_count * FLOW_RECORD_LENGTH;
    std::streamoff bytes_remain = file_len_ - file_.tellg();
    if(bytes_remain < static_cast<std::streamoff>(data_len))
        data_len = static_cast<std::size_t>(bytes_remain);

    //buffer.resize(data_len);
    file_.read(reinterpret_cast<char*>(&buffer[0]), static_cast<std::streamoff>(data_len));

//    std::cout << "*** buffer size " << buffer.size() << " data_len " << data_len << std::endl
//              << std::hex;
//    int i = 0;
//    for(unsigned char c: buffer)
//    {
//        std::cout << std::setw(2) << std::setfill('0') << (int)c;
//        i++;
//        if(i != 0 && i % 16 == 0)
//            std::cout << '\n';
//    }
//    std::cout << std::dec << std::endl;

    return data_len / FLOW_RECORD_LENGTH;
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

void container::seek_by_timestamp_(uint32_t timestamp)
{
    const size_t with_delta = timestamp; //- settings_.time_delta;
    std::streamoff min_pos = FLOW_RECORD_LENGTH, max_pos = file_len_;
    std::streamoff pos = (max_pos - min_pos) / (2 * FLOW_RECORD_LENGTH) * FLOW_RECORD_LENGTH;

    while(true)
    {
        if(pos <= FLOW_RECORD_LENGTH || pos >= file_len_)
        {
            break;
        }

        file_.seekg(pos);
        flow_data fd = read_flow_();

        if(fd.timestamp < with_delta)
        {
            min_pos = pos;
            std::streamoff diff = max_pos - min_pos;
            if(diff == FLOW_RECORD_LENGTH)
            {
                break;
            }
            else
            {
                pos += diff / (2 * FLOW_RECORD_LENGTH) * FLOW_RECORD_LENGTH;
            }
        }
        else
        {
            max_pos = pos;
            std::streamoff diff = max_pos - min_pos;
            if(diff == FLOW_RECORD_LENGTH)
            {
                file_.seekg(min_pos);
                if(min_pos != FLOW_RECORD_LENGTH)
                {
                    file_.seekg(FLOW_RECORD_LENGTH, file_.cur);
                }
                break;
            }
            else
            {
                pos -= diff / (2 * FLOW_RECORD_LENGTH) * FLOW_RECORD_LENGTH;
            }
        }

        //std::cout << "pos=" << pos << " min_pos=" << min_pos << " max_pos=" << max_pos << std::endl;
    }

    //std::cout << std::hex << "seeked to " << file_.tellg() << std::dec << std::endl;
    //std::cout << "pos=" << pos << " min_pos=" << min_pos << " max_pos=" << max_pos << std::endl;
}


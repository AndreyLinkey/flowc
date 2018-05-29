#include "include/handler.h"

handler::handler(std::reference_wrapper<flow_buffer> buffer, std::reference_wrapper<std::mutex> buffer_access,
        std::reference_wrapper<std::condition_variable> data_ready, std::reference_wrapper<connection_info> conn_info,
        size_t data_length, int idx)
    : buffer_(buffer), buffer_access_(buffer_access), data_ready_(data_ready), data_(data_length), process_(true), conn_info_(conn_info), idx_(idx)
{
        std::cout << "create object" << std::endl;
}

void handler::run()
{
    std::unique_lock<std::mutex> data_ready_lock(buffer_access_.get());
    std::cout << "*thr" << idx_ << "* " << "hello from run" << std::endl;
    while(true)
    {
        connection db(conn_info_.get().to_string());
        std::cout << "*thr" << idx_ << "* " << "checking buffer size " << to_string(buffer_.get().size()) << std::endl;
        while(buffer_.get().size() != 0)
        {
            std::cout << "*thr" << idx_ << "* " << "there is somethig in buffer, swapping data" << std::endl;
            buffer_.get().swap_tail(data_);
            std::cout << "*thr" << idx_ << "* " << "performing data process, creating parser object" << std::endl;
            parser p(data_);
            std::cout << "*thr" << idx_ << "* " << "unlocking thread" << std::endl;
            data_ready_lock.unlock();
            std::vector<flow_data> flows = p.flows();
            pgsql_transaction tr(db);
            std::cout << "*thr" << idx_ << "* " << "accuired flows: " << flows.size() << std::endl;
            for(flow_data flow: flows)
            {
                if(!check_addr_(flow.ip_src_addr))
                    continue;
                std::string flow_str("flow data:\n");
                flow_str += "\ttimestamp=" + std::to_string(flow.timestamp) +
                        "\n\tip_src_addr=" + std::to_string(flow.ip_src_addr) + "(" + boost::asio::ip::address_v4(flow.ip_src_addr).to_string() + ")\n" +
                        "\tip_dst_addr=" + std::to_string(flow.ip_dst_addr) + "(" + boost::asio::ip::address_v4(flow.ip_dst_addr).to_string() + ")\n" +
                        "\tpostnat_src_addr=" + std::to_string(flow.postnat_src_addr) + "(" + boost::asio::ip::address_v4(flow.postnat_src_addr).to_string() + ")";
                std::cout << flow_str << std::endl;
                tr.append_data(flow);
            }
            tr.execute();
            std::cout << "*thr" << idx_ << "* " << "locking thread" << std::endl;
            data_ready_lock.lock();
        }
        db.disconnect();
        std::cout << "*thr" << idx_ << "* " << "buffer is empty, nice) ***PROCESS = " << (long)&process_ << "/" << std::this_thread::get_id() << std::endl;
        if(process_ == false)
        {
            std::cout << "*thr" << idx_ << "* " << "terminate? ok..." << std::endl;
            break;
        }
        std::cout << "*thr" << idx_ << "* " << "it's time to wait notification, zZzZz...\n" << std::endl;
        data_ready_.get().wait(data_ready_lock);
        std::cout << "*thr" << idx_ << "* " << "time to work!" << std::endl;
    }
}

void handler::terminate()
{
    std::cout << "*thr" << idx_ << "* " << "hello from terminate" << std::endl;
    std::lock_guard<std::mutex> data_ready_lock(buffer_access_.get());
    std::cout << "*thr" << idx_ << "* " << "terminate lock accuired, set process = false" << std::endl;
    process_ = false;
    std::cout << "*thr" << idx_ << "* " << "terminate flag is set, unlocked ***PROCESS = " << (long)&process_ << "/" << std::this_thread::get_id() << std::endl;
}

bool handler::check_addr_(uint32_t addr)
{
    uint32_t netaddr = 3232239360;
    uint32_t netmask = 4294967040;
    if((netaddr & netmask) == (addr & netmask) && addr != netaddr + 1)
        return true;
    return false;
}


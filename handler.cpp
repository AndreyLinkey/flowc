#include "include/handler.h"

handler::handler(std::reference_wrapper<flow_buffer> buffer, std::reference_wrapper<std::mutex> buffer_access,
        std::reference_wrapper<std::condition_variable> data_ready, std::reference_wrapper<connection_info> conn_info,
        size_t data_length, int idx)
    : buffer_(buffer), buffer_access_(buffer_access), data_ready_(data_ready), data_(data_length), process_(true), conn_info_(conn_info), idx_(idx)
{

}

void handler::run()
{
    connection db(conn_info_.get().to_string());
    pgsql_transaction tr;
    filter flt;
    std::unique_lock<std::mutex> data_ready_lock(buffer_access_.get());
    while(true)
    {
        while(buffer_.get().size() != 0)
        {
            buffer_.get().swap_tail(data_);
            parser p(data_);
            data_ready_lock.unlock();
            std::vector<flow_data> flows = p.flows();
            for(flow_data flow: flows)
            {
                if(!check_addr_(flow.ip_src_addr) || !flt.check_flow(flow))
                    continue;
                tr.append_data(flow);
            }
            if(tr.row_count() > 1000)
            {
                tr.execute(db);
            }
            data_ready_lock.lock();
        }

        if(process_ == false)
        {
            break;
        }
        data_ready_.get().wait(data_ready_lock);
    }
    db.disconnect();
}

void handler::terminate()
{
    std::lock_guard<std::mutex> data_ready_lock(buffer_access_.get());
    process_ = false;
}

bool handler::check_addr_(uint32_t addr)
{
    uint32_t netaddr = 1681915904;
    uint32_t netmask = 4294901760;
    if((netaddr & netmask) == (addr & netmask) && addr != netaddr + 1)
        return true;
    return false;
}


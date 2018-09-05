#include "include/handler.h"

handler::handler(flow_buffer& buffer, filter& flt, container& cont, const settings& conf_opts, int idx)
    : buffer_(buffer), flt_(flt), cont_(cont), conf_opts_(conf_opts), data_(buffer.unit_length()),
      process_(true), idx_(idx)
{

}

void handler::run(std::mutex& buffer_access, std::condition_variable& data_ready) try
{
    std::unique_lock<std::mutex> data_ready_lock(buffer_access);

    std::vector<flow_data> flows;
    while(true)
    {
        while(buffer_.size() != 0)
        {
            buffer_.swap_tail(data_);
            parser p(data_);
            data_ready_lock.unlock();
            std::vector<flow_data> parsed = p.flows();
            for(flow_data flow: parsed)
            {
                if(!flt_.check_flow(flow))
                    continue;
                flows.push_back(flow);
            }
            if(flows.size() > RECORDS_COUNT)
            {
                cont_.store_flows(flows);
                flows.clear();
            }
            data_ready_lock.lock();
        }

        if(process_ == false)
        {
           break;
        }
        data_ready.wait(data_ready_lock);
    }
}
catch (const std::exception &e)
{
    log_writer.write_log(e.what());
    throw e;
}

void handler::terminate()
{
    process_ = false;
}

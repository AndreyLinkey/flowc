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
    uint32_t packet = 0, counter = 0, filtered = 0, bs = 0;
    time_t t = time(nullptr);
    while(true)
    {
        bs = buffer_.size();
        while(buffer_.size() != 0)
        {
            packet++;
            buffer_.swap_tail(data_);
            parser p(data_);
            data_ready_lock.unlock();
            std::vector<flow_data> parsed = p.flows();
            for(flow_data flow: parsed)
            {
                counter++;
                if(!flt_.check_flow(flow))
                    continue;
                filtered++;
                flows.push_back(flow);
            }
            if(flows.size() > RECORDS_COUNT)
            {
                cont_.store_flows(flows);
                flows.clear();
            }

            time_t new_t = time(nullptr);
            if(new_t != t && (new_t - t) % 600 == 0 )
            {
                log_writer.write_log("buffer size " + std::to_string(bs) +
                                     " packet count " + std::to_string(packet / 600) +
                                     " average flows count " + std::to_string(counter / 600) +
                                     " average stored count " + std::to_string(filtered / 600));
                t = new_t;
                packet = 0;
                counter = 0;
                filtered = 0;
            }

            data_ready_lock.lock();
        }

        if(process_ == false)
        {
           cont_.store_flows(flows);
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

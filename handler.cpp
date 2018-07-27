#include "include/handler.h"

handler::handler(flow_buffer& buffer, filter& flt, container& cont, const settings& conf_opts, int idx)
    : buffer_(buffer), flt_(flt), cont_(cont), conf_opts_(conf_opts), data_(buffer.unit_length()),
      process_(true), idx_(idx)
{

}

void handler::run(std::mutex& buffer_access, std::condition_variable& data_ready)
{
    //connection db(conf_opts_.conn_info().to_string());
    //pgsql_transaction tr;
    std::unique_lock<std::mutex> data_ready_lock(buffer_access);

    std::cout << "fbuff addr in thread " << idx_ << " is " << &buffer_ << " "
              << "filter addr in thread " << idx_ << " is " << &flt_ << std::endl;

    std::vector<flow_data> flows;
    while(true)
    {
        while(buffer_.size() != 0)
        {
            buffer_.swap_tail(data_);
            parser p(data_);
            data_ready_lock.unlock();
            std::vector<flow_data> parsed = p.flows();
            //std::vector<flow_data> flows = p.flows();
            for(flow_data flow: parsed)
            {
                if(!flt_.check_flow(flow))
                    continue;
                flows.push_back(flow);
                //tr.append_data(table_, flow);
            }
            if(flows.size() > RECORDS_COUNT)
            {
                cont_.store_flows(flows);
                flows.clear();
                //tr.execute(db);
            }
            data_ready_lock.lock();
        }

        if(process_ == false)
        {
            break;
        }
        data_ready.wait(data_ready_lock);
    }
    //db.disconnect();
}

void handler::set_table(const std::string& table_name)
{
    table_ = table_name;
}

void handler::terminate()
{
    //std::lock_guard<std::mutex> data_ready_lock(buffer_access_.get());
    process_ = false;
}

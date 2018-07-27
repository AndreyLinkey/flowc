#include "include/settings.h"

settings::settings()
    : source_port_(SOURCE_PORT), buff_length_(BUFF_LENGTH), queue_length_(QUEUE_LENGTH), threads_count_(THREADS_COUNT)
{

}

settings settings::load_config(std::string config_name)
{
    options_description desc{"Options"};
    desc.add_options()
            ("source", value<int>()->default_value(SOURCE_PORT), "Source port")
            ("buffer", value<int>()->default_value(BUFF_LENGTH), "Buffer length")
            ("queue", value<int>()->default_value(QUEUE_LENGTH), "Queue length")
            ("threads", value<int>()->default_value(THREADS_COUNT), "Threads count")
            ("dbname", value<std::string>(), "Database name")
            ("user", value<std::string>(), "Database user")
            ("password", value<std::string>(), "Database paword")
            ("hostaddr", value<std::string>(), "Database host address")
            ("srcnet", value<std::vector<std::string>>()->multitoken(), "Source networks");
    variables_map vm;

    std::ifstream ifs{config_name.c_str()};
    if (!ifs)
        throw std::runtime_error("no configuration found");

    store(parse_config_file(ifs, desc, true), vm);
    notify(vm);

    settings conf_opts;
    if (vm.count("source"))
        conf_opts.source_port_ = vm["source"].as<int>();
    if (vm.count("buffer"))
        conf_opts.buff_length_ = vm["buffer"].as<int>();
    if (vm.count("queue"))
        conf_opts.queue_length_ = vm["queue"].as<int>();
    if (vm.count("threads"))
        conf_opts.threads_count_ = vm["threads"].as<int>();
    if (vm.count("dbname"))
        conf_opts.conn_info_.dbname = vm["dbname"].as<std::string>();
    if (vm.count("user"))
        conf_opts.conn_info_.user = vm["user"].as<std::string>();
    if (vm.count("password"))
        conf_opts.conn_info_.password = vm["password"].as<std::string>();
    if (vm.count("hostaddr"))
        conf_opts.conn_info_.hostaddr = vm["hostaddr"].as<std::string>();
    if (vm.count("srcnet"))
    {
        std::vector<std::string> src_net = vm["srcnet"].as<std::vector<std::string>>();
        for(std::string& s: src_net)
        {
            network net = network::from_string(s);
            conf_opts.networks_.push_back(net);
        }
    }
    return conf_opts;
}

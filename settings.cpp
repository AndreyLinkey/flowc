#include "include/settings.h"

settings::settings()
    : source_port_(SOURCE_PORT), buff_length_(BUFF_LENGTH), input_queue_length_(INPUT_QUEUE_LENGTH),
      write_queue_length_(WRITE_QUEUE_LENGTH), threads_count_(THREADS_COUNT), flow_timeout_(FLOW_TIMEOUT),
      new_file_inteval_(NEW_FILE_INTERVAL), log_file_name_(LOG_FILE_NAME), output_directory_(OUTPUT_DIRECTORY)
{

}

settings settings::load_config(std::string config_name)
{
    options_description desc{"Options"};
    desc.add_options()
            ("source", value<int>()->default_value(SOURCE_PORT), "Source port")
            ("buffer", value<int>()->default_value(BUFF_LENGTH), "Buffer length")
            ("input_queue", value<int>()->default_value(INPUT_QUEUE_LENGTH), "Input queue length")
            ("write_queue", value<int>()->default_value(WRITE_QUEUE_LENGTH), "Write queue length")
            ("threads", value<int>()->default_value(THREADS_COUNT), "Threads count")
            ("flow_timeout", value<int>()->default_value(FLOW_TIMEOUT), "Flow timeout")
            ("new_file_inteval", value<int>()->default_value(NEW_FILE_INTERVAL), "New file inteval")
            ("log_file_name", value<std::string>()->default_value(LOG_FILE_NAME), "Log file name")
            ("output_directory", value<std::string>()->default_value(OUTPUT_DIRECTORY), "Output directory")
            ("srcnet", value<std::vector<std::string>>()->multitoken(), "Source networks");
    variables_map vm;

    std::ifstream ifs{config_name.c_str()};
    if (!ifs)
        throw std::runtime_error("No configuration found");

    store(parse_config_file(ifs, desc, true), vm);
    notify(vm);

    settings conf_opts;
    if (vm.count("source"))
        conf_opts.source_port_ = vm["source"].as<int>();
    if (vm.count("buffer"))
        conf_opts.buff_length_ = vm["buffer"].as<int>();
    if (vm.count("input_queue"))
        conf_opts.input_queue_length_ = vm["input_queue"].as<int>();
    if (vm.count("write_queue"))
        conf_opts.write_queue_length_ = vm["write_queue"].as<int>();
    if (vm.count("threads"))
        conf_opts.threads_count_ = vm["threads"].as<int>();
    if (vm.count("flow_timeout"))
        conf_opts.flow_timeout_ = vm["flow_timeout"].as<int>();
    if (vm.count("new_file_inteval"))
        conf_opts.new_file_inteval_ = vm["new_file_inteval"].as<int>();
    if (vm.count("log_file_name"))
        conf_opts.log_file_name_ = vm["log_file_name"].as<std::string>();
    if (vm.count("output_directory"))
        conf_opts.output_directory_ = vm["output_directory"].as<std::string>();
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

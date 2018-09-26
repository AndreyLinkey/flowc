#ifndef SETTINGS_H
#define SETTINGS_H

#include <fstream>
#include <iostream>
#include <vector>
#include <boost/program_options.hpp>

#include "defaults.h"
#include "network.h"

using namespace boost::program_options;

class settings
{
public:
    static settings load_config(std::string config_name);

    unsigned short flowcd_port() const {return flowcd_port_;}
    unsigned short flowc_port() const {return flowc_port_;}
    std::size_t buff_length() const {return buff_length_;}
    std::size_t input_queue_length() const {return input_queue_length_;}
    std::size_t write_queue_length() const {return write_queue_length_;}
    unsigned short threads_count() const {return threads_count_;}
    std::time_t flow_timeout() const {return flow_timeout_;}
    std::time_t new_file_inteval() const {return new_file_inteval_;}
    const std::string log_file_name() const {return log_file_name_;}
    const std::string output_directory() const {return output_directory_;}
    const std::vector<network>& networks() const {return networks_;}

private:
    settings();
    unsigned short flowcd_port_;
    unsigned short flowc_port_;
    size_t buff_length_;
    size_t input_queue_length_;
    size_t write_queue_length_;
    unsigned short threads_count_;
    std::time_t flow_timeout_;
    std::time_t new_file_inteval_;
    std::string log_file_name_;
    std::string output_directory_;
    std::vector<network> networks_;
};

#endif // SETTINGS_H

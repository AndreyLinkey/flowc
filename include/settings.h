#ifndef SETTINGS_H
#define SETTINGS_H

#include <boost/program_options.hpp>
#include <fstream>
#include <iostream>
#include <vector>

#include "network.h"
#include "transaction.h"

const std::size_t SOURCE_PORT = 2055;
const std::size_t BUFF_LENGTH = 1472;
const std::size_t QUEUE_LENGTH = 256;
const std::size_t THREADS_COUNT = 1;

using namespace boost::program_options;

class settings
{
public:
    //settings(settings&) = delete;
    static settings load_config(std::string config_name);

    unsigned int source_port() const {return source_port_;}
    std::size_t buff_length() const {return buff_length_;}
    std::size_t queue_length() const {return queue_length_;}
    unsigned int threads_count() const {return threads_count_;}
    const connection_info& conn_info() const {return conn_info_;}
    const std::vector<network>& networks() const {return networks_;}

private:
    settings();
    unsigned int source_port_;
    unsigned int buff_length_;
    unsigned int queue_length_;
    unsigned int threads_count_;
    connection_info conn_info_;
    std::vector<network> networks_;

};

#endif // SETTINGS_H

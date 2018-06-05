
#include <array>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <iostream>
#include <iomanip>
#include <algorithm>

using boost::asio::ip::udp;

#include <chrono>
#include <csignal>
#include <cstdlib>
#include <fstream>
#include <queue>
#include <string>
#include <thread>
#include <utility>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include "include/swappable_circular_buffer.h"
#include "include/parser.h"
#include "include/templates.h"
#include "include/transaction.h"
#include "include/handler.h"

const std::string CONFIG_NAME = "config";
const std::size_t MAX_LENGTH = 1472;

bool terminate = false;

void signal_handler(int signal)
{
    //std::cout << "*main* signal received" << std::endl;
    terminate = true;
}

using namespace boost::program_options;

int main()
{
    std::signal(SIGINT, signal_handler);

    int source_port, queue_length, threads_count;
    connection_info ci;
    try
    {
        options_description desc{"Options"};
        desc.add_options()
            ("help,h", "Help screen")
            ("source", value<int>()->default_value(2055), "Source port")
            ("queue", value<int>()->default_value(10), "Queue length")
            ("threads", value<int>()->default_value(1), "Threads count")
            ("dbname", value<std::string>(), "Database name")
            ("user", value<std::string>(), "Database user")
            ("password", value<std::string>(), "Database paword")
            ("hostaddr", value<std::string>(), "Database host address");
        variables_map vm;

        std::ifstream ifs{CONFIG_NAME.c_str()};
        if (!ifs)
            throw std::runtime_error("no configuration found");

        store(parse_config_file(ifs, desc, true), vm);
        notify(vm);

        if (vm.count("source"))
            source_port = vm["source"].as<int>();
        if (vm.count("queue"))
            queue_length = vm["queue"].as<int>();
        if (vm.count("threads"))
            threads_count = vm["threads"].as<int>();
        if (vm.count("dbname"))
            ci.dbname = vm["dbname"].as<std::string>();
        if (vm.count("user"))
            ci.user = vm["user"].as<std::string>();
        if (vm.count("password"))
            ci.password = vm["password"].as<std::string>();
        if (vm.count("hostaddr"))
            ci.hostaddr = vm["hostaddr"].as<std::string>();
    }
    catch (const error &ex)
    {
        std::cerr << ex.what() << '\n';
    }

    std::mutex buffer_access;
    std::condition_variable data_ready;

    boost::asio::io_service io_service;
    udp::endpoint flow_source = udp::endpoint(boost::asio::ip::address_v4::any(), source_port);
    boost::asio::ip::udp::socket socket = boost::asio::ip::udp::socket(io_service, flow_source);
    io_service.run();

    raw_data data(MAX_LENGTH);
    flow_buffer fbuff(queue_length, MAX_LENGTH);

    std::vector<handler> handlers;
    handlers.reserve(threads_count);
    std::vector<std::thread> threads;
    threads.reserve(threads_count);

    for (int i = 0; i < threads_count; ++i)
    {
        handlers.emplace_back(std::ref(fbuff), std::ref(buffer_access), std::ref(data_ready), std::ref(ci), MAX_LENGTH, i + 1);
        threads.push_back(std::thread(std::bind(&handler::run, &handlers.back())));
    }

    std::unique_lock<std::mutex> data_ready_lock(buffer_access);
    data_ready_lock.unlock();
    while(!terminate)
    {
        socket.receive(boost::asio::buffer(data, MAX_LENGTH));
        data_ready_lock.lock();
        fbuff.swap_head(data);
        data_ready_lock.unlock();
        data_ready.notify_one();
    }

    for (handler& h: handlers)
    {
        h.terminate();
    }

    data_ready_lock.lock();
    data_ready.notify_all();
    data_ready_lock.unlock();

    for (std::thread& t: threads)
    {
        t.join();
    }

    return 0;
}

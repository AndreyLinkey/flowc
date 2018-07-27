
//#include <array>
#include <vector>
#include <cstdlib>
#include <iostream>
//#include <boost/asio.hpp>
#include <iostream>
#include <iomanip>
//#include <algorithm>

#include <chrono>
#include <csignal>
#include <cstdlib>
#include <fstream>
//#include <queue>
#include <string>
#include <thread>
#include <utility>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include "include/swappable_circular_buffer.h"
#include "include/parser.h"
#include "include/settings.h"
#include "include/templates.h"
//#include "include/transaction.h"
#include "include/handler.h"
#include "include/container.h"

using boost::asio::io_service;
using boost::asio::ip::udp;

const uint32_t DUPLICATE_TIMEOUT = 180;
const std::string CONFIG_NAME = "config";

bool terminate = false;

//void create_table(const settings& s, const std::string& name)
//{
//    connection db(s.conn_info().to_string());
//    pgsql_transaction tr;
//    tr.create_table(db, name);
//}

void sigint_handler(int signal)
{
    terminate = true;
}

int main()
{
    try
    {
        settings cfg = settings::load_config(CONFIG_NAME);

        io_service srv;
        udp::endpoint flow_source = udp::endpoint(boost::asio::ip::address_v4::any(), cfg.source_port());
        udp::socket socket = udp::socket(srv, flow_source);
        srv.run();

        std::signal(SIGINT, sigint_handler);
        std::mutex buffer_access;
        std::condition_variable data_ready;

        raw_data data(cfg.buff_length());
        flow_buffer fbuff(cfg.queue_length(), cfg.buff_length());
        filter flt(cfg.networks(), DUPLICATE_TIMEOUT);
        container cont;

        std::cout << "fbuff addr is " << &fbuff << " "
                  << "filter addr is " << &flt << std::endl;

        std::vector<handler> handlers;
        handlers.reserve(cfg.threads_count());
        std::vector<std::thread> threads;
        threads.reserve(cfg.threads_count());

        for (int i = 0; i < cfg.threads_count(); ++i)
        {
            //handlers.emplace_back(std::ref(fbuff), std::cref(cfg), i + 1);
            handlers.emplace_back(fbuff, flt, cont, cfg, i + 1);
            threads.push_back(std::thread(std::bind(&handler::run, &handlers.back(), std::ref(buffer_access), std::ref(data_ready))));
        }

        std::unique_lock<std::mutex> data_ready_lock(buffer_access);
        data_ready_lock.unlock();

        uint32_t t = time(0);
//        create_table(cfg, std::to_string(t));
//        for(handler& hdl: handlers)
//            hdl.set_table(std::to_string(t));
        std::string file_name("/home/files/scripts/");
        file_name += std::to_string(t);
        cont.open_file(file_name, 'w');

        while(!terminate)
        {
            socket.receive(boost::asio::buffer(data, cfg.buff_length()));
            data_ready_lock.lock();


            uint32_t new_t = time(0);
            if((new_t - t) % 300 == 0 & new_t != t)
            {
                std::cout << "bufer size " << fbuff.size() << std::endl;
                file_name = "/home/files/scripts/" + std::to_string(new_t);
                cont.open_file(file_name, 'w');
                t = new_t;
            }


//            uint32_t new_t = time(0);
//            if((new_t - t) % 3600 == 0 & new_t != t)
//            {
//                create_table(cfg, std::to_string(new_t));
//                for(handler& hdl: handlers)
//                    hdl.set_table(std::to_string(new_t));
//                t = new_t;
//            }

            fbuff.swap_head(data);
            data_ready_lock.unlock();
            data_ready.notify_one();
        }

        data_ready_lock.lock();
        for (handler& h: handlers)
        {
            h.terminate();
        }
        data_ready.notify_all();
        data_ready_lock.unlock();

        for (std::thread& t: threads)
        {
            t.join();
        }

        return 0;
    }
    catch (const error &ex)
    {
        std::cerr << ex.what() << '\n';
        return 1;
    }
}

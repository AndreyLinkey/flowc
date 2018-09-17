
#include <csignal>
#include <ctime>
#include <exception>
#include <filesystem>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <boost/asio.hpp>

#include "include/container.h"
#include "include/defaults.h"
#include "include/handler.h"
#include "include/logger.h"
#include "include/parser.h"
#include "include/settings.h"
#include "include/swappable_circular_buffer.h"
#include "include/templates.h"

using boost::asio::io_service;
using boost::asio::ip::udp;

static bool terminate = false;
static udp::socket* socket_ptr = nullptr;

void sig_handler(int signal)
{
    terminate = true;
    if(socket_ptr != nullptr)
    {
        socket_ptr -> shutdown(boost::asio::ip::udp::socket::shutdown_receive);
    }
}

int main() try
{
    settings cfg = settings::load_config(CONFIG_NAME);
    log_writer.set_log_name(cfg.log_file_name());

    io_service srv;
    udp::endpoint flow_source(boost::asio::ip::address_v4::any(), cfg.source_port());
    udp::socket socket(srv, flow_source);
    socket_ptr = &socket;
    srv.run();

    std::signal(SIGINT, sig_handler);
    std::signal(SIGTERM, sig_handler);

    std::mutex buffer_access;
    std::condition_variable data_ready;

    raw_data data(cfg.buff_length());
    flow_buffer fbuff(cfg.input_queue_length(), cfg.buff_length());
    filter flt(cfg.networks(), cfg.flow_timeout());
    container cont(cfg.write_queue_length());
    std::thread cont_thr = std::thread(std::bind(&container::run, &cont));

    std::vector<handler> handlers;
    handlers.reserve(cfg.threads_count());
    std::vector<std::thread> threads;
    threads.reserve(cfg.threads_count());

    for(unsigned int i = 0; i < cfg.threads_count(); ++i)
    {
        handlers.emplace_back(fbuff, flt, cont, cfg, i + 1);
        threads.emplace_back(std::bind(&handler::run, &handlers.back(), std::ref(buffer_access), std::ref(data_ready)));
    }

    try
    {
        time_t t = time(nullptr);
        std::filesystem::path file_name(cfg.output_directory());
        file_name /= std::to_string(t) + OUTPUT_EXTENSION;
        cont.create_file(file_name.string());
        log_writer.write_log("flow collector started");

        while(!terminate)
        {
            if(!socket.receive(boost::asio::buffer(data, cfg.buff_length())))
                continue;
            std::lock_guard<std::mutex> lock(buffer_access);
            time_t new_t = time(nullptr);

            if((new_t > t + cfg.new_file_inteval()) |
                    ((new_t - t) % cfg.new_file_inteval() == 0 & new_t != t))
            {
                file_name = std::filesystem::path(cfg.output_directory()) / (std::to_string(new_t) + OUTPUT_EXTENSION);
                cont.create_file(file_name.string());
                t = new_t;
            }
            fbuff.swap_head(data);
            data_ready.notify_one();
        }
    }
    catch (const std::exception &e)
    {
        log_writer.write_log(e.what());
    }

    std::unique_lock<std::mutex> data_ready_lock(buffer_access);
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

    cont.terminate();
    cont_thr.join();
    log_writer.write_log("flow collector terminated");

    return (terminate) ? 0 : 1;
}
catch (const std::exception &e)
{
    log_writer.write_log(e.what());
    return 1;
}



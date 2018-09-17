//
// async_udp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <array>
#include <filesystem>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <iostream>
#include <iomanip>
#include <algorithm>

using boost::asio::ip::udp;

#include <condition_variable>
#include <thread>
#include <chrono>
#include <time.h>
#include <queue>
#include <string>
#include <utility>
#include <cstdlib>
#include <tuple>

//http://www.boost.org/doc/libs/1_66_0/doc/html/boost/circular_buffer.html

#include <boost/circular_buffer.hpp>
#include <unistd.h>

#include "include/swappable_circular_buffer.h"
#include "defaults.h"
#include "include/parser.h"
#include "include/templates.h"
#include "include/container.h"

#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>
#include <string>

//using namespace boost::program_options;

//void on_age(int age)
//{
//  std::cout << "On age: " << age << '\n';
//}

//void to_cout(const std::vector<std::string> &v)
//{
//  std::copy(v.begin(), v.end(), std::ostream_iterator<std::string>{
//    std::cout, "\n"});
//}

//void options(int argc, const char *argv[])
//{
//    std::string f_name = "config";
//    try
//    {
//        options_description desc{"Options"};
//        desc.add_options()
//            ("help,h", "Help screen")
//            ("pi,p", value<float>()->default_value(3.14f), "Pi")
//            //("phone", value<int>()->notifier(on_age), "Phone")
//            ("age", value<int>()->notifier(on_age), "Age")
//            ("phone", value<std::vector<std::string>>()->multitoken()->composing(), "Phone");

//        variables_map vm2;


//        std::ifstream ifs{f_name.c_str()};
//        if (ifs)
//           store(parse_config_file(ifs, desc, true), vm2);

//        variables_map vm;
//        store(parse_command_line(argc, argv, desc), vm);
//        //vm.merge(vm2);
//        //if(vm2.count("pi"))
//        //    vm["pi"] = vm2["pi"];
//        notify(vm2);

//      if (vm2.count("help"))
//        std::cout << desc << '\n';
//      else
//      {
//        if (vm2.count("age"))
//            std::cout << "Age: " << vm2["age"].as<int>() << '\n';
//        if (vm2.count("pi"))
//            std::cout << "Pi: " << vm2["pi"].as<float>() << '\n';
//        if (vm2.count("phone"))
//        {
//            std::cout << "phone" << '\n';
//            to_cout(vm2["phone"].as<std::vector<std::string>>());
//        }
//      }
//    }
//    catch (const error &ex)
//    {
//      std::cerr << ex.what() << '\n';
//    }
//}

#include <algorithm>
#include <iterator>
#include <regex>
//#include <boost/range/algorithm.hpp>
//#include <boost/range/adaptors.hpp>
//using boost::adaptors::filtered;
//using boost::adaptors::transformed;
//using boost::adaptors::uniqued;
using std::copy_if;

//    if(find_if(networks_, [&flow](const network& net){return net.net_addr() == (flow.ip_src_addr & net.net_mask());}) == networks_.end())
//    {
//        return false;
//    }

//    std::vector<uint32_t> erased;
//    copy(
//        flow_cache_.left
//            | filtered([this, min_timestamp](const bimap_cache::left_value_type lval){return lval.second < min_timestamp;})
//            | transformed([](const bimap_cache::left_value_type lval){return lval.second;})
//            | uniqued,
//        std::back_inserter(erased));

//    for_each(erased, [this](int item){flow_cache_.right.erase(item);});

static std::string timestr_from_timestamp(time_t timestamp)
{
    std::tm t = *std::localtime(&timestamp);
    std::ostringstream ss;
    ss << std::put_time(&t, "%Y%m%dT%H%M%S");
    return ss.str();
}

static time_t timestr_to_timestamp(std::string timestr)
{
    std::tm t = {};
    std::istringstream ss(timestr);
    ss >> std::get_time(&t, "%Y%m%dT%H%M%S");
    return mktime(&t);
}


int main(int argc, const char *argv[])
{
    std::string file_name("/home/files/scripts/flowc/build-flowc-clang-Debug");
    //file_name += std::to_string(1532619915);

    std::vector<std::filesystem::directory_entry> files;
    std::filesystem::directory_iterator dir(file_name);

    std::time_t file_int = 4;
    std::string start_from = timestr_from_timestamp(1535105228 - file_int);
    std::string end_at = timestr_from_timestamp(1535105234); //time(nullptr);

    std::cout << std::to_string(timestr_to_timestamp(start_from)) << " "
              << std::to_string(timestr_to_timestamp(end_at)) << std::endl;

    copy_if(std::filesystem::begin(dir), std::filesystem::end(dir), std::back_inserter(files),
        [start_from, end_at](const std::filesystem::directory_entry& entry)
        {
            return entry.status().type() == std::filesystem::file_type::regular &&
                   entry.path().extension().string() == OUTPUT_EXTENSION &&
                   std::regex_search(entry.path().stem().string(), std::regex("^\\d{8}T\\d{6}$")) &&
                   (entry.path().stem().string() > start_from &&
                    entry.path().stem().string() <= end_at);
        });

//            | filtered(
//            | filtered([](const std::filesystem::directory_entry& entry){
//                return std::regex_search(entry.path().filename().string(), std::regex("^\\d{10}\\d*\\.dmp$"));})


    for(const std::filesystem::directory_entry& p: files)
    {
        std::cout << std::to_string(std::stoi(p.path().stem())) << " " << p.path().extension() << '\n';
    }

//    for(const std::filesystem::directory_entry& p: std::filesystem::directory_iterator(file_name))
//    {
//        if(p.status().type() != std::filesystem::file_type::regular)
//            continue;
//        if(std::regex_search(p.path().filename().string(), std::regex("^\\d{10}\\d*\\.dmp$")))
//        {
//            std::cout << p.path().filename() << " " << p.path().extension() << '\n';
//        }

//    }

    return 0;


    container cont;
    cont.open_file(file_name.c_str());

    std::vector<flow_data> flows;
    using flow_item = std::tuple<uint32_t, uint32_t, uint32_t>;
    std::map<flow_item, uint32_t> buffer;

    while(true)
    {
        flows = cont.read_flows(256);
        if(flows.size() == 0)
        {
            break;
        }

        for(flow_data flow: flows)
        {
            flow_item fi = std::make_tuple(flow.ip_src_addr, flow.ip_dst_addr, flow.postnat_src_addr);

            if(buffer.find(fi) != buffer.end())
            {
                ++buffer.at(fi);
            }
            else
            {
                buffer[fi] = 1;
            }
        }
    }

    size_t duplicate = 0;
    for(std::pair<flow_item, uint32_t> flow: buffer)
    {
        if(flow.second > 1)
        {
            std::cout << "src addr " << std::get<0>(flow.first) << " "
                      << "dst addr " << std::get<1>(flow.first) << " "
                         "post nat " << std::get<2>(flow.first) << std::endl;
            ++duplicate;
        }
    }
    std::cout << "total count " << buffer.size() << " "
              << "duplicate count " << duplicate << std::endl;

}


/*
void data_preparation_thread()
{
    udp::socket& socket_;
    boost::asio::io_service&  io_service_;
    udp::endpoint sender_endpoint_;

    while(true) //more_data_to_prepare())
    {
        data_chunk const data=prepare_data();
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(data);
        data_cond.notify_one();
    }
}

void data_processing_thread()
{
    while(true)
    {
    std::unique_lock<std::mutex> lk(mut);
    data_cond.wait(
        lk,[]{return !data_queue.empty();});
    data_chunk data=data_queue.front();
    data_queue.pop();
    lk.unlock();
    process(data);
    if(is_last_chunk(data))
        break;
    }
}
*/


/*
class handler
{
public:
  handler(boost::asio::io_service& io_serv, udp::socket& flow_socket)
    : io_service_(io_serv), socket_(flow_socket)
  {
    //do_receive();
     // io_service_.run();
  }

  void do_receive()
  {
    std::cout << "thread" << std::endl;
    socket_.async_receive_from(
        boost::asio::buffer(data_, max_length), sender_endpoint_,
        [this](boost::system::error_code ec, std::size_t bytes_recvd)
        {
        std::cout << "lambda" << std::endl;

          if (!ec && bytes_recvd > 0)
          {
              //for(unsigned char& byte : data_)
              for(int i = 0; i < bytes_recvd; ++i)
                  std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)data_[i]; //(int)byte;
              std::cout << std::endl;
          }
          do_receive();
        });
  }

private:
  udp::socket& socket_;
  boost::asio::io_service&  io_service_;
  udp::endpoint sender_endpoint_;
  enum { max_length = 1024 };
  std::array<unsigned char, max_length> data_;
};

void thread_function(handler &hdl)
{
     hdl.do_receive();
}

int main(int argc, char* argv[])
{
    int port = 2055;
    boost::asio::io_service io_service;
    udp::socket flow_socket(io_service, udp::endpoint(boost::asio::ip::address_v4::any(), port));


    handler h1(io_service, flow_socket);
    handler h2(io_service, flow_socket);
    std::thread t1(std::bind(&handler::do_receive, h1));
    std::thread t2(std::bind(&handler::do_receive, h2));

    //std::thread t1(thread_function, std::ref(h1));
    //std::thread t2(thread_function, std::ref(h2));

    io_service.run();


    t1.join();
    t2.join();

//std::thread t(thread_function, std::ref(h));
//std::thread t(std::bind(&handler::process, &h));



    return 0;
}*/

/*
const unsigned int IPFIX_HDR_LENGTH = 16;


std::mutex mut;
//std::queue<data_chunk> data_queue;
std::condition_variable data_cond;


void print_buffer(boost::asio::streambuf buff) {
    for(auto& byte : buff.data())
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    std::cout << std::endl;
}

void print_buffer(std::vector<unsigned char> buff) {
    for(unsigned char& byte : buff)
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    std::cout << std::endl;
}
*/

/*
int main()
{
    //std::array<unsigned char, IPFIX_HDR_LENGTH> hdr_buff;

    boost::asio::io_service io_service;
    int port = 2055;
    udp::endpoint flow_source = udp::endpoint(boost::asio::ip::address_v4::any(), port);
    boost::asio::ip::udp::socket socket = boost::asio::ip::udp::socket(io_service, flow_source);
    io_service.run();


    boost::asio::streambuf hdr_buff;
    hdr_buff.prepare(IPFIX_HDR_LENGTH);

    // reserve 512 bytes in output sequence
    //boost::asio::streambuf::mutable_buffers_type hdr_buff = b.prepare(IPFIX_HDR_LENGTH);

    // received data is "committed" from output sequence to input sequence
    //b.commit(n);



    //socket.receive_from(boost::asio::buffer(hdr_buff, IPFIX_HDR_LENGTH), flow_source);
    socket.receive(hdr_buff);
    print_buffer(hdr_buff);

    int version = (hdr_buff[0] << 8) | hdr_buff[1];
    int length = (hdr_buff[2] << 8) | hdr_buff[3];
    std::cout << std::dec << version << " " << length << std::endl;

    std::cout << socket.available() << std::endl;


    std::vector<unsigned char> buff;
    length -= IPFIX_HDR_LENGTH;
    buff.resize(length);
    //socket.receive_from(boost::asio::buffer(buff.data(), length), flow_source);
    socket.receive(boost::asio::buffer(buff.data(), length));

    //buff.resize(length);
    print_buffer(buff);


//    try
//    {
//        int i = 0;
//        while(i < 10000000)
//        {
//            socket.async_receive_from(boost::asio::buffer(handler.data(), MAX_LENGTH), flow_source, handler);
//            i++;
//        }
//    }
//    catch (std::exception& e)
//    {
//      std::cerr << "Exception: " << e.what() << "\n";
//    }


//    while(True)
//    {
//        data_chunk const data=prepare_data();
//        std::lock_guard<std::mutex> lk(mut);
//        data_queue.push(data);
//        data_cond.notify_one();
//    }
}*/

//void data_processing_thread()
//{
//    while(true)
//    {
//        std::unique_lock<std::mutex> lk(mut);
//        data_cond.wait(lk, []{return !data_queue.empty();});
//        data_chunk data=data_queue.front();
//        data_queue.pop();
//        lk.unlock();
//        process(data);
//        if(is_last_chunk(data))
//            break;
//    }
//}


/*

class handler
{
public:
    handler(std::condition_variable& task_done, int interval)
        : task_done_(task_done), interval_(interval)
    {}

    void process()
    {
        while(true)
        {
            std::unique_lock<std::mutex> lk(handler_mutex_);
            std::cout << interval_ << " Waiting... \n" << std::endl;
            data_ready_.wait(lk, [this](){return notified_;});
            is_busy_ = true;
            std::cout << interval_ << " Signal accuired, performing some actions... \n" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(interval_ * 1000));
            std::cout << interval_ << " Notified! \n" << std::endl;
            is_busy_ = false;
            task_done_.notify_all();
        }
    }

    void notify()
    {
        notified_ = true;
        data_ready_.notify_one();
        notified_ = false;
    }

    bool is_busy() const
    {
        return is_busy_;
    }

private:
    int interval_;
    bool notified_ = false;
    bool is_busy_ = false;
    std::condition_variable data_ready_;
    std::condition_variable& task_done_;
    std::mutex handler_mutex_;
};



void thread_function(handler &hdl)
{
     hdl.process();
}

int main(int argc, char* argv[])
{
    std::mutex mtx;
    std::unique_lock<std::mutex> lk(mtx);
    std::condition_variable task_done;

    handler h1(task_done, 1);
    handler h2(task_done, 2);
    handler h3(task_done, 3);
    handler h4(task_done, 4);
    std::vector<handler> hdls;
    hdls.push_back(h1);
    hdls.push_back(h2);
    hdls.push_back(h3);
    hdls.push_back(h4);

    //const std::array<handler&, 4> hdls{handler(task_done, 1), handler(task_done, 2), handler(task_done, 3), handler(task_done, 4)};
    std::vector<std::thread> trds;
    for(const handler& hdl : hdls)
    {
        std::thread t(std::bind(&handler::process, hdl));
        std::cout << "Thread started \n" << std::endl;
        trds.push_back(t);
    }

    //std::thread t(thread_function, std::ref(h));
    //std::thread t(std::bind(&handler::process, &h));

    while(true)
    {
        if(!std::any_of(hdls.begin(), hdls.end(), [](const handler& hdl){return hdl.is_busy();}))
        {
            std::cout << "Handler is busy, whaiting... \n" << std::endl;
            task_done.wait(lk);
        }
        for(handler& hdl : hdls)
            if(!hdl.is_busy())
            {
                std::cout << "Sending notification \n" << std::endl;
                hdl.notify();
                std::cout << "Notification sent \n" << std::endl;
                break;
            }
    }

    for(std::thread& trd : trds)
        trd.join();

}
*/
/*
class IpfixHandler
{
public:

    int operator()(boost::system::error_code ec, std::size_t bytes_recvd) const
    {
        std::cout << "received  " << std::endl;
        if (!ec && bytes_recvd > 0)
        {
          std::cout << bytes_recvd << ": " << std::endl;

          for(int i = 0 ; i < bytes_recvd; ++i ){
                if(i > 0 && i % 16 == 0)
                    std::cout << std::endl;
                else if(i > 0 && i % 8 == 0)
                    std::cout << "  ";
                std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)data_[i] << ' ';
          }
          std::cout << std::endl;
         }
    }
    std::array<unsigned char, MAX_LENGTH>& data()
    {
        return data_;
    }
private:
    std::array<unsigned char, MAX_LENGTH> data_;
    boost::asio::ip::udp::endpoint sender_endpoint_;
};



int main(int argc, char* argv[])
{
    boost::asio::io_service io_service;
    int port = 2055;
    udp::endpoint flow_source = udp::endpoint(boost::asio::ip::address_v4::any(), port);
    boost::asio::ip::udp::socket socket = boost::asio::ip::udp::socket(io_service, flow_source);
    IpfixHandler handler = IpfixHandler();

    io_service.run();

    try
    {
        int i = 0;
        while(i < 10000000)
        {
            socket.async_receive_from(boost::asio::buffer(handler.data(), MAX_LENGTH), flow_source, handler);
            i++;
        }
    }
    catch (std::exception& e)
    {
      std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
} */

//  try
//  {
////    if (argc != 2)
////    {
////      std::cerr << "Usage: async_udp_echo_server <port>\n";
////      return 1;
////    }

//    boost::asio::io_service io_service;
//    int port = 2055;
//    server s(io_service, port);

//    io_service.run();
//  }
//  catch (std::exception& e)
//  {
//    std::cerr << "Exception: " << e.what() << "\n";
//  }

//  return 0;


//class server
//{
//  public:
//    server(boost::asio::io_service& io_service, short port)
//      : socket_(io_service, udp::endpoint(boost::asio::ip::address_v4::any(), port))
//    {
//      //socket_.open(udp::v4());
//      do_receive();
//    }

//    void do_receive()
//    {
//      std::cout << "do_receive" << std::endl;
//      socket_.async_receive_from(
//      boost::asio::buffer(data_, max_length), sender_endpoint_,
//        [this](boost::system::error_code ec, std::size_t bytes_recvd)
//        {
//          std::cout << "received" << std::endl;
//          if (!ec && bytes_recvd > 0)
//          {
//            std::cout << bytes_recvd << ": " << std::endl;

//            for(int i = 0 ; i < bytes_recvd; ++i ){
//                  if(i > 0 && i % 16 == 0)
//                      std::cout << std::endl;
//                  else if(i > 0 && i % 8 == 0)
//                      std::cout << "  ";
//                  std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)data_[i] << ' ';
//            }
//            std::cout << std::endl;
//            do_receive();
//          }
//          else
//          {
//            do_receive();
//          }
//        });
//    }

//  private:
//    boost::asio::ip::udp::socket socket_;
//    boost::asio::ip::udp::endpoint sender_endpoint_;
//    enum { max_length = 1024 };
//    unsigned char data_[max_length];
//};

/*
using namespace boost::asio;
io_service service;
ip::udp::socket sock(service);
boost::asio::ip::udp::endpoint sender_ep;
char buff[512];
void on_read(const boost::system::error_code & err, std::size_t read_bytes)
 {
    std::cout << "do_receive" << std::endl;
    std::cout << "read " << read_bytes << std::endl;
    sock.async_receive_from(buffer(buff), sender_ep, on_read);
}

int main(int argc, char* argv[])
{
    ip::udp::endpoint ep(boost::asio::ip::address_v4::any(), 2055 );
    sock.open(ep.protocol());
    sock.set_option(boost::asio::ip::udp::socket::reuse_address(true));
    //sock.set_option(boost::asio::ip::udp::socket::implementation_type);
    sock.bind(ep);
    sock.async_receive_from(buffer(buff, 512), sender_ep, on_read);
    service.run();
}*/

/*
#include<sys/socket.h>
#define BUFLEN 512  //Max length of buffer
#define PORT 2055

void die(char *s)
{
    perror(s);
    exit(1);
}

int main(int argc, char* argv[])
{
    struct sockaddr_in si_me, si_other;

    int s, i, recv_len;
    socklen_t slen = sizeof(si_other);
    char buf[BUFLEN];

    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }

    //keep listening for data
    while(1)
    {
        printf("Waiting for data...");
        fflush(stdout);

        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
        {
            die("recvfrom()");
        }

        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("Data: %s\n" , buf);
    }

    close(s);
    return 0;


}
*/

//#include <iostream>

//using namespace std;

//int main()
//{
//    cout << "Hello World!" << endl;
//    return 0;
//}

/*
std::vector<int> data({16, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20});
int data_index = 0;


void read_data(void* buf_ptr, int length)
{

    for(int i = data_index; i < data_index + length; ++data_index)
    {
        *(int*)buf_ptr = data[i];
        buf_ptr++;
    }
}

int handler(boost::system::error_code ec, std::size_t bytes_recvd)
{

}

int main(int argc, char* argv[])
{
    expandable_buffer<int, MAX_LENGTH> ex_buf;
    read_data(ex_buf.begin(), MAX_LENGTH);
    ex_buf.print();

    boost::asio::io_service io_service;
    int port = 2055;
    udp::endpoint flow_source = udp::endpoint(boost::asio::ip::address_v4::any(), port);
    boost::asio::ip::udp::socket socket = boost::asio::ip::udp::socket(io_service, flow_source);
    io_service.run();

    socket.async_receive_from(boost::asio::buffer((void*)ex_buf.begin(), MAX_LENGTH), flow_source, handler);
}
*/

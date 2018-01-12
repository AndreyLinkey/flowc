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
#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <iostream>
#include <iomanip>

using boost::asio::ip::udp;

const unsigned int MAX_LENGTH = 1024;


#include <condition_variable>
#include <thread>


class handler
{
public:
    handler(std::condition_variable& whait_buffer)
        : whait_buffer_(whait_buffer)
    {}

    void process()
    {
        std::unique_lock<std::mutex> lk(handler_mutex_);
        std::cout << "Waiting... \n" << std::endl;
        whait_buffer_.wait(lk, [this](){return notified_;});
        std::cout << "Notified! \n" << std::endl;


    }

    void set_notify()
    {
        notified_ = true;
    }

private:
    bool notified_ = false;
    std::condition_variable& whait_buffer_;
    std::mutex handler_mutex_;
};



void thread_function(handler &hdl)
{
     hdl.process();
}

int main(int argc, char* argv[])
{
    std::condition_variable notify_haldler;

    handler h(notify_haldler);

    std::thread t(thread_function, std::ref(h));
    std::thread t(std::bind(&handler::process, &h));

    std::cout << "Thread started \n" << std::endl;
    for(int i = 0; i < 5; ++i)
    {
        std::cout << "Sending notification \n" << std::endl;
        h.set_notify();
        notify_haldler.notify_one();
        std::cout << "Notification sent \n" << std::endl;
    }


    t.join();

}

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

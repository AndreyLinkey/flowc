#include "include/logger.h"

logger::logger(std::string log_name)
    : log_name_(log_name)
{

}

void logger::write_log(std::string message)
{
    time_t t = time(nullptr);
    struct tm* timeinfo = localtime(&t);;
    char t_buff[20];
    strftime(t_buff, sizeof(t_buff), "%Y-%m-%d %H:%M:%S", timeinfo);
    std::string formatted_message(t_buff);
    formatted_message += " " + message + '\n';

    std::fstream file;
    try
    {
        file.open(log_name_, std::ios::out | std::ios::app);
        if(!file.is_open())
        {
            std::string err("Unable to open file ");
            err += log_name_;
            throw std::invalid_argument(err);
        }
        std::cout << formatted_message;
        file.write(formatted_message.c_str(), formatted_message.size());
        file.close();
    }

    catch (std::exception& e)
    {
        std::cout << "Unable to write log: " << e.what() << std::endl;
    }
}

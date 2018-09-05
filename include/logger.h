#ifndef LOGGER_H
#define LOGGER_H

#include <ctime>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>

#include "defaults.h"

class logger
{
public:
    logger(std::string log_name = LOG_FILE_NAME);
    void set_log_name(std::string log_name) {log_name_ = log_name;}
    void write_log(std::string message);

private:
    std::string log_name_;

};

static logger log_writer;

#endif // LOGGER_H

#ifndef DEFAULTS_H
#define DEFAULTS_H

#include <cstddef>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <string>

const std::string CONFIG_NAME = "/etc/flowc.cfg";

const std::size_t SOURCE_PORT = 2055;
const std::size_t BUFF_LENGTH = 1472;
const std::size_t INPUT_QUEUE_LENGTH = 256;
const std::size_t WRITE_QUEUE_LENGTH = 256;
const std::size_t THREADS_COUNT = 1;
const std::time_t FLOW_TIMEOUT = 180;
const std::time_t NEW_FILE_INTERVAL = 3600;
const std::string OUTPUT_DIRECTORY("./");
const std::string LOG_FILE_NAME("/var/log/flowc.log");
const std::string OUTPUT_EXTENSION(".flwc");

const std::uint32_t RECORDS_COUNT = 512;
const std::uint16_t TEMPLATE_SET_ID = 2;
const std::size_t TEMPLATE_STORAGE_SIZE = 10;

const std::string FILE_NAME_DATETIME_FORMAT = "%Y%m%dT%H%M%S";
const std::uint32_t DUMP_SIGNATURE = 1668770918;

#endif // DEFAULTS_H

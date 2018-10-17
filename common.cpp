#include "include/common.h"


uint16_t pack_be_to_uint16(raw_data::const_iterator data_it)
{
    return static_cast<uint16_t>(*data_it << 8 | *(data_it + 1));
}

uint32_t pack_be_to_uint32(raw_data::const_iterator data_it)
{
    return static_cast<uint32_t>(*data_it << 24 | *(data_it + 1) << 16 | *(data_it + 2) << 8 | *(data_it + 3));
}

uint32_t pack_le_to_uint32(raw_data::const_iterator data_it)
{
    return static_cast<uint32_t>(*(data_it + 3) << 24 | *(data_it + 2) << 16 | *(data_it + 1) << 8 | *data_it);
}

uint32_t ipstr_to_ipnum(const std::string& ipstr)
{
    std::cmatch cm;
    std::regex_match(ipstr.c_str(), cm, IPSTR_REGEXP);

    if(cm.size() != 5)
        throw std::invalid_argument("invalid ip address");

    return static_cast<uint32_t>(std::stoi(cm[1]) << 24 | std::stoi(cm[2]) << 16 |
                                              std::stoi(cm[3]) << 8 | std::stoi(cm[4]));
}

std::string ipnum_to_ipstr(const uint32_t ipnum)
{
    uint32_t mask = 255;
    std::ostringstream ss;
    ss << std::to_string((ipnum & mask << 24) >> 24) << "."
       << std::to_string((ipnum & mask << 16) >> 16) << "."
       << std::to_string((ipnum & mask << 8) >> 8) << "."
       << std::to_string(ipnum & mask);
    return ss.str();
}

uint32_t timestr_to_timestamp(std::string timestr)
{
    std::tm t = {};
    std::istringstream ss(timestr);
    ss >> std::get_time(&t, FILE_NAME_DATETIME_FORMAT.c_str());
    return static_cast<uint32_t>(mktime(&t));
}

std::string timestamp_to_timestr(time_t timestamp)
{
    std::tm t = *std::localtime(&timestamp);
    std::ostringstream ss;
    ss << std::put_time(&t, FILE_NAME_DATETIME_FORMAT.c_str());
    return ss.str();
}


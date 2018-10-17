#ifndef COMMON_H
#define COMMON_H

#include <cstdint>
#include <ctime>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <vector>

#include "defaults.h"

#include <regex>

using raw_data = std::vector<uint8_t>;
const std::regex IPSTR_REGEXP("^(25[0-5]|2[0-4][0-9]|[01]?[0-9]{1,2})\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9]{1,2})\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9]{1,2})\\."
                                 "(25[0-5]|2[0-4][0-9]|[01]?[0-9]{1,2})$");

uint16_t pack_be_to_uint16(raw_data::const_iterator data_it);
uint32_t pack_be_to_uint32(raw_data::const_iterator data_it);
uint32_t pack_le_to_uint32(raw_data::const_iterator data_it);
uint32_t ipstr_to_ipnum(const std::string& ipstr);
std::string ipnum_to_ipstr(const uint32_t ipnum);

uint32_t timestr_to_timestamp(std::string timestr);
std::string timestamp_to_timestr(time_t timestamp);

#endif // COMMON_H

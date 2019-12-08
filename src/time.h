#pragma once

#include <ctime>
#include <ostream>
#include <string>
#include <json.hpp>

namespace festi{

std::ostream& prettyPrintTime(std::string& timezone,time_t time,std::ostream& stream);
bool isSameDay(time_t d1,time_t d2);
bool isToday(time_t d);
void to_localtime(const std::string& timezone,time_t t,struct tm&);


struct Time {
    uint8_t hour;
    uint8_t minute;
    using ptr = std::shared_ptr<Time>;
};

void to_json(nlohmann::json&, const Time&);
void from_json(const nlohmann::json&, Time&);



}
#pragma once
#include <string>
#include <fstream>
#include <ctime>
#include <json.hpp>

namespace festi {

bool canRead(const std::string& filename);
std::string readFile(const std::string& filename);
nlohmann::json readJson(const std::string& filename);
std::string readUrl(const std::string& url);
nlohmann::json readJsonUrl(const std::string& url);

void prettyPrintTime(time_t time,std::ostream& stream);
bool isSameDay(time_t d1,time_t d2);
bool isToday(time_t d);


}
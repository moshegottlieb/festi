#pragma once
#include <string>
#include <fstream>
#include "time.h"
#include <json.hpp>

namespace festi {

bool canRead(const std::string& filename);
std::string readFile(const std::string& filename);
nlohmann::json readJson(const std::string& filename);
std::string readUrl(const std::string& url);
nlohmann::json readJsonUrl(const std::string& url);


}
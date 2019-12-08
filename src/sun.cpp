#include "sun.h"
#include "util.h"
#include "config.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <ios>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <csignal>
#include <json.hpp>
#include <ctime>
#include <unistd.h>

using json = nlohmann::json;

using namespace festi;

static bool _need_reload = true;

const std::string& cacheFile() noexcept {
    static bool sInit = false;
    static std::string sFilename;
    if (!sInit){
        sInit = true;
        std::stringstream s;
        s << "/tmp/fest_cache." << getuid() << ".json";
        sFilename = s.str();
    }
    return sFilename;
}

Sun& Sun::shared(){
    static Sun sSun;
    return sSun;
}

Sun::Sun():_pig(),_pin(_pig),_sunrise(0),_sunset(0){
}

bool Sun::needDownload() const noexcept{
    if (_sunrise == 0){ 
        return true;
    }
    return !isToday(_sunrise);
}

/*
bad, we need to save "today's" sunrise/sunset, and after midnight, save tomorrow's
*/

bool Sun::lightsOn() const noexcept{
    auto now = time(nullptr);
    return (now >= _sunset || now <= _sunrise);
}

static Sun* sSun;

void Sun::breakHandle(int signal){
    sSun->_pin.setValue(true);
    std::clog << "Ending due to signal " << signal << std::endl;
    exit(0);
}

void Sun::hupHandle(int signal){
    _need_reload = true;
}

void Sun::run(){
    sSun = this;

    signal(SIGINT,breakHandle);
    signal(SIGTERM,breakHandle);
    signal(SIGHUP,hupHandle);
    _last = 0;
    bool was_on = false;
    while (true){
        if (_need_reload){
            Config::shared().reload();
            _need_reload = false;
            _pin.setPin(Config::shared().pin);
            _pin.setValue(true); // turn off
            _pin.setMode(Pig::Pin::Output);
        }
        if (needDownload()){
            std::clog << "Need download" << std::endl;
            time_t now = time(nullptr);
            if ((now - _last) < 1){
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            try {
                if (!readCache()){
                    download();
                    std::clog << "Download complete" << std::endl;
                    writeCache();
                }
                _last = time(nullptr);
                std::clog << "Sunrise: ";
                prettyPrintTime(_timezone,_sunrise,std::clog) << std::endl;
                std::clog << "Sunset: ";
                prettyPrintTime(_timezone,_sunset,std::clog) << std::endl;
            } catch (const std::exception& e){
                std::cerr << "Download error: " << e.what() << std::endl;
                continue;
            }
        }
        if (lightsOn()){
            if (!was_on){
                was_on = true;
                std::clog << "Turning on" << std::endl;
	            _pin.setValue(false); // turn on the lights
            	_pin.setStrength(1);
            }
            bool dir = false;
            for (int j=0;j<2;++j){
                for (int i=1;i<=16;++i){
                    _pin.setStrength(dir ? i : 17 - i);
                    std::this_thread::sleep_for(std::chrono::milliseconds(30));
                }
                dir ^= true;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        } else {
            if (was_on){
	            _pin.setValue(true); // turn off
                was_on = false;
                std::clog << "Turning off" << std::endl;
            }
        }
    }
}

bool Sun::readCache(){
    try {
        auto parsed = readJson(cacheFile());                       
        time_t sunrise = parsed["sunrise"].get<int>();
        time_t sunset = parsed["sunset"].get<int>();
        std::string zone = parsed["timezone"].get<std::string>();
        if (isToday(sunrise)){
            std::clog << "Using cached values" << std::endl;
            _sunrise = sunrise;
            _sunset = sunset;
            _timezone = zone;
            return true;
        }
    } catch (const std::exception& e){
        std::cerr << "Error reading /tmp/festi_cache.json " << e.what() << std::endl;
    }
    std::clog << "Not using cached values" << std::endl;
    return false;
}
void Sun::writeCache(){
    try {
        std::ofstream cache(cacheFile(),std::ofstream::trunc);
        if (!cache) throw std::runtime_error("Error opening file");
        cache << "{\n";
        cache << "\t\"sunrise\":" << _sunrise << "," << std::endl;
        cache << "\t\"sunset\":" << _sunset << "," << std::endl;
        cache << "\t\"timezone\": \"" << _timezone << "\"" << std::endl;
        cache << "}";
    } catch (const std::exception& e){
        std::cerr << "Error writing cache file " << cacheFile() << ": " << e.what() << std::endl;
    }
}

void Sun::download(){
    // Will move soon to the whole config stuff, for now, this is what we've got
    std::stringstream url;
    url << "https://api.darksky.net/forecast/" << Config::shared().dark.key << "/";
    url << Config::shared().dark.location.latitude << "," << Config::shared().dark.location.longitude << "?exclude=minutely,hourly,alerts,flags&units=si";
    auto parsed = readJsonUrl(url.str());
    if (parsed.empty()){
        throw std::runtime_error("Unable to parse json");
    }
    _sunrise = parsed.at("daily").at("data").at(0).at("sunriseTime").get<int>();
    _sunset = parsed.at("daily").at("data").at(0).at("sunsetTime").get<int>();
    _timezone = parsed.at("timezone").get<std::string>();
    
    struct tm t;
    to_localtime(_timezone,_sunrise,t);
    t.tm_hour = 1;
    t.tm_min = 0;
    t.tm_sec = 0;
    _sunrise = mktime(&t);
}

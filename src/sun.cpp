#include "sun.h"
#include "util.h"
#include "config.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <ios>
#include <fstream>
#include <strstream>
#include <iomanip>
#include <csignal>
#include <json.hpp>

using json = nlohmann::json;

using namespace festi;

static const std::string CACHE_FILE = "/tmp/festi_cache.json";

Sun::Sun():_pig(),_pin(24,_pig),_sunrise(0),_sunset(0){
    _pin.setMode(Pig::Pin::Output);
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


void Sun::run(){
    sSun = this;
    _pin.setValue(true); // turn off
    signal(SIGINT,breakHandle);
    signal(SIGTERM,breakHandle);
    _last = 0;
    bool was_on = false;
    while (true){
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
                prettyPrintTime(_sunrise,std::clog);
                std::clog << "Sunset: ";
                prettyPrintTime(_sunset,std::clog);
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
        auto parsed = readJson(CACHE_FILE);                       
        time_t sunrise = parsed["sunrise"].get<int>();
        time_t sunset = parsed["sunset"].get<int>();
        if (isToday(sunrise) && isToday(sunset)){
            _sunrise = sunrise;
            _sunset = sunset;
            return true;
        }
    } catch (const std::exception& e){
        std::cerr << "Error reading /tmp/festi_cache.json " << e.what() << std::endl;
    }
    return false;
}
void Sun::writeCache(){
    try {
        std::ofstream cache(CACHE_FILE,std::ofstream::trunc);
        if (!cache) throw std::runtime_error("Error opening file");
        cache << "{\n";
        cache << "\t\"sunrise\":" << _sunrise << "," << std::endl;
        cache << "\t\"sunset\":" << _sunset << std::endl;
        cache << "}";
    } catch (const std::exception& e){
        std::cerr << "Error writing cache file " << CACHE_FILE << ": " << e.what() << std::endl;
    }
}

void Sun::download(){
    // Will move soon to the whole config stuff, for now, this is what we've got
    std::strstream url;
    url << "https://api.darksky.net/forecast/" << Config::shared().dark.key << "/";
    url << Config::shared().dark.location.latitude << "," << Config::shared().dark.location.longitude << "?exclude=minutely,hourly,alerts,flags&units=si";
    std::string surl(url.str(),url.pcount());
    auto parsed = readJsonUrl(surl);
    if (parsed.empty()){
        throw std::runtime_error("Unable to parse json");
    }
    _sunrise = parsed.at("daily").at("data").at(0).at("sunriseTime").get<int>();
    _sunset = parsed.at("daily").at("data").at(0).at("sunsetTime").get<int>();
    printf(ctime(&_sunrise));
    struct tm t;
    localtime_r(&_sunrise,&t);
    t.tm_hour = 1;
    t.tm_min = 0;
    t.tm_sec = 0;
    _sunrise = mktime(&t);
    printf(ctime(&_sunrise));
}

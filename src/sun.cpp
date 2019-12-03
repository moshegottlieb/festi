#include "sun.h"
#include <curl/curl.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <ios>
#include <fstream>
#include <iomanip>
#include <csignal>
#include "nlohmann_json/json.hpp"

#define max(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b;       \
})

#define min(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b;       \
})

using json = nlohmann::json;

static const std::string CACHE_FILE = "/tmp/festi_cache.json";

#define BUFFER_SIZE 16384

struct Buffer {
    Buffer(){
        len = 0;
        data[0] = 0;
    }
    char data[BUFFER_SIZE];
    size_t len;
};

Sun::Sun():_pig(),_pin(24,_pig),_sunrise(0),_sunset(0){
    _pin.setMode(Pig::Pin::Output);
}

void prettyPrintTime(time_t time,std::ostream& stream){
    struct tm t;
    localtime_r(&time,&t);
    stream << std::put_time(&t,"%X") << std::endl;
}


bool isSameDay(time_t d1,time_t d2){
    struct tm ld1;
    localtime_r(&d1,&ld1);
    struct tm ld2;
    localtime_r(&d2,&ld2);
    return ld1.tm_mday == ld2.tm_mday && ld1.tm_mon == ld2.tm_mon && ld1.tm_year == ld2.tm_year;
}
bool isToday(time_t d){
    return isSameDay(d,time(nullptr));
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
        std::ifstream cache(CACHE_FILE,std::ifstream::binary);
        if (!cache) return false; // no file
        cache.seekg(0,cache.end);
        if (!cache) throw std::runtime_error("Seek to end error");
        size_t length = cache.tellg();
        if (length > 4095) throw std::runtime_error("File too big");
        cache.seekg(0,cache.beg);
        if (!cache) throw std::runtime_error("Seek to start error");
        char buffer[length+1];
        cache.read(buffer,length);
        if (!cache) throw std::runtime_error("Error reading file");
        buffer[length] = '\0';
        auto parsed = json::parse(buffer);                        
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
    auto url = "https://api.darksky.net/forecast/5e7ebea6c84678ad44c6f3371436e10a/52.5082702,13.4672354?exclude=minutely,hourly,alerts,flags&units=si";
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    Buffer buffer;
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            std::string error = curl_easy_strerror(res);
            curl_easy_cleanup(curl);
            throw std::runtime_error(error);
        }
        curl_easy_cleanup(curl);
    }
    auto parsed = json::parse(buffer.data);
    if (parsed.empty()){
        throw std::runtime_error("Unable to parse json");
    }
    _sunrise = parsed["daily"]["data"][0]["sunriseTime"].get<int>();
    _sunset = parsed["daily"]["data"][0]["sunsetTime"].get<int>();
    printf(ctime(&_sunrise));
    struct tm t;
    localtime_r(&_sunrise,&t);
    t.tm_hour = 1;
    t.tm_min = 0;
    t.tm_sec = 0;
    _sunrise = mktime(&t);
    printf(ctime(&_sunrise));
}

size_t Sun::write_data(void *ptr, size_t size, size_t nmemb, void *stream){
    size_t len = size * nmemb;
    Buffer& buffer = *((Buffer*)stream);
    if ((buffer.len + len) < BUFFER_SIZE){
        memcpy(buffer.data + buffer.len,ptr,len);
        buffer.len += len;
        buffer.data[buffer.len] = 0;
        return len;
    } else {
        return 0;
    }
}

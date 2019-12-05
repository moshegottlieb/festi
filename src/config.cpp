#include "config.h"
#include "util.h"
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

using json = nlohmann::json;

namespace festi {

Config& Config::shared(){
    static bool sInit = false;
    static Config config;
    if (!sInit){
        sInit = true;
        config.reload();
    }
    return config;
}

Config::Config(){
}

void Config::reload(){
    // Reset
    sunrise = sunset = Time::ptr(nullptr);
    // Get home dir
    const char *homedir;
    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }
    if (!homedir || !load(std::string(homedir) + "/.config/festi.json")){
        if (!load("/etc/festi.json")){
            sunrise = Time::ptr(new Time());
            sunset = Time::ptr(new Time());
            sunrise->hour = 1; // Stop at 1am
            sunset->hour = 19; // Default sunset at 19:00
        }
    }
}

void to_json(nlohmann::json& j, const Config::Time& t){
    j = json { "hour", t.hour, "minute", t.minute };
}
void from_json(const nlohmann::json& j, Config::Time& t){
    t.hour = j.at("hour").get<double>();
    t.minute = j.at("minute").get<double>();
}

void to_json(nlohmann::json& j, const Config::DarkSky& d){
    j = json { "location" , d.location, "key" , d.key };
}
void from_json(const nlohmann::json& j, Config::DarkSky& d){
    d.key = j.at("key");
    d.location = j.at("location");
}

void to_json(nlohmann::json& j, const class Config::DarkSky::Location& l){
    j = json{{"latitude", l.latitude}, {"longitude", l.longitude}};
}
void from_json(const nlohmann::json& j, class Config::DarkSky::Location& l){
    l.longitude = j.at("longitude").get<double>();
    l.latitude = j.at("latitude").get<double>();
}

void to_json(nlohmann::json& j, const Config& c){
    j = nlohmann::json { "dark-sky" , c.dark };
    if (c.sunset){
        j["sunset"] = *c.sunset;
    }
    if (c.sunrise){
        j["sunrise"] = *c.sunrise;
    }
}
void from_json(const nlohmann::json& j, Config& c){
    c.dark = j.at("dark-sky");
    auto i = j.find("sunset");
    if (i != j.end()){
        c.sunset = Config::Time::ptr(new Config::Time(i->get<Config::Time>()));
    }
    i = j.find("sunrise");
    if (i != j.end()){
        c.sunrise = Config::Time::ptr(new Config::Time(i->get<Config::Time>()));
    }
}

bool Config::load(const std::string& path){
    try {
        try {
            _json = readJson(path);
            *this = _json.get<Config>();
            
        } catch (const std::exception& e){
            std::cerr << "Error loading configuration file: " << path << std::endl;
        }
        return true;
    } catch (const std::exception& e){
        std::clog << "Could not load configuration file: " << path << std::endl;
        return false;
    }
}

const json& Config::json() const noexcept {
    return _json;
}

Config::~Config(){

}

void parse(){
   
        std::string user;
        std::string forUser;
    if (forUser.empty()){
        user = getenv("USER");
        if (user.empty()){
            throw std::runtime_error("Could not determine current user");
        }
    } else {
        user = forUser;
    }
    if (user == "root"){
        throw std::runtime_error("Refusing to configure as root, you can still do it yourself if you know what you're doing.");
    }

}

} // namespace festi
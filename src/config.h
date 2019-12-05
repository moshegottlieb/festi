#pragma once

#include <string>
#include <json.hpp>

namespace festi {


class Config {
public:
    Config();
    virtual ~Config();
    void reload();
    static Config& shared();

    struct Time {
        uint8_t hour;
        uint8_t minute;
        using ptr = std::shared_ptr<Time>;
    };
    struct DarkSky {
        struct Location {
            double longitude;
            double latitude;
        };    
        std::string key;
        Location location;
    };
    const nlohmann::json& json() const noexcept;

    DarkSky dark;
    Time::ptr sunrise;
    Time::ptr sunset;


private:
    bool load(const std::string& path);
    nlohmann::json _json;
};

/*
void to_json(nlohmann::json&, const Config::Time&);
void from_json(const nlohmann::json&, Config::Time&);
void to_json(nlohmann::json&, const Config::DarkSky&);
void from_json(const nlohmann::json&, Config::DarkSky&);
void to_json(nlohmann::json& j, const class Config::DarkSky::Location& l);
void from_json(const nlohmann::json& j, class Config::DarkSky::Location& l);
void to_json(nlohmann::json&, const Config&);
void from_json(const nlohmann::json&, Config&);
*/

}
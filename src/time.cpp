#include "time.h"
#include <iomanip>
#include <cstring>
#include <cctz/time_zone.h>
#include <cctz/civil_time.h>


namespace festi{


std::ostream& prettyPrintTime(std::string& timezone,time_t time,std::ostream& stream){
    struct tm t;
    to_localtime(timezone,time,t);
    return stream << std::put_time(&t,"%X");
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


void to_localtime(const std::string& tz,time_t t,struct tm& tms){
    /*
    if (!tz.empty()){
        cctz::time_zone timezone;
        if (cctz::load_time_zone(tz, &timezone)){
            auto tp = std::chrono::system_clock::from_time_t(t);
            const cctz::civil_second cs = cctz::convert(tp, timezone);
            memset(&tms,0,sizeof(tms));
            tms.tm_sec = cs.second();
            tms.tm_min = cs.minute();
            tms.tm_hour = cs.hour();    
            tms.tm_mday = cs.day();
            tms.tm_mon = cs.month();
            tms.tm_year = cs.year();
            // We'll just leave these for now
            // tms.tm_wday;
            // tms.tm_yday;
            // tms.tm_isdst;
            return;
        }
    }*/
    localtime_r(&t,&tms);
}


void to_json(nlohmann::json& j, const Time& t){
    j = nlohmann::json { "hour", t.hour, "minute", t.minute };
}
void from_json(const nlohmann::json& j, Time& t){
    t.hour = j.at("hour").get<double>();
    t.minute = j.at("minute").get<double>();
}


    
}
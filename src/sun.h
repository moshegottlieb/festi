#pragma once

#include <ctime>
#include <cstdlib>
#include "pig.h"

namespace festi {

class Sun {
public:
    static Sun& shared();
    void run();
private:
    Sun();
private:
    void download();
    bool readCache();
    void writeCache();
    bool needDownload() const noexcept;
    /**
     * @return Lights should be on or off?
     */
    bool lightsOn() const noexcept;
private:
    static void breakHandle(int signal);
    static void hupHandle(int signal);
private:
    time_t _sunrise;
    time_t _sunset;
    time_t _last;
    std::string _timezone;
    Pig _pig;
    Pig::Pin _pin;
};

}
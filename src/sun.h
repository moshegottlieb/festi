#pragma once

#include <ctime>
#include <cstdlib>
#include "pig.h"

class Sun {
public:
    Sun();
    void run();
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
    static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);
    static void breakHandle(int signal);
private:
    time_t _sunrise;
    time_t _sunset;
    time_t _last;
    Pig _pig;
    Pig::Pin _pin;
};
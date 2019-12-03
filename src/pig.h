#pragma once

#include <stdexcept>
#include <pigpiod_if2.h>

class Pig {
public:
    Pig();
    virtual ~Pig() noexcept;
    operator int() const noexcept;

    class Pin {
    public:
        enum Mode {
            Input = PI_INPUT,
            Output = PI_OUTPUT,
            Alt0 = PI_ALT0,
            Alt1 = PI_ALT1,
            Alt2 = PI_ALT2,
            Alt3 = PI_ALT3,
            Alt4 = PI_ALT4,
            Alt5 = PI_ALT5
        };
        Pin(unsigned int pin,int pig) noexcept;
        void setValue(bool value);
        bool value() const;
        void setMode(Mode);
        Mode mode() const;
        /**
         * Strength is 1-16, affects entire PAD
         * +-------------+
         * | Pad | GPIO  |
         * +-----+-------+
         * | 0   | 0-27  |
         * +-----+-------+
         * | 1   | 28-45 |
         * +-----+-------+
         * | 2   | 46-53 |
         * +-----+-------+
         */
        void setStrength(unsigned int ma);
        unsigned int strength() const;
    private:
        unsigned int _pin;
        int _pig;
    };

    Pin operator[](unsigned int pin) noexcept;
private:
    int _pig;
};

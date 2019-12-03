#include "pig.h"

Pig::Pig(){
    _pig = pigpio_start(nullptr,nullptr);
    if (_pig < 0){
        throw std::runtime_error("Could not connect to pigpio daemon");
    }
}

Pig::operator int() const noexcept{
    return _pig;
}

Pig::Pin::Pin(unsigned int pin,int pig) noexcept :_pig(pig),_pin(pin){
}

void Pig::Pin::setValue(bool value){
    
    switch (gpio_write(_pig,_pin,value ? 1 : 0)){
        case PI_BAD_GPIO:
            throw std::runtime_error("gpio_write Bad gpio number");
        case PI_BAD_LEVEL:
            throw std::runtime_error("gpio_write Bad level");
        case PI_NOT_PERMITTED:
            throw std::runtime_error("gpio_write Not permitted");
        case 0:
            break;
        default:
            throw std::runtime_error("gpio_write unknown error");
    }
}

inline unsigned int _pad_for_pin(unsigned int pin){
    if (pin <= 27) {
        return 0;
    } else if (pin <= 45){
        return 1;
    } else return 2;
}

void Pig::Pin::setStrength(unsigned int ma){
    switch (set_pad_strength(_pig,_pad_for_pin(_pin),ma)){
         case PI_BAD_PAD:
            throw std::runtime_error("set_pad_strength Bad pad");
         case PI_BAD_STRENGTH:
            throw std::runtime_error("set_pad_strength Bad strength");
         case 0:
            break;
         default:
            throw std::runtime_error("set_pad_strength Unknown error");
    }
}

unsigned int Pig::Pin::strength() const{
    auto ret = get_pad_strength(_pig,_pad_for_pin(_pin));
    if (ret < 0)
    switch (ret){
        case PI_BAD_PAD:
            throw std::runtime_error("get_pad_strength Bad pad");
        default:
            throw std::runtime_error("get_pad_strength Unknown error");
    }
    return (unsigned int)ret;
}


bool Pig::Pin::value() const{
    auto value = gpio_read(_pig,_pin);
    if (value ==  PI_BAD_GPIO){
        throw std::runtime_error("gpio_read - bad gpio");
    }
    return value ? true : false;
}
void Pig::Pin::setMode(Pig::Pin::Mode mode){
    auto ret = set_mode(_pig,_pin,mode);
    switch (ret){
        case PI_NOT_PERMITTED:
            throw std::runtime_error("set_mode Not permitted");
        case PI_BAD_GPIO:
            throw std::runtime_error("set_mode Bad gpio number");
        case PI_BAD_MODE:
            throw std::runtime_error("set_mode Bad mode");
        case 0:
            break;
        default:
            throw std::runtime_error("set_mode unknown error");
    }
}

Pig::Pin::Mode Pig::Pin::mode() const{
    auto ret = get_mode(_pig,_pin);
    if (ret < 0){
        throw std::runtime_error("Could not get mode");
    }
    return (Mode)ret;
}

Pig::Pin Pig::operator[](unsigned int pin) noexcept{
    return Pin(pin,_pig);
}

Pig::~Pig() noexcept{
    pigpio_stop(_pig);
}

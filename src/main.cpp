
#include <iostream>
#include <unistd.h>
#include "sun.h"

int main(int,char**){
    festi::Sun s = festi::Sun::shared();
    s.run();
    return 0;
}
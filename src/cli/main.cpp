#include <iostream>
#include "Version.h"

int main() {
    std::cout << "Style Studio v" << style::version() << "\n";
    return 0;
}
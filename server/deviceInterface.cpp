#include <iostream>
#include <cstring>
#include "device.hpp"

#define MENU 1

template <typename data>

void decisor(int command, LightBulb device, data d){
    switch(command){
        case 1:
        case 4:
            device.setOn(d);
            break;
        case 2:
            device.setIntensity(d);
            
            break;
        case 3:
            device.setColor(d);
            break;
    }
}

void menu(){
    std::system("clear");
    int command = 0;
    while(MENU){
        std::cout << "###Device Management###" << std::endl;
        std::cout << "[1] - Ligar " << std::endl;
        std::cout << "[2] - Alterar Intensidade" << std::endl;
        std::cout << "[3] - Alterar Cor" << std::endl;
        std::cout << "[4] - Desligar" << std::endl;
        std::cout << ">> " << std::endl;
        std::cin >> command;
    }
}


#include <iostream>
#include <cstring>
#include <pthread.h>
#include "device.hpp"
#include "deviceComms.cpp"

// Função para exibir o menu
void showMenu() {
    std::cout << "###Device Management###" << std::endl;
        std::cout << "[1] - Alterar Status (ligado/desligado) " << std::endl;
        std::cout << "[2] - Alterar Intensidade" << std::endl;
        std::cout << "[3] - Alterar Cor" << std::endl;
        std::cout << ">> " << std::endl;
}

void* menu(void* device_ptr){
    std::system("clear");
    LightBulb *device = static_cast<LightBulb*>(device_ptr);
    
    int command = -1;
    
    while(command != 0){
        showMenu();
        std::cin >> command;

        switch(command) {
            case 1: {
                bool newState;
                std::cout << "Digite o novo status (1 para ligado, 0 para desligado)";
                std::cout << ">> ";
                std::cin >> newState;
                device->setOn(newState);
                break;
            }
            case 2: {
                int newIntensity;
                std::cout << "Digite a nova intensidade";
                std::cout << ">> ";
                std::cin >> newIntensity;
                device->setIntensity(newIntensity);
                break;
            }
            case 3: {
                int selectColor;
                std::cout << "Digite a nova cor: [0] - Branco [1] - Vermelho [2] - Roxo [3] - Azul";
                std::cout << ">> ";
                std::cin >> selectColor;
                Color newColor = static_cast<Color>(selectColor);
                device->setColor(newColor);
                break;
            }
            default:
                std::cout << "Opção inválida!" << std::endl;
        }
        std::system("clear");
    }
    pthread_exit(NULL);
}

int main(){

    LightBulb device(14700, false, 0, Color::WHITE);

    pthread_t sendThread, receiveThread, menuThread;

    // Criando threads de menu
    if (pthread_create(&menuThread, NULL, menu, &device) != 0) {
        std::cerr << "Erro ao criar thread de recebimento" << std::endl;
        return 1;
    }
 
    // Criando threads para envio e recebimento
    if (pthread_create(&sendThread, NULL, sendUDP, &device) != 0) {
        std::cerr << "Erro ao criar thread de envio" << std::endl;
        return 1;
    }

    if (pthread_create(&receiveThread, NULL, receiveTCP, NULL) != 0) {
        std::cerr << "Erro ao criar thread de recebimento" << std::endl;
        return 1;
    }

    // Esperar pelo término das threads
    pthread_join(sendThread, NULL);
    pthread_join(receiveThread, NULL);
    pthread_join(menuThread, NULL);
    return 0;
}
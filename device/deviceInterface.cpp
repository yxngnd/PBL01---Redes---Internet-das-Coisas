#include <iostream>
#include <cstring>
#include <pthread.h>
#include "device.hpp"
#include "deviceComms.cpp"

// Função de exibição do menu
void showMenu() {
    std::cout << "###Device Management###" << std::endl;
    std::cout << "[1] - Alterar Status (ligado/desligado) " << std::endl;
    std::cout << "[2] - Alterar Intensidade" << std::endl;
    std::cout << "[3] - Alterar Cor" << std::endl;
    std::cout << ">> " << std::endl;
}

// Função que roda o menu, exibe os comando e seleciona o comando, id e valor
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
                device->setColor(selectColor);
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

    LightBulb device((-1), false, 0, 0);

    pthread_t sendThread, receiveThread, menuThread;

    // Crian thread de menu
    pthread_create(&menuThread, NULL, menu, &device);
 
    // Cria thread para envio e recebimento
    pthread_create(&sendThread, NULL, sendUDP, &device);
    pthread_create(&receiveThread, NULL, receiveTCP, &device);

    pthread_join(sendThread, NULL);
    pthread_join(receiveThread, NULL);
    pthread_join(menuThread, NULL);
    return 0;
}
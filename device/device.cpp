#include "device.hpp"
// Implementação da classe do dispositivo
LightBulb::LightBulb(int id, bool on, int intensity, int color) : id(id), on(on), intensity(intensity), color(static_cast<Color>(color)){
    id = id;
    on = on;
    intensity = intensity;
    color = color;
}

void LightBulb::setId(int newId){
    id = newId;
}

int LightBulb::getId(){
    return id;
}

void LightBulb::setOn(bool state) {
    on = state;
}

bool LightBulb::getOn() const{
    return on;
}

void LightBulb::setIntensity(int newIntensity) {
    if (newIntensity >= 0 && newIntensity <= 100) {
        intensity = newIntensity;
    } else {
        std::cout << "Invalid intensity. Must be between 0 and 100." << std::endl;
    }
}

int LightBulb::getIntensity() const {
    return intensity;
}

void LightBulb::setColor(int selectColor) {
    Color newColor = static_cast<Color>(selectColor);
    color = newColor;
}

std::string LightBulb::getColor() const{
    switch (color) {
        case Color::WHITE:
            return "White";
            break;
        case Color::RED:
            return "Red";
            break;
        case Color::PURPLE:
            return "Purple";
            break;
        case Color::BLUE:
            return "Blue";
            break;
    }
    return "Colorless";
}

void LightBulb::displayStatus() const{
    std::cout << "Light Bulb Status:" << std::endl;
    std::cout << "ID: " << id << std::endl;
    std::cout << "On: " << (on ? "Yes" : "No") << std::endl;
    std::cout << "Intensity: " << intensity << "%" << std::endl;
    std::cout << "Color: " << getColor() << std::endl;
}

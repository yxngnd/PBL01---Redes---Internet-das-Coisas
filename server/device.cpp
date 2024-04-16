#include <iostream>
#include <string>
#include "device.hpp"

enum class Color { WHITE, RED, PURPLE, BLUE };

class LightBulb {
private:
    bool on;
    int intensity;
    Color color;

public:

    LightBulb() : on(false), intensity(0), color(Color::WHITE) {}

    void setOn(bool state) {
        on = state;
    }

    bool getOn() const{
        return on;
    }

    void setIntensity(int newIntensity) {
        if (newIntensity >= 0 && newIntensity <= 100) {
            intensity = newIntensity;
        } else {
            std::cout << "Invalid intensity. Must be between 0 and 100." << std::endl;
        }
    }

    int getIntensity() const {
        return intensity;
    }

    void setColor(Color newColor) {
        color = newColor;
    }

    Color getColor() const{
        return color;
    }

    void displayStatus() {
        std::cout << "Light Bulb Status:" << std::endl;
        std::cout << "On: " << (on ? "Yes" : "No") << std::endl;
        std::cout << "Intensity: " << intensity << "%" << std::endl;
        std::string colorStr;
        switch (color) {
            case Color::WHITE:
                colorStr = "White";
                break;
            case Color::RED:
                colorStr = "Red";
                break;
            case Color::PURPLE:
                colorStr = "Purple";
                break;
            case Color::BLUE:
                colorStr = "Blue";
                break;
        }
        std::cout << "Color: " << colorStr << std::endl;
    }
};

int main() {
    LightBulb light;

    light.displayStatus();

    light.setOn(true);
    light.setIntensity(50);
    light.setColor(Color::BLUE);

    light.displayStatus();

    return 0;
}
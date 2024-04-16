#ifndef LIGHT_BULB_HPP
#define LIGHT_BULB_HPP

#include <iostream>
#include <string>

// Enum para representar as opções de cor
enum class Color { WHITE, RED, PURPLE, BLUE };

class LightBulb {
private:
    bool on;
    int intensity;
    Color color;

public:
    LightBulb(bool on = false, int intensity = 0, Color color = Color::WHITE);

    void setOn(bool state);
    bool getOn() const;
    void setIntensity(int newIntensity);
    int getIntensity() const;
    void setColor(Color newColor);
    std::string getColor() const;
    void displayStatus() const;
};

#endif /* LIGHT_BULB_HPP */
#ifndef LIGHT_BULB_HPP
#define LIGHT_BULB_HPP
// Definição da classe do dispositivo
#include <iostream>
#include <string>

// Enum para representar as opções de cor
enum class Color { WHITE, RED, PURPLE, BLUE };

class LightBulb {
private:
    int id;
    bool on;
    int intensity;
    Color color;

public:
    LightBulb(int id, bool on, int intensity, int selectColor);

    void setId(int newId);
    int getId();
    void setOn(bool state);
    bool getOn() const;
    void setIntensity(int newIntensity);
    int getIntensity() const;
    void setColor(int selectColor);
    std::string getColor() const;
    void displayStatus() const;
};

#endif /* LIGHT_BULB_HPP */
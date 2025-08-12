#include <Arduino.h>
#include "Button.h"

//Constructor
Button::Button(int pin) {
    pinMode(pin, INPUT_PULLUP);
}
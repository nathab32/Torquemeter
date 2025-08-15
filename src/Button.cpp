#include <Arduino.h>
#include "Button.h"

//Constructor
Button::Button(int pin) {
    this->pin = pin;
    pinMode(pin, INPUT_PULLUP);
}

Button::Button(int pin, int debounceTime, int holdTime) {
    this->pin = pin;
    this->debounceTime = debounceTime;
    this->holdTime = holdTime;
    pinMode(pin, INPUT_PULLUP);
}

void Button::setDebounceTime(int debounceTime) {
    this->debounceTime = debounceTime;
}

void Button::setHoldTime(int holdTime) {
    this->holdTime = holdTime;
}

void Button::pressFunction(void(*function)()) {
    pFunction = function;
}

void Button::holdFunction(void(*function)()) {
    hFunction = function;
}

void Button::handle() {
    unsigned long time = millis();

    //do nothing if within debounce period
    if (time - debounceStart <= debounceTime) {
        return;
    }

    bool state = digitalRead(pin);
    
    //update debounce start time if button changes state
    if (previousState != state) {
        debounceStart = time;
        
        if (state == LOW) { //HIGH to LOW state
            pressTime = time;

        } else { //LOW to HIGH state
            //only do something if time is lower than hold, otherwise nothing
            if ((time - pressTime) < holdTime) {
                pFunction();
            }
        }
    }

    //trigger hold function when held for longer than hold time
    if ((state == LOW) && (time - pressTime >= holdTime)) {
        hFunction();
    }

    
    previousTime = time;
    previousState = state;
}
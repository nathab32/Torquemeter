#ifndef BUTTON_H
#define BUTTON_H

class Button {
    public:
        //Constructors
        Button(int pin);
        Button(int pin, int debounceTime, int holdTime);

        void setDebounceTime(int debounceTime);
        void setHoldTime(int holdTime);

        void pressFunction(void(*function)());
        void holdFunction(void(*function)());
        void handle();

    private:
        unsigned int pin;
        unsigned int debounceTime;
        unsigned int holdTime;
        
        void (*pFunction)();
        void (*hFunction)();

        unsigned long debounceStart = 0;
        bool previousState = HIGH;
        unsigned long pressTime = 0;
        unsigned long previousTime = 0;
};

#endif // BUTTON_H
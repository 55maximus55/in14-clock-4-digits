#include <buildTime.h>
#include <microWire.h>
#include <microDS3231.h>
#include <GyverButton.h>

int pin_button_set = 6;
int pin_button_up = 7;
int pin_button_next = 8;

GButton button_set(pin_button_set);
GButton button_up(pin_button_up);
GButton button_next(pin_button_next);

int pin_digits[] = {2, 3, 4, 5};
int pin_a0 = A0;
int pin_a1 = A1;
int pin_a2 = A2;
int pin_a3 = A3;
int pin_dot_left = 11;
int pin_dot_right = 12;

MicroDS3231 rtc;

void setup() {
    for (int i = 0; i < 4; i++) {
        pinMode(pin_digits[i], OUTPUT);
    }
    pinMode(A0, OUTPUT);
    pinMode(A1, OUTPUT);
    pinMode(A2, OUTPUT);
    pinMode(A3, OUTPUT);
    pinMode(pin_dot_left, OUTPUT);
    pinMode(pin_dot_right, OUTPUT);
    
    // кнопки
    button1.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
    button1.setTimeout(700);        // настройка таймаута на удержание (по умолчанию 500 мс)
    button1.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)
    button1.setType(HIGH_PULL);
    button1.setDirection(NORM_OPEN);

    button2.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
    button2.setTimeout(700);        // настройка таймаута на удержание (по умолчанию 500 мс)
    button2.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)
    button2.setType(HIGH_PULL);
    button2.setDirection(NORM_OPEN);

    button3.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
    button3.setTimeout(700);        // настройка таймаута на удержание (по умолчанию 500 мс)
    button3.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)
    button3.setType(HIGH_PULL);
    button3.setDirection(NORM_OPEN);

    if (rtc.lostPower()) {  //  при потере питания
        rtc.setTime(COMPILE_TIME);  // установить время компиляции
    }
}

void loop() {
    
}
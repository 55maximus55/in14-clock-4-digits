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
int pin_point_left = 11;
int pin_point_right = 12;

MicroDS3231 rtc;

void setDigit(bool d0, bool d1, bool d2, bool d3) {
    if (d0)
        digitalWrite(A0, HIGH);
    else
        digitalWrite(A0, LOW);
    if (d1)
        digitalWrite(A1, HIGH);
    else
        digitalWrite(A1, LOW);
    if (d2)
        digitalWrite(A2, HIGH);
    else
        digitalWrite(A2, LOW);
    if (d3)
        digitalWrite(A3, HIGH);
    else
        digitalWrite(A3, LOW);
}
void setDigit(int d) {
    switch (d)
    {
    case 0:
        setDigit(1, 1, 0, 0);
        break;
    case 1:
        setDigit(0, 0, 1, 0);
        break;
    case 2:
        setDigit(1, 0, 1, 0);
        break;
    case 3:
        setDigit(1, 0, 1, 1);
        break;
    case 4:
        setDigit(0, 0, 1, 1);
        break;
    case 5:
        setDigit(0, 0, 0, 1);
        break;
    case 6:
        setDigit(1, 0, 0, 1);
        break;
    case 7:
        setDigit(1, 0, 0, 0);
        break;
    case 8:
        setDigit(0, 0, 0, 0);
        break;
    case 9:
        setDigit(0, 1, 0, 0);
        break;
    default:
        setDigit(1, 1, 1, 1);
        break;
    }
}
void setPoint(bool left, bool state) {
    auto level = (state) ? HIGH : LOW;
    auto point_pin = (left) ? pin_point_left : pin_point_right;
    digitalWrite(point_pin, level);
}

void setup() {
    for (int i = 0; i < 4; i++) {
        pinMode(pin_digits[i], OUTPUT);
    }
    pinMode(A0, OUTPUT);
    pinMode(A1, OUTPUT);
    pinMode(A2, OUTPUT);
    pinMode(A3, OUTPUT);
    pinMode(pin_point_left, OUTPUT);
    pinMode(pin_point_right, OUTPUT);
    
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
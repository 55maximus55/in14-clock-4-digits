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

int pin_digits[] = {5, 4, 3, 2};
int pin_a0 = A0;
int pin_a1 = A1;
int pin_a2 = A2;
int pin_a3 = A3;
int pin_point_left = 11;
int pin_point_right = 12;

MicroDS3231 rtc;

void setDigitBool(bool d0, bool d1, bool d2, bool d3)
{
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
void setDigit(int d)
{
    switch (d)
    {
    case 0:
        setDigitBool(1, 1, 0, 0);
        break;
    case 1:
        setDigitBool(0, 0, 1, 0);
        break;
    case 2:
        setDigitBool(1, 0, 1, 0);
        break;
    case 3:
        setDigitBool(1, 0, 1, 1);
        break;
    case 4:
        setDigitBool(0, 0, 1, 1);
        break;
    case 5:
        setDigitBool(0, 0, 0, 1);
        break;
    case 6:
        setDigitBool(1, 0, 0, 1);
        break;
    case 7:
        setDigitBool(1, 0, 0, 0);
        break;
    case 8:
        setDigitBool(0, 0, 0, 0);
        break;
    case 9:
        setDigitBool(0, 1, 0, 0);
        break;
    default:
        setDigitBool(1, 1, 1, 1);
        break;
    }
}
void setPoint(bool left, bool state)
{
    auto level = (state) ? HIGH : LOW;
    auto point_pin = (left) ? pin_point_left : pin_point_right;
    digitalWrite(point_pin, level);
}

void turnLamp(int n, int digit, bool left_point, bool right_point)
{
    int lamp = pin_digits[n];

    digitalWrite(lamp, HIGH);
    setDigit(digit);
    delay(1);
    setDigit(-1);
    setPoint(false, left_point);
    delay(1);
    setPoint(false, false);
    setPoint(true, right_point);
    delay(1);
    setPoint(true, false);
    digitalWrite(lamp, LOW);
}

void setup()
{
    for (int i = 0; i < 4; i++)
    {
        pinMode(pin_digits[i], OUTPUT);
        digitalWrite(pin_digits[i], LOW);
    }
    pinMode(A0, OUTPUT);
    pinMode(A1, OUTPUT);
    pinMode(A2, OUTPUT);
    pinMode(A3, OUTPUT);
    pinMode(pin_point_left, OUTPUT);
    pinMode(pin_point_right, OUTPUT);

    digitalWrite(A0, LOW);
    digitalWrite(A1, LOW);
    digitalWrite(A2, LOW);
    digitalWrite(A3, LOW);
    digitalWrite(pin_point_left, LOW);
    digitalWrite(pin_point_right, LOW);

    // кнопки
    button_set.setDebounce(50);      // настройка антидребезга (по умолчанию 80 мс)
    button_set.setTimeout(700);      // настройка таймаута на удержание (по умолчанию 500 мс)
    button_set.setClickTimeout(600); // настройка таймаута между кликами (по умолчанию 300 мс)
    button_set.setType(HIGH_PULL);
    button_set.setDirection(NORM_OPEN);

    button_up.setDebounce(50);      // настройка антидребезга (по умолчанию 80 мс)
    button_up.setTimeout(700);      // настройка таймаута на удержание (по умолчанию 500 мс)
    button_up.setClickTimeout(600); // настройка таймаута между кликами (по умолчанию 300 мс)
    button_up.setType(HIGH_PULL);
    button_up.setDirection(NORM_OPEN);

    button_next.setDebounce(50);      // настройка антидребезга (по умолчанию 80 мс)
    button_next.setTimeout(700);      // настройка таймаута на удержание (по умолчанию 500 мс)
    button_next.setClickTimeout(600); // настройка таймаута между кликами (по умолчанию 300 мс)
    button_next.setType(HIGH_PULL);
    button_next.setDirection(NORM_OPEN);

    if (rtc.lostPower())
    {                              //  при потере питания
        rtc.setTime(COMPILE_TIME); // установить время компиляции
    }
}

void anti_poisoning()
{
    for (int j = 0; j < 4; j++)
    {
        for (int i = 0; i < 10; i++)
        {
            turnLamp(j, i, true, true);
        }
    }
}

void updateButtons()
{
    button_set.tick();
    button_up.tick();
    button_next.tick();
}
void resetButtons()
{
    button_set.resetStates();
    button_up.resetStates();
    button_next.resetStates();
}

class State
{
private:
public:
    State() {}
    ~State() {}
    void set()
    {
        resetButtons();
    }
    virtual void update()
    {
        turnLamp(0, 0, true, true);
        turnLamp(1, 0, true, true);
        turnLamp(2, 0, true, true);
        turnLamp(3, 0, true, true);
    }
};

class ClockState : public State
{
private:
public:
    ClockState(){};
    ~ClockState(){};
    void update() override
    {
        auto hours = rtc.getHours();
        auto minutes = rtc.getMinutes();
        auto month = rtc.getMonth();
        auto date = rtc.getDate();

        turnLamp(0, hours / 10, true, true);
        turnLamp(1, hours % 10, true, true);
        turnLamp(2, minutes / 10, true, true);
        turnLamp(3, minutes % 10, true, true);
    }
};

State *currentState = new ClockState();
void loop()
{
    updateButtons();
    currentState->update();
}
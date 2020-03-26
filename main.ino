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

// работа с лампами
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
void setPoint(bool right, bool state)
{
    auto level = (state) ? HIGH : LOW;
    auto point_pin = (right) ? pin_point_right : pin_point_left;
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
void antiPoisoning()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            turnLamp(i, j, true, true);
        }
    }
    for (int i = 3; i >= 0; i--)
    {
        for (int j = 0; j < 10; j++)
        {
            turnLamp(i, j, true, true);
        }
    }
}

// работа с кнопками
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

// состояния часов
#define STATE_NONE -1
#define STATE_CLOCK 0
#define STATE_SMENU 1
#define STATE_STIME 2
#define STATE_SDATE 3
#define STATE_SYEAR 4
int nextState = STATE_NONE;

class State
{
private:
public:
    State() {}
    ~State() {}
    virtual void set()
    {
        nextState = -1;
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
State *currentState;

class ClockState : public State
{
private:
    bool isAntiPoisoning(uint8_t s)
    {
        return s % 5 == 0;
    }
    bool isClock(uint8_t s)
    {
        return s % 5 != 0 && (s / 5) % 2 == 0;
    }
    bool isDate(uint8_t s)
    {
        return s % 5 != 0 && (s / 5) % 2 != 0;
    }

public:
    ClockState() {}
    ~ClockState() {}
    void set() override
    {
        State::set();
    }
    void update() override
    {
        auto hours = rtc.getHours();
        auto minutes = rtc.getMinutes();
        auto seconds = rtc.getSeconds();
        auto year = rtc.getYear();
        auto month = rtc.getMonth();
        auto date = rtc.getDate();

        if (isClock(seconds))
        {
            turnLamp(0, hours / 10, seconds % 2 == 0, seconds % 2 == 1);
            turnLamp(1, hours % 10, seconds % 2 == 0, seconds % 2 == 1);
            turnLamp(2, minutes / 10, seconds % 2 == 0, seconds % 2 == 1);
            turnLamp(3, minutes % 10, seconds % 2 == 0, seconds % 2 == 1);
        }
        else if (isDate(seconds))
        {
            turnLamp(0, date / 10, false, false);
            turnLamp(1, date % 10, false, true);
            turnLamp(2, month / 10, true, false);
            turnLamp(3, month % 10, false, false);
        }
        else if (isAntiPoisoning(seconds))
        {
            antiPoisoning();
        }

        if (button_set.isClick())
        {
            nextState = STATE_SMENU;
        }
    }
};
State *clockState = new ClockState();

class SetupMenuState : public State
{
private:
    int selection = 0;

public:
    SetupMenuState() {}
    ~SetupMenuState() {}
    void set() override
    {
        State::set();
        selection = 0;
    }
    void update() override
    {
        switch (selection)
        {
        case 0:
            turnLamp(0, 1, true, true);
            turnLamp(1, -1, true, true);
            turnLamp(2, -1, true, true);
            turnLamp(3, -1, true, true);
            break;
        case 1:
            turnLamp(0, -1, true, true);
            turnLamp(1, 2, true, true);
            turnLamp(2, -1, true, true);
            turnLamp(3, -1, true, true);
            break;
        case 2:
            turnLamp(0, -1, true, true);
            turnLamp(1, -1, true, true);
            turnLamp(2, 3, true, true);
            turnLamp(3, -1, true, true);
            break;

        default:
            break;
        }
        if (button_set.isClick())
        {
            nextState = 0;
        }
        if (button_up.isClick())
        {
            selection++;
            if (selection >= 3)
            {
                selection = 0;
            }
        }
        if (button_next.isClick())
        {
            switch (selection)
            {
            case 0:
                nextState = STATE_STIME;
                break;
            case 1:
                nextState = STATE_SDATE;
                break;
            case 2:
                nextState = STATE_SYEAR;
                break;

            default:
                break;
            }
        }
    }
};
State *setupMenuState = new SetupMenuState();

class SetupTimeState : public State
{
private:
    uint8_t setupHour;
    uint8_t setupMinute;
    int n;

public:
    SetupTimeState() {}
    ~SetupTimeState() {}
    void set() override
    {
        State::set();
        setupHour = rtc.getHours();
        setupMinute = rtc.getMinutes();

        n = 0;
    }
    void update() override
    {
        turnLamp(0, setupHour / 10, n == 0, n == 0);
        turnLamp(1, setupHour % 10, n == 1, n == 1);
        turnLamp(2, setupMinute / 10, n == 2, n == 2);
        turnLamp(3, setupMinute % 10, n == 3, n == 3);

        if (button_set.isClick())
        {
            rtc.setTime(0, setupMinute, setupHour, rtc.getDate(), rtc.getMonth(), rtc.getYear());
            nextState = STATE_SMENU;
        }
        if (button_up.isClick())
        {
            auto h1 = setupHour / 10;
            auto h2 = setupHour % 10;
            auto m1 = setupMinute / 10;
            auto m2 = setupMinute % 10;
            switch (n)
            {
            case 0:
                h1++;
                if (h1 > 2)
                {
                    h1 = 0;
                }
                setupHour = h1 * 10 + h2;
                break;
            case 1:
                h2++;
                if (h2 >= 10)
                {
                    h2 = 0;
                }
                setupHour = h1 * 10 + h2;
                break;
            case 2:
                m1++;
                if (m1 >= 6)
                {
                    m1 = 0;
                }
                setupMinute = m1 * 10 + m2;
                break;
            case 3:
                m2++;
                if (m2 >= 10)
                {
                    m2 = 0;
                }
                setupMinute = m1 * 10 + m2;
                break;

            default:
                break;
            }
        }
        if (button_next.isClick())
        {
            n++;
            if (n >= 4)
            {
                n = 0;
            }
        }
    }
};
State *setupTimeState = new SetupTimeState();

class SetupDateState : public State
{
private:
    uint8_t setupDate;
    uint8_t setupMonth;
    int n;

public:
    SetupDateState() {}
    ~SetupDateState() {}
    void set() override
    {
        State::set();
        setupDate = rtc.getDate();
        setupMonth = rtc.getMonth();

        n = 0;
    }
    void update() override
    {
        turnLamp(0, setupDate / 10, n == 0, n == 0);
        turnLamp(1, setupDate % 10, n == 1, n == 1);
        turnLamp(2, setupMonth / 10, n == 2, n == 2);
        turnLamp(3, setupMonth % 10, n == 3, n == 3);

        if (button_set.isClick())
        {
            rtc.setTime(rtc.getSeconds(), rtc.getMinutes(), rtc.getHours(), setupDate, setupMonth, rtc.getYear());
            nextState = STATE_SMENU;
        }
        if (button_up.isClick())
        {
            auto d1 = setupDate / 10;
            auto d2 = setupDate % 10;
            auto m1 = setupMonth / 10;
            auto m2 = setupMonth % 10;
            switch (n)
            {
            case 0:
                d1++;
                if (d1 > 3)
                {
                    d1 = 0;
                }
                setupDate = d1 * 10 + d2;
                break;
            case 1:
                d2++;
                if (d2 >= 10)
                {
                    d2 = 0;
                }
                setupDate = d1 * 10 + d2;
                break;
            case 2:
                m1++;
                if (m1 > 1)
                {
                    m1 = 0;
                }
                setupMonth = m1 * 10 + m2;
                break;
            case 3:
                m2++;
                if (m2 >= 10)
                {
                    m2 = 0;
                }
                setupMonth = m1 * 10 + m2;
                break;

            default:
                break;
            }
        }
        if (button_next.isClick())
        {
            n++;
            if (n >= 4)
            {
                n = 0;
            }
        }
    }
};
State *setupDateState = new SetupDateState();

class SetupYearState : public State
{
private:
    uint16_t setupYear;
    int n;

public:
    SetupYearState() {}
    ~SetupYearState() {}
    void set() override
    {
        State::set();
        setupYear = rtc.getYear();
        n = 0;
    }
    void update() override
    {
        auto y1 = setupYear / 1000;
        auto y2 = setupYear % 1000 / 100;
        auto y3 = setupYear % 100 / 10;
        auto y4 = setupYear % 10;

        turnLamp(0, y1, n == 0, n == 0);
        turnLamp(1, y2, n == 1, n == 1);
        turnLamp(2, y3, n == 2, n == 2);
        turnLamp(3, y4, n == 3, n == 3);

        if (button_set.isClick())
        {
            rtc.setTime(rtc.getSeconds(), rtc.getMinutes(), rtc.getHours(), rtc.getDate(), rtc.getMonth(), setupYear);
            nextState = STATE_SMENU;
        }
        if (button_up.isClick())
        {
            switch (n)
            {
            case 0:
                y1++;
                if (y1 >= 10)
                {
                    y1 = 0;
                }
                break;
            case 1:
                y2++;
                if (y2 >= 10)
                {
                    y2 = 0;
                }
                break;
            case 2:
                y3++;
                if (y3 >= 10)
                {
                    y3 = 0;
                }
                break;
            case 3:
                y4++;
                if (y4 >= 10)
                {
                    y4 = 0;
                }
                break;

            default:
                break;
            }
            setupYear = y1 * 1000 + y2 * 100 + y3 * 10 + y4;
        }
        if (button_next.isClick())
        {
            n++;
            if (n >= 4)
            {
                n = 0;
            }
        }
    }
};
State *setupYearState = new SetupYearState();

// стандартные функции работы с arduino
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

    // при потере питания
    if (rtc.lostPower())
    {
        rtc.setTime(COMPILE_TIME);
    }

    // установка стандартного состояния часов
    currentState = clockState;
}
void loop()
{
    updateButtons();
    currentState->update();
    switch (nextState)
    {
    case STATE_CLOCK:
        currentState = clockState;
        nextState = STATE_NONE;
        currentState->set();
        break;
    case STATE_SMENU:
        currentState = setupMenuState;
        nextState = STATE_NONE;
        currentState->set();
        break;
    case STATE_STIME:
        currentState = setupTimeState;
        nextState = STATE_NONE;
        currentState->set();
        break;
    case STATE_SDATE:
        currentState = setupDateState;
        nextState = STATE_NONE;
        currentState->set();
        break;
    case STATE_SYEAR:
        currentState = setupYearState;
        nextState = STATE_NONE;
        currentState->set();
        break;

    default:
        break;
    }
}
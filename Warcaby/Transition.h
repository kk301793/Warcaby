#pragma once

#include <SFML/System.hpp>

//klasa odpowiadaj¹ca za animacjê kiedy pionek zmienia pozycjê

class Transition
{
public:
    Transition();
    ~Transition();

    float evaluate(sf::Time time);

    void setMinValue(float value);
    float getMinValue() const;
    void setMaxValue(float value);
    float getMaxValue() const;

    void setStartTime(sf::Time time);

    void setLasting(sf::Time time);

    bool isFinished() const;


private:
    float minValue, maxValue;
    sf::Time startTime, lasting;
    bool finished;
};

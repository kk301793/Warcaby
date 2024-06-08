#include "Transition.h"

Transition::Transition() : minValue(), maxValue(), lasting(), finished(true) {}

Transition::~Transition() {}

float Transition::evaluate(sf::Time time)
{
    if (time <= startTime) {
        return minValue;
    }
    // Je�li czas jest mniejszy lub r�wny startTime, zwr�� minValue

    if (time >= startTime + lasting) {
        finished = true;
        return maxValue;
    }
    // Je�li czas jest wi�kszy lub r�wny startTime + lasting, ustaw finished na true i zwr�� maxValue

    float progress = (time - startTime) / lasting;
    // Oblicza post�p, czyli stosunek up�ywu czasu do ca�kowitego czasu trwania

    float valueSpan = maxValue - minValue;
    // Oblicza r�nic� pomi�dzy maxValue a minValue

    return minValue + valueSpan;
    // Zwraca warto�� oznaczaj�c� warto�� po�redni� na podstawie post�pu i r�nicy warto�ci
}

void Transition::setMinValue(float value)
{
    minValue = value;
    // Ustawia minValue na podan� warto��
}

float Transition::getMinValue() const
{
    return minValue;
    // Zwraca minValue
}

void Transition::setMaxValue(float value)
{
    maxValue = value;
    // Ustawia maxValue na podan� warto��
}

float Transition::getMaxValue() const
{
    return maxValue;
    // Zwraca maxValue
}

void Transition::setStartTime(sf::Time time)
{
    finished = false;
    startTime = time;
    // Ustawia startTime na podany czas i ustawia finished na false
}

void Transition::setLasting(sf::Time time)
{
    lasting = time;
    // Ustawia lasting na podany czas
}

bool Transition::isFinished() const
{
    return finished;
    // Zwraca warto�� finished, kt�ra wskazuje, czy przej�cie zosta�o zako�czone
}
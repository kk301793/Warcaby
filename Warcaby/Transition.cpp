#include "Transition.h"

Transition::Transition() : minValue(), maxValue(), lasting(), finished(true) {}

Transition::~Transition() {}

float Transition::evaluate(sf::Time time)
{
    if (time <= startTime) {
        return minValue;
    }
    // Jeœli czas jest mniejszy lub równy startTime, zwróæ minValue

    if (time >= startTime + lasting) {
        finished = true;
        return maxValue;
    }
    // Jeœli czas jest wiêkszy lub równy startTime + lasting, ustaw finished na true i zwróæ maxValue

    float progress = (time - startTime) / lasting;
    // Oblicza postêp, czyli stosunek up³ywu czasu do ca³kowitego czasu trwania

    float valueSpan = maxValue - minValue;
    // Oblicza ró¿nicê pomiêdzy maxValue a minValue

    return minValue + valueSpan;
    // Zwraca wartoœæ oznaczaj¹c¹ wartoœæ poœredni¹ na podstawie postêpu i ró¿nicy wartoœci
}

void Transition::setMinValue(float value)
{
    minValue = value;
    // Ustawia minValue na podan¹ wartoœæ
}

float Transition::getMinValue() const
{
    return minValue;
    // Zwraca minValue
}

void Transition::setMaxValue(float value)
{
    maxValue = value;
    // Ustawia maxValue na podan¹ wartoœæ
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
    // Zwraca wartoœæ finished, która wskazuje, czy przejœcie zosta³o zakoñczone
}
#include "Draughts.h"

#include <cstdlib>
#include <memory>
#include <iostream>





int main()
{
    // Tworzenie unikalnego wska�nika na obiekt klasy Draughts
    std::unique_ptr<Draughts> draughts = std::make_unique<Draughts>();
    draughts->init(); // Inicjalizacja gry

    sf::Clock gameClock; // Zegar u�ywany do �ledzenia czasu gry
    sf::Time beforeRender = gameClock.getElapsedTime(); // Pocz�tkowy czas przed renderowaniem
    sf::Time updateTick(sf::microseconds(1'000'000U / 60U)); // Czas interwa�u aktualizacji (1/60 sekundy)
    unsigned long long updateNumber = 0ULL; // Licznik aktualizacji

    while (!draughts->shouldFinish()) { // Dop�ki gra nie powinna si� zako�czy�
        draughts->update(gameClock.getElapsedTime()); // Aktualizacja logiki gry

        while (gameClock.getElapsedTime() < beforeRender + updateTick); // Oczekiwanie na up�yw czasu odswiezania

        beforeRender = gameClock.getElapsedTime(); // Aktualizacja czasu przed renderowaniem

        draughts->render(); // Renderowanie gry
        ++updateNumber; // Inkrementacja licznika aktualizacji
    }

    draughts->finish(); // Zako�czenie gry

    return 0;
}
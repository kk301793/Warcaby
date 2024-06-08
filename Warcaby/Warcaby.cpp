#include "Draughts.h"

#include <cstdlib>
#include <memory>
#include <iostream>





int main()
{
    // Tworzenie unikalnego wskaŸnika na obiekt klasy Draughts
    std::unique_ptr<Draughts> draughts = std::make_unique<Draughts>();
    draughts->init(); // Inicjalizacja gry

    sf::Clock gameClock; // Zegar u¿ywany do œledzenia czasu gry
    sf::Time beforeRender = gameClock.getElapsedTime(); // Pocz¹tkowy czas przed renderowaniem
    sf::Time updateTick(sf::microseconds(1'000'000U / 60U)); // Czas interwa³u aktualizacji (1/60 sekundy)
    unsigned long long updateNumber = 0ULL; // Licznik aktualizacji

    while (!draughts->shouldFinish()) { // Dopóki gra nie powinna siê zakoñczyæ
        draughts->update(gameClock.getElapsedTime()); // Aktualizacja logiki gry

        while (gameClock.getElapsedTime() < beforeRender + updateTick); // Oczekiwanie na up³yw czasu odswiezania

        beforeRender = gameClock.getElapsedTime(); // Aktualizacja czasu przed renderowaniem

        draughts->render(); // Renderowanie gry
        ++updateNumber; // Inkrementacja licznika aktualizacji
    }

    draughts->finish(); // Zakoñczenie gry

    return 0;
}
#pragma once

#include <cstdint>
#include <list>

#include "Field.h"
#include "Piece.h"

//Drzewo krok�w - analiza mo�liwych ruch�w 

struct StepTree {
    StepTree();
    ~StepTree();

    // Oblicza maksymaln� liczb� zbi� dla tego kroku i jego nast�pnych.
    void evalCapturesNumber();
    void validate(int capturesNumber);
    // Wskazuje na rodzica danego wierzcho�ka. Je�li nullptr to wierzcho�ek jest wierzcho�kiem drzewa.
    StepTree* parent;
    // Wskazuje na pole na kt�rym dany pionek zosta� przemieszczony.
    Field* field;
    // Je�li nast�pi�o bicie pionka przeciwnika ta zmienna b�dzie przechowywa� wska�nik na ten pionek, je�li nie bedzie przechowywac nullptr
    Piece* capture;
    // Lista krok�w kt�re s� dost�pne po obecnym ruchu. Ta lista jest pusta je�li to ostatni krok. 
    std::list<StepTree*> nextStepList;
    // Ustala czy wybrany ruch mo�e by� dodany do tury, ograniczeniem b�d� zasady gry w warcaby. Przyk�adowo gracz musi
    // wykona� tur� o maksymalnej liczbie bi� pionk�w przeciwnika.
    bool isValid;
    int capturesNumber;
};
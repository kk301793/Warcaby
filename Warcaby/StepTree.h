#pragma once

#include <cstdint>
#include <list>

#include "Field.h"
#include "Piece.h"

//Drzewo kroków - analiza mo¿liwych ruchów 

struct StepTree {
    StepTree();
    ~StepTree();

    // Oblicza maksymaln¹ liczbê zbiæ dla tego kroku i jego nastêpnych.
    void evalCapturesNumber();
    void validate(int capturesNumber);
    // Wskazuje na rodzica danego wierzcho³ka. Jeœli nullptr to wierzcho³ek jest wierzcho³kiem drzewa.
    StepTree* parent;
    // Wskazuje na pole na którym dany pionek zosta³ przemieszczony.
    Field* field;
    // Jeœli nast¹pi³o bicie pionka przeciwnika ta zmienna bêdzie przechowywaæ wskaŸnik na ten pionek, jeœli nie bedzie przechowywac nullptr
    Piece* capture;
    // Lista kroków które s¹ dostêpne po obecnym ruchu. Ta lista jest pusta jeœli to ostatni krok. 
    std::list<StepTree*> nextStepList;
    // Ustala czy wybrany ruch mo¿e byæ dodany do tury, ograniczeniem bêd¹ zasady gry w warcaby. Przyk³adowo gracz musi
    // wykonaæ turê o maksymalnej liczbie biæ pionków przeciwnika.
    bool isValid;
    int capturesNumber;
};
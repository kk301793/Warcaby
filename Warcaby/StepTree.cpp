#include "StepTree.h"
#include "Field.h"
#include "Piece.h"

#include <algorithm>

StepTree::StepTree() : parent(nullptr), field(nullptr), capture(nullptr), capturesNumber(0U), isValid(false) {}

void StepTree::evalCapturesNumber()
{
    int captureIncr = 0U;
    if (capture != nullptr)
        captureIncr = 1U;
    capturesNumber += captureIncr;
    // Zwi�ksza liczb� zdobyczy o 1, je�li jest dost�pna zdobycz (capture) dla tego kroku
    // Nast�pnie dodaje to do og�lnej liczby zdobyczy (capturesNumber)

    for (StepTree* subStep : nextStepList) {
        subStep->capturesNumber = capturesNumber;
        subStep->evalCapturesNumber();
        // Aktualizuje liczb� zdobyczy dla kolejnych krok�w i ich podkrok�w
    }

    for (StepTree* subStep : nextStepList) {
        capturesNumber = std::max(capturesNumber, subStep->capturesNumber);
        // Aktualizuje liczb� zdobyczy dla tego kroku, bior�c pod uwag� maksymaln� liczb� zdobyczy w�r�d podkrok�w
    }
}

void StepTree::validate(int capturesNumber)
{
    isValid = (capturesNumber == this->capturesNumber);
    // Sprawdza, czy liczba zdobyczy dla tego kroku jest zgodna z przekazan� liczb� zdobyczy

    if (parent == nullptr)
        isValid = (isValid && !nextStepList.empty());
    // Je�li ten krok jest korzeniem drzewa (nie ma rodzica), sprawdza, czy ma co najmniej jeden nast�pny krok

    for (StepTree* subStep : nextStepList) {
        subStep->validate(capturesNumber);
        // Sprawdza poprawno�� dla kolejnych krok�w i ich podkrok�w
    }
}

StepTree::~StepTree()
{
    for (StepTree* subStep : nextStepList) {
        delete subStep;
    }
    // Usuwa wszystkie podkroki tego kroku
}

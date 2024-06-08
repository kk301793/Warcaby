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
    // Zwiêksza liczbê zdobyczy o 1, jeœli jest dostêpna zdobycz (capture) dla tego kroku
    // Nastêpnie dodaje to do ogólnej liczby zdobyczy (capturesNumber)

    for (StepTree* subStep : nextStepList) {
        subStep->capturesNumber = capturesNumber;
        subStep->evalCapturesNumber();
        // Aktualizuje liczbê zdobyczy dla kolejnych kroków i ich podkroków
    }

    for (StepTree* subStep : nextStepList) {
        capturesNumber = std::max(capturesNumber, subStep->capturesNumber);
        // Aktualizuje liczbê zdobyczy dla tego kroku, bior¹c pod uwagê maksymaln¹ liczbê zdobyczy wœród podkroków
    }
}

void StepTree::validate(int capturesNumber)
{
    isValid = (capturesNumber == this->capturesNumber);
    // Sprawdza, czy liczba zdobyczy dla tego kroku jest zgodna z przekazan¹ liczb¹ zdobyczy

    if (parent == nullptr)
        isValid = (isValid && !nextStepList.empty());
    // Jeœli ten krok jest korzeniem drzewa (nie ma rodzica), sprawdza, czy ma co najmniej jeden nastêpny krok

    for (StepTree* subStep : nextStepList) {
        subStep->validate(capturesNumber);
        // Sprawdza poprawnoœæ dla kolejnych kroków i ich podkroków
    }
}

StepTree::~StepTree()
{
    for (StepTree* subStep : nextStepList) {
        delete subStep;
    }
    // Usuwa wszystkie podkroki tego kroku
}

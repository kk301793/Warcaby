#include "Player.h"
#include "Field.h"
#include "Piece.h"
#include "StepTree.h"

Player::Player(Color color, Board* board) : color(color), board(board),
fieldClicked_(nullptr),
turnPiece(nullptr) {}

Player::~Player() {}

void Player::fieldClicked(Field* const field)
{
    fieldClicked_ = field;
    // Aktualizuje klikni�te pole
}

void Player::turnFinished()
{
    turnPiece = nullptr;
    fieldClicked_ = nullptr;
    // Resetuje turnPiece i fieldClicked_ na nullptr, sygnalizuj�c zako�czenie tury
}

Piece* Player::getTurnPiece(const std::map<Piece*, std::unique_ptr<StepTree>>& possibleTurns)
{
    if (fieldClicked_ == nullptr)
        return nullptr;
    // Je�li �adne pole nie zosta�o klikni�te, zwr�� nullptr

    Piece* clickedPiece = fieldClicked_->getCurrentPiece();
    if (clickedPiece == nullptr)
        return nullptr;
    // Je�li na klikni�tym polu nie ma �adnej figury, zwr�� nullptr

    if (clickedPiece->getColor() != color)
        return nullptr;
    // Je�li klikni�ta figura nie nale�y do koloru gracza, zwr�� nullptr

    if (!possibleTurns.at(clickedPiece)->isValid)
        return nullptr;
    // Je�li klikni�ta figura nie ma �adnych mo�liwych ruch�w, zwr�� nullptr

    turnPiece = clickedPiece;
    // Ustaw turnPiece na klikni�t� figur�
    return turnPiece;
}

StepTree* Player::nextStep(Piece* const movingPiece, StepTree* const narrowedTurns)
{
    if (movingPiece != turnPiece)
        return nullptr;
    // Je�li ruchaj�ca figura nie pasuje do turnPiece, zwr�� nullptr

    if (narrowedTurns == nullptr)
        return nullptr;
    // Je�li narrowedTurns jest nullptr, zwr�� nullptr

    const std::list<StepTree*>& possibleNextSteps = narrowedTurns->nextStepList;
    // Pobierz list� mo�liwych kolejnych ruch�w z narrowedTurns

    for (StepTree* step : possibleNextSteps) {
        if (step->field == fieldClicked_ && step->isValid)
            return step;
        // Sprawd�, czy pole ruchu pasuje do klikni�tego pola i czy jest poprawne, a nast�pnie zwr�� ten ruch
    }

    return nullptr;
    // Je�li nie znaleziono poprawnego ruchu, zwr�� nullptr
}

sf::Clock Player::getClock()
{
    return this->playerClock;
    // Zwraca zegar gracza
}

Player::Color Player::getColor() const
{
    return color;
    // Zwraca kolor gracza
}

int Player::getLose()
{
    return lose;
}

void Player::setLose(int lose)
{
    this->lose = lose;
}

int Player::getWin()
{
    return win;
}

void Player::setWin(int win)
{
    this->win = win;
}

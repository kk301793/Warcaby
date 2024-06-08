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
    // Aktualizuje klikniête pole
}

void Player::turnFinished()
{
    turnPiece = nullptr;
    fieldClicked_ = nullptr;
    // Resetuje turnPiece i fieldClicked_ na nullptr, sygnalizuj¹c zakoñczenie tury
}

Piece* Player::getTurnPiece(const std::map<Piece*, std::unique_ptr<StepTree>>& possibleTurns)
{
    if (fieldClicked_ == nullptr)
        return nullptr;
    // Jeœli ¿adne pole nie zosta³o klikniête, zwróæ nullptr

    Piece* clickedPiece = fieldClicked_->getCurrentPiece();
    if (clickedPiece == nullptr)
        return nullptr;
    // Jeœli na klikniêtym polu nie ma ¿adnej figury, zwróæ nullptr

    if (clickedPiece->getColor() != color)
        return nullptr;
    // Jeœli klikniêta figura nie nale¿y do koloru gracza, zwróæ nullptr

    if (!possibleTurns.at(clickedPiece)->isValid)
        return nullptr;
    // Jeœli klikniêta figura nie ma ¿adnych mo¿liwych ruchów, zwróæ nullptr

    turnPiece = clickedPiece;
    // Ustaw turnPiece na klikniêt¹ figurê
    return turnPiece;
}

StepTree* Player::nextStep(Piece* const movingPiece, StepTree* const narrowedTurns)
{
    if (movingPiece != turnPiece)
        return nullptr;
    // Jeœli ruchaj¹ca figura nie pasuje do turnPiece, zwróæ nullptr

    if (narrowedTurns == nullptr)
        return nullptr;
    // Jeœli narrowedTurns jest nullptr, zwróæ nullptr

    const std::list<StepTree*>& possibleNextSteps = narrowedTurns->nextStepList;
    // Pobierz listê mo¿liwych kolejnych ruchów z narrowedTurns

    for (StepTree* step : possibleNextSteps) {
        if (step->field == fieldClicked_ && step->isValid)
            return step;
        // SprawdŸ, czy pole ruchu pasuje do klikniêtego pola i czy jest poprawne, a nastêpnie zwróæ ten ruch
    }

    return nullptr;
    // Jeœli nie znaleziono poprawnego ruchu, zwróæ nullptr
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

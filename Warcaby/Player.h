#pragma once

#include <map>
#include <memory>
#include <SFML/Graphics.hpp>

class Board;
class Field;
class Piece;
struct StepTree;

class Player
{
public:
    enum struct Color { White = 0, Black };

    Player(Color color, Board* const board);
    ~Player();

    void fieldClicked(Field* const field);

    void turnFinished();

    Piece* getTurnPiece(const std::map<Piece*, std::unique_ptr<StepTree>>& possibleTurns);

    StepTree* nextStep(Piece* const movingPiece, StepTree* const narrowedTurns);

    sf::Clock getClock();

    Color getColor() const;

    int getLose();
    void setLose(int lose);

    int getWin();
    void setWin(int win);


private:
    Color color;
    Board* board;
    sf::Clock playerClock;
    Field* fieldClicked_;
    Piece* turnPiece;
    int lose;
    int win;

};
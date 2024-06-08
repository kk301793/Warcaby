#pragma once

#include "Player.h"
#include "Transition.h"
#include "Field.h"

#include <SFML/Graphics.hpp>

//klasa Piece - klasa odpowiedzialna za jeden pionek

class Piece :public sf::Drawable, public sf::Transformable
{
public:
    enum struct PieceType { Man = 0, King };

    Piece(int pieceNumber, Player::Color color);
    ~Piece();

    void init();
    void update(sf::Time time);
    void draw(sf::RenderTarget& renderTarget, sf::RenderStates states) const;

    void setPosition(const sf::Vector2f& position);
    void setPosition(float x, float y);

    void transistToPosition(const sf::Vector2f& position, sf::Time startTime);
    void transistToPosition(float x, float y, sf::Time startTime);
    bool isMovingTransitionRunning() const;

    Player::Color getColor() const;



    void setCurrentField(Field* field);
    Field* getCurrentField() const;

    PieceType getPieceType() const;
    void upgrade();




private:
    int pieceId;
    PieceType pieceType;
    std::string pieceName;

    Player::Color color;
    sf::CircleShape pieceShape;
    sf::CircleShape crownShape;
    bool hidden;

    Transition xMovingTransition, yMovingTransition;

    Field* currentField;
};

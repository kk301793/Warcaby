#include "Field.h"

//pole na szachownicy

Field::Field(int x, int y, Color color) :
    boardPosition(x, y),
    fieldName(),
    color(color),
    fieldShape(4U),
    highlight(None),
    frameShape(4U),
    frameThickness(0.1f),
    currentPiece(nullptr)
{
}

Field::~Field() {}

void Field::init()
{
    // Inicjalizacja rozmiaru pola
    size.x = 1.0f;
    size.y = 1.0f;

    // Ustawienie kszta³tu pola
    fieldShape.setPoint(0U, sf::Vector2f(0.0f, 0.0f));
    fieldShape.setPoint(1U, sf::Vector2f(1.0f, 0.0f));
    fieldShape.setPoint(2U, sf::Vector2f(1.0f, 1.0f));
    fieldShape.setPoint(3U, sf::Vector2f(0.0f, 1.0f));

    // Ustawienie koloru pola na podstawie przekazanego argumentu
    sf::Color color = sf::Color(255U, 255U, 240U);
    if (this->color == Color::Black)
        color = sf::Color(178U, 147U, 97U);
    fieldShape.setFillColor(color);

    // Inicjalizacja kszta³tu obramowania pola
    frameShape.setPoint(0U, sf::Vector2f(frameThickness, frameThickness));
    frameShape.setPoint(1U, sf::Vector2f(1.0f - frameThickness, frameThickness));
    frameShape.setPoint(2U, sf::Vector2f(1.0f - frameThickness, 1.0f - frameThickness));
    frameShape.setPoint(3U, sf::Vector2f(frameThickness, 1.0f - frameThickness));
    frameShape.setFillColor(sf::Color::Transparent);
    frameShape.setOutlineThickness(frameThickness);
    frameShape.setOutlineColor(sf::Color::Transparent);
}

void Field::update(sf::Time time) {}

void Field::draw(sf::RenderTarget& renderTarget, sf::RenderStates states) const
{
    renderTarget.draw(fieldShape, states);
    renderTarget.draw(frameShape, states);
}


void Field::setPosition(float x, float y)
{
    fieldShape.setPosition(x, y);
    frameShape.setPosition(x, y);
    Transformable::setPosition(x, y);
}


void Field::setHighlight(Highlight highlight)
{
    this->highlight = highlight;
    if (highlight == None) {
        frameShape.setOutlineColor(sf::Color::Transparent);
    }
    else if (highlight == Selected) {
        frameShape.setOutlineColor(sf::Color(186U, 0U, 0U, 153U));
    }
    else if (highlight == AvailableMove) {
        frameShape.setOutlineColor(sf::Color(59U, 71U, 140U, 153U));
    }
    else if (highlight == AvailablePiece) {
        frameShape.setOutlineColor(sf::Color(59U, 71U, 140U, 153U));
    }
}


Field::Color Field::getColor() const
{
    return color;
}



void Field::setCurrentPiece(Piece* piece)
{
    currentPiece = piece;
}

Piece* Field::getCurrentPiece() const
{
    return currentPiece;
}

sf::Vector2i Field::getBoardPostion() const
{
    return boardPosition;
}

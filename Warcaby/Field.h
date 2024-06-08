#pragma once


#include <SFML/Graphics.hpp>

class Piece;

class Field : public sf::Drawable, public sf::Transformable
{
public:
    enum Color { White = 0, Black };
    enum Highlight { None = 0, Selected, AvailableMove, AvailablePiece, HighlightCount };

    Field(int x, int y, Color color);
    ~Field();

    void init();
    void update(sf::Time time);
    void draw(sf::RenderTarget& renderTarget, sf::RenderStates states) const;


    void setPosition(float x, float y);


    void setHighlight(Highlight highlight);

    Field::Color getColor() const;



    void setCurrentPiece(Piece* piece);
    Piece* getCurrentPiece() const; 

    sf::Vector2i getBoardPostion() const;

private:
    sf::Vector2i boardPosition;
    std::string fieldName;
    Color color;

    sf::ConvexShape fieldShape;
    Highlight highlight;
    sf::ConvexShape frameShape;
    float frameThickness;

    Piece* currentPiece;

    sf::Vector2f size;
};

#pragma once

#include "Field.h"

#include <list>

class Board final : public sf::Drawable, public sf::Transformable
{
public:
    Board(int sideFieldNumber);
    ~Board();

    void init();
    void update(sf::Time time);
    void draw(sf::RenderTarget& renderTarget, sf::RenderStates states) const;


    std::list<Field*> getFieldsByColor(Field::Color color) const;
    Field* getFieldByPosition(const sf::Vector2f& position) const;
    Field* getFieldByBoardPosition(const sf::Vector2i& position) const;

    void clearSelection();

    bool isBoardPositionValid(const sf::Vector2i& position) const;
    bool isBoardPositionInvalid(const sf::Vector2i& position) const;
    bool isBoardPositionOccupied(const sf::Vector2i& position) const;
    sf::Vector2f getSize() const;

private:
    int sideFieldsNumber;
    sf::ConvexShape boardShape;
    Field*** fields;
    sf::IntRect validFields;
    sf::Vector2f size;
};
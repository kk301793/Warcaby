#include "Piece.h"


Piece::Piece(int pieceNumber, Player::Color color) :
    pieceId(pieceNumber),
    pieceType(PieceType::Man),
    pieceName(),
    color(color),
    pieceShape(),
    crownShape(),
    hidden(false),
    xMovingTransition(),
    yMovingTransition(),
    currentField(nullptr)
{
}

Piece::~Piece() {}

void Piece::init()
{
    sf::Color color = sf::Color(224U, 224U, 192U);
    if (this->color == Player::Color::Black) {
        color = sf::Color(68U, 48U, 17U);
    }
    pieceShape.setRadius(0.4f);
    pieceShape.setFillColor(color);

    crownShape.setRadius(0.2f);
    crownShape.setFillColor(sf::Color(207U, 190U, 108U));

    pieceName = std::to_string(pieceId);


}

void Piece::update(sf::Time time)
{
    if (isMovingTransitionRunning()) {
        float x = xMovingTransition.evaluate(time);
        float y = yMovingTransition.evaluate(time);
        setPosition(x, y);
    }
}

void Piece::draw(sf::RenderTarget& renderTarget, sf::RenderStates states) const
{
    if (hidden)
        return;
    renderTarget.draw(pieceShape, states);
    if (pieceType == PieceType::King)
        renderTarget.draw(crownShape, states);
}

void Piece::setPosition(const sf::Vector2f& position)
{
    setPosition(position.x, position.y);
}

void Piece::setPosition(float x, float y)
{
    pieceShape.setPosition(x + (0.5f - pieceShape.getRadius()), y + (0.5f - pieceShape.getRadius()));
    crownShape.setPosition(x + (0.5f - crownShape.getRadius()), y + (0.5f - crownShape.getRadius()));
    Transformable::setPosition(x, y);
}

void Piece::transistToPosition(const sf::Vector2f& position, sf::Time startTime)
{
    transistToPosition(position.x, position.y, startTime);
}

void Piece::transistToPosition(float x, float y, sf::Time startTime)
{
    const sf::Vector2f& startPosition = this->getPosition();

    float startX = startPosition.x;
    xMovingTransition.setMinValue(startX);
    xMovingTransition.setMaxValue(x);
    xMovingTransition.setStartTime(startTime);

    float startY = startPosition.y;
    yMovingTransition.setMinValue(startY);
    yMovingTransition.setMaxValue(y);
    yMovingTransition.setStartTime(startTime);
}

bool Piece::isMovingTransitionRunning() const
{
    return !xMovingTransition.isFinished() || !yMovingTransition.isFinished();
}

Player::Color Piece::getColor() const
{
    return color;
}



void Piece::setCurrentField(Field* field)
{
    currentField = field;
}

Field* Piece::getCurrentField() const
{
    return currentField;
}

Piece::PieceType Piece::getPieceType() const
{
    return pieceType;
}

void Piece::upgrade()
{
    pieceType = Piece::PieceType::King;
}




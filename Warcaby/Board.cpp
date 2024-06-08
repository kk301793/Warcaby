#include "Board.h"


Board::Board(int sideFieldNumber) :
    sideFieldsNumber(sideFieldNumber),
    boardShape(4U),
    fields(),
    validFields(0, 0, sideFieldNumber, sideFieldNumber)
{
    // Tworzenie tablicy pól
    fields = new Field * *[sideFieldNumber];
    for (int y = 0U; y < sideFieldNumber; ++y) {
        fields[y] = new Field * [sideFieldNumber];
        for (int x = 0U; x < sideFieldNumber; ++x) {
            // Ustalanie koloru pola na podstawie wspó³rzêdnych
            Field::Color color = static_cast<Field::Color>((1 + x + y) % 2);
            fields[y][x] = new Field(x, y, color);
        }
    }
}

Board::~Board()
{
    // Usuwanie pól i zwalnianie pamiêci
    for (int y = 0U; y < sideFieldsNumber; ++y) {
        for (int x = 0U; x < sideFieldsNumber; ++x) {
            delete fields[y][x];
        }
        delete[] fields[y];
    }
    delete[] fields;
}

void Board::init()
{
    // Inicjalizacja rozmiaru planszy
    size.x = static_cast<float>(sideFieldsNumber);
    size.y = static_cast<float>(sideFieldsNumber);

    // Ustalanie kszta³tu planszy
    float borderSize = 0.2f;
    boardShape.setPoint(0U, sf::Vector2f(-borderSize, -borderSize));
    boardShape.setPoint(1U, sf::Vector2f(size.x + borderSize, -borderSize));
    boardShape.setPoint(2U, sf::Vector2f(size.x + borderSize, size.y + borderSize));
    boardShape.setPoint(3U, sf::Vector2f(-borderSize, size.y + borderSize));

    // Ustawianie koloru planszy
    boardShape.setFillColor(sf::Color(0U, 0U, 0U));

    for (int y = 0U; y < sideFieldsNumber; ++y) {
        for (int x = 0U; x < sideFieldsNumber; ++x) {
            Field* field = fields[y][x];
            // Inicjalizacja pola
            field->init();
            field->Field::setPosition(static_cast<float>(x), static_cast<float>(sideFieldsNumber - 1 - y));
        }
    }
}

void Board::update(sf::Time time)
{
    // Aktualizowanie pól na planszy
    for (int y = 0U; y < sideFieldsNumber; ++y) {
        for (int x = 0U; x < sideFieldsNumber; ++x) {
            Field* field = fields[y][x];
            field->update(time);
        }
    }
}

void Board::draw(sf::RenderTarget& renderTarget, sf::RenderStates states) const
{
    // Rysowanie planszy
    renderTarget.draw(boardShape);
    for (int y = 0U; y < sideFieldsNumber; ++y) {
        for (int x = 0U; x < sideFieldsNumber; ++x) {
            Field* field = fields[y][x];
            renderTarget.draw(*field);
        }
    }
}

std::list<Field*> Board::getFieldsByColor(Field::Color color) const
{
    std::list<Field*> resultList;

    // Przeszukiwanie wszystkich pól na planszy
    for (int y = 0U; y < sideFieldsNumber; ++y) {
        for (int x = 0U; x < sideFieldsNumber; ++x) {
            Field* field = fields[y][x];

            // Sprawdzanie koloru pola i porównywanie go z podanym kolorem
            if (field->getColor() == color)
                resultList.push_back(field); // Dodawanie pola do listy wynikowej
        }
    }

    return resultList; // Zwracanie listy pól o podanym kolorze
}
Field* Board::getFieldByPosition(const sf::Vector2f& position) const
{
    // Sprawdzanie, czy dana pozycja nale¿y do planszy
    sf::Rect<float> boardRect(0.0f, 0.0f, static_cast<float>(sideFieldsNumber), static_cast<float>(sideFieldsNumber));
    if (!boardRect.contains(position))
        return nullptr;
    // Zwracanie pola na podstawie pozycji
    return fields[sideFieldsNumber - 1 - static_cast<int>(position.y)][static_cast<int>(position.x)];
}

Field* Board::getFieldByBoardPosition(const sf::Vector2i& position) const
{
    // Zwracanie pola na podstawie pozycji planszy
    return fields[position.y][position.x];
}



void Board::clearSelection()
{
    // Czyszczenie zaznaczenia na wszystkich polach
    for (int y = 0U; y < sideFieldsNumber; ++y) {
        for (int x = 0U; x < sideFieldsNumber; ++x) {
            Field* field = fields[y][x];
            field->setHighlight(Field::Highlight::None);
        }
    }
}

bool Board::isBoardPositionValid(const sf::Vector2i& position) const
{
    // Sprawdzanie, czy dana pozycja jest poprawna na planszy
    return validFields.contains(position);
}

bool Board::isBoardPositionInvalid(const sf::Vector2i& position) const
{
    // Sprawdzanie, czy dana pozycja jest niepoprawna na planszy
    return !validFields.contains(position);
}

bool Board::isBoardPositionOccupied(const sf::Vector2i& position) const
{
    // Sprawdzanie, czy dana pozycja jest zajêta na planszy
    return fields[position.y][position.x]->getCurrentPiece() != nullptr;
}

sf::Vector2f Board::getSize() const
{
    // Zwracanie rozmiaru planszy
    return size;
}

#include "Draughts.h"
#include "Piece.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <unordered_map>



const sf::Vector2i Draughts::AllOffsets[Draughts::AllOffsetsNumber] = {
    sf::Vector2i(-1, 1), sf::Vector2i(1, 1), sf::Vector2i(1, -1), sf::Vector2i(-1, -1)
};

Draughts::Draughts() :
    renderWindow(),
    gameplayView(),
    viewLeftTopCorner(),
    fontScaleFactor(),
    gameplayTexture(),
    gameStarted(false),
    gameOver(false),
    isFinished_(false),
    sideFieldsNumber(8U),
    board(std::make_unique<Board>(sideFieldsNumber)),
    player1(std::make_unique<Player>(Player::Color::White, board.get())),
    player2(std::make_unique<Player>(Player::Color::Black, board.get())),
    winner(),
    playerPiecesRowsNumber(3U),
    piecesNumber(playerPiecesRowsNumber * 2 * sideFieldsNumber / 2),
    pieces(),
    currentPlayer(nullptr),
    waitingForInteraction(false),
    selectedPiece(),
    currentTurn(),
    possibleTurns(),
    narrowedTurns(),
    captures(),
    gameState(GameState::Login),
    startButton(sf::Vector2f(100.f, 50.f)),
    nameBox(sf::Vector2f(400.f, 50.f)),
    nameBox2(sf::Vector2f(400.f, 50.f)),
    nextButton(sf::Vector2f(100.f, 50)),
    changeButton(sf::Vector2f(100.f, 50.f)),
    giveMove(sf::Vector2f(100.f, 50.f)),
    current_player(&NAME_player1),
    errorLogin(false),
    gameKind(GameKind::None),
    closeButton(sf::Vector2f(100.f, 50.f)),
    changing(false),
    current__player(new std::string)

{
    font.loadFromFile("res/cour.ttf");
    changeButton.setOutlineColor(sf::Color::Black);
    changeButton.setOutlineThickness(3);


    closeButton.setOutlineColor(sf::Color::Black);
    closeButton.setOutlineThickness(3);

    nextText.setFont(font);
    closeText.setFont(font);
    changeText.setFont(font);


    time.setFont(font);
    time.setPosition(600.f, 100.f);
    time.setFillColor(sf::Color::Black);

    error.setFont(font);
    error.setFillColor(sf::Color(128, 0, 0));
    error.setString(L"Uwaga: Imiona mog¹ sk³adaæ siê wy³¹cznie z liter, pierwsza musi byæ wielka [max 10 znakow]!");
    error.setPosition(70.f, 150.f);
    error.setCharacterSize(26);

    nameBox.setFillColor(sf::Color(0, 0, 0, 0));
    nameBox2.setFillColor(sf::Color(0, 0, 0, 0));

    nameBox.setOutlineColor(sf::Color::Black);
    nameBox.setOutlineThickness(3);
    nameBox2.setOutlineColor(sf::Color::Black);
    nameBox2.setOutlineThickness(3);

    winner_.setFont(font);
    winner_.setCharacterSize(60U);
    winner_.setRotation(35.f);
    winner_.setPosition(renderWindow.getSize().x + 200.f, renderWindow.getSize().y);


    white.setFont(font);
    black.setFont(font);
    white.setString("Bialy");
    black.setString("Czarny");
    startText.setFont(font);
    NAME_player1.setFont(font);
    NAME_player1.setCharacterSize(30);
    NAME_player2.setCharacterSize(30);
    NAME_player2.setFont(font);


    NAME_player1.setFillColor(sf::Color::Black);
    NAME_player2.setFillColor(sf::Color::Black);

    per_Game.setSize(sf::Vector2f(200.f, 300.f));
    per_Move.setSize(sf::Vector2f(200.f, 300.f));
    per_Game.setOutlineThickness(3);
    per_Move.setOutlineThickness(3);
    per_Game.setOutlineColor(sf::Color::Black);
    per_Move.setOutlineColor(sf::Color::Black);
    per_Game.setPosition(250.f, 100.f);
    per_Move.setPosition(490.f, 100.f);
    per_Move.setFillColor(sf::Color(0, 0, 0, 0));
    per_Game.setFillColor(sf::Color(0, 0, 0, 0));

}

Draughts::~Draughts() {}

void Draughts::init()
{
    sf::VideoMode videoMode = sf::VideoMode::getFullscreenModes()[0];

    videoMode = sf::VideoMode::getDesktopMode();

    //regulacja wielkosci okna gry 
    videoMode.width = static_cast<unsigned int>(videoMode.width * 0.8f);
    videoMode.height = static_cast<unsigned int>(videoMode.height * 0.8f);


    // Ustawienie ustawieñ renderowania okna
    renderWindow.setFramerateLimit(60U);
    renderWindow.setVerticalSyncEnabled(true);
    renderWindow.setKeyRepeatEnabled(true);


    gameplayTexture.create(videoMode.width, videoMode.height);
    renderWindow.create(videoMode, L"Warcaby", sf::Style::Default);

    gameplayTexture.setView(gameplayView);
    prepareGame();


}

void Draughts::update(sf::Time time)
{
    // Aktualizacja czasu gry
    gameTime = time;

    // Pobranie zdarzeñ
    retrieveEvents();

    // Aktualizacja planszy
    board->update(time);

    // Aktualizacja po³o¿enia pionkow
    bool pieceMoving = false;
    for (const std::unique_ptr<Piece>& piece : pieces) {
        piece->update(time);
        if (piece->isMovingTransitionRunning())
            pieceMoving = true;
    }


    if (gameState == GameState::Gameplay) {
        // Aktualizacja dla stanu rozgrywki

        // Sprawdzenie limitu czasu na ruch
        if (gameKind == GameKind::Limited_time_per_move) {
            if (moveTimer.getElapsedTime().asSeconds() >= 10) {
                changeTurn();
                std::cout << "Nast¹pi³a zmiana tur, brak ruchu w dozwolonym czasie\n";
            }
        }
        else if (gameKind == GameKind::Limited_time_per_game) {
            // Sprawdzenie limitu czasu na grê

            if (currentPlayer == player1.get()) {
                if (player1TotalTime.asSeconds() + moveTimer.getElapsedTime().asSeconds() > 120.f) {
                    changeTurn(false);
                    playerWon();
                }
                else if (player2TotalTime.asSeconds() > 120.f) {
                    changeTurn();
                    playerWon();
                }
            }
            else if (currentPlayer == player2.get()) {
                if (player2TotalTime.asSeconds() + moveTimer.getElapsedTime().asSeconds() > 120.f) {
                    changeTurn(false);
                    playerWon();
                }
                else if (player1TotalTime.asSeconds() > 120.f) {
                    changeTurn();
                    playerWon();
                }
            }

            // Ka¿dy z graczy ma 2 minuty na grê, jeœli czas któregoœ z nich minie, to drugi wygrywa
        }

        // Wybór i wykonanie ruchu
        if (selectedPiece == nullptr) {
            selectPiece();
            narrowTurns();
        }
        else {
            makeStep();
        }

        board->clearSelection();

        highlightPossibleSteps();
    }
}

void Draughts::render()
{
    gameplayTexture.clear(sf::Color(159U, 117U, 69U));   //kolor g³ównej tekstury

    winner_.setFillColor(sf::Color(0, 0, 0, 0));

    if (gameState == GameState::Gameplay || gameState == GameState::Menu)
    {
        gameplayTexture.draw(*board);

        //nazwy graczy
        NAME_player1.setPosition(renderWindow.getSize().x / 2.f - 50.f, 5.f);
        NAME_player2.setPosition(renderWindow.getSize().x / 2.f - 50.f, renderWindow.getSize().y - 50.f);



        startButton.setOutlineThickness(3);
        startButton.setOutlineColor(sf::Color::Black);
        startButton.setFillColor(sf::Color::Transparent);

        // wszystkie pionki, które siê nie ruszaj¹ i nie s¹ zbite
        for (const std::unique_ptr<Piece>& piece : pieces)
        {
            //pionek jest zbity
            if (piece->getCurrentField() == nullptr)
                continue;
            gameplayTexture.draw(*piece);
        }

        if (gameState == GameState::Menu)
        {

            //sf::RectangleShape startButton();  // Tworzenie prostok¹ta jako przycisku
            startButton.setPosition(70.f, 70.f); // Ustawienie pozycji przycisku
            nextButton.setPosition(70.f, 145.f);
            changeButton.setPosition(70.f, 220.f);
            closeButton.setPosition(70.f, 295.f);


            startButton.setFillColor(sf::Color(0, 0, 0, 0)); // Ustawienie koloru przycisku
            nextButton.setFillColor(sf::Color(0, 0, 0, 0)); // Ustawienie koloru przycisku
            changeButton.setFillColor(sf::Color(0, 0, 0, 0)); // Ustawienie koloru przycisku
            closeButton.setFillColor(sf::Color(0, 0, 0, 0)); // Ustawienie koloru przycisku

            startText.setCharacterSize(20U);
            startText.setPosition(80.f, 80.f);
            if (true)
                startText.setString("Start");
            else
                startText.setString(L"Ponów");

            startText.setFillColor(sf::Color::Black);



        }

    }


    renderWindow.clear(sf::Color(159U, 117U, 69U));

    gameplayTexture.display();
    sf::Sprite gameplaySprite(gameplayTexture.getTexture());
    renderWindow.draw(gameplaySprite);

    renderWindow.draw(NAME_player1);
    renderWindow.draw(NAME_player2);



    if (gameState == GameState::Login)
    {
        //wyswietlanie strony do logowania

        sf::Text text("\tWarcaby !\n\nPodaj imiona graczy:", font);
        text.setFillColor(sf::Color::Black);
        sf::FloatRect textBounds = text.getGlobalBounds();
        sf::Vector2u windowSize = renderWindow.getSize();
        float x = (windowSize.x - textBounds.width) / 2.f;
        float y = 20.f;
        text.setPosition(x, y);
        y = (windowSize.y - textBounds.height) / 2.f;
        black.setPosition(x, y - 40);
        nameBox.setPosition(x, y);
        NAME_player1.setPosition(x, y);
        white.setPosition(x, y + 60);
        nameBox2.setPosition(x, y + 100);
        NAME_player2.setPosition(x, y + 100);

        nextButton.setFillColor(sf::Color(0, 0, 0, 0)); // Ustawienie przezroczystego koloru
        nextButton.setPosition(renderWindow.getSize().x - 150.f, renderWindow.getSize().y - 100.f);
        nextButton.setOutlineThickness(3);
        nextButton.setOutlineColor(sf::Color::Black);

        startText.setPosition(renderWindow.getSize().x - 135.f, renderWindow.getSize().y - 90.f);
        startText.setCharacterSize(20);
        startText.setString("Dalej");
        startText.setStyle(sf::Text::Bold);
        startText.setFillColor(sf::Color::Black);

        renderWindow.draw(startText);
        renderWindow.draw(nextButton);
        renderWindow.draw(text);
        renderWindow.draw(nameBox);
        renderWindow.draw(nameBox2);
        renderWindow.draw(NAME_player1);
        renderWindow.draw(NAME_player2);
        renderWindow.draw(white);
        renderWindow.draw(black);
        if (errorLogin)
            renderWindow.draw(error);
    }

    else if (gameState == GameState::Choice)
    {

        //przyciski ktore mozna wykorzystac next button

        sf::Text text("\tWybierz zasady gry!", font);
        text.setFillColor(sf::Color::Black);
        sf::FloatRect textBounds = text.getGlobalBounds();
        sf::Vector2u windowSize = renderWindow.getSize();
        float x = (windowSize.x - textBounds.width - 50.f) / 2.f;;
        float y = 20.f;
        text.setPosition(x, y);


        nextButton.setFillColor(sf::Color(0, 0, 0, 0)); // Ustawienie przezroczystego koloru
        nextButton.setPosition(renderWindow.getSize().x - 150.f, renderWindow.getSize().y - 100.f);
        nextButton.setOutlineThickness(3);
        nextButton.setOutlineColor(sf::Color::Black);

        startText.setPosition(renderWindow.getSize().x - 135.f, renderWindow.getSize().y - 90.f);
        startText.setCharacterSize(20);
        startText.setString("Dalej");
        startText.setStyle(sf::Text::Bold);
        startText.setFillColor(sf::Color::Black);


        sf::Text choice1(L"Czas na ruch\nwynosi 10 sekund!", font);
        sf::Text choice2(L"Czas na grê\nwynosi 2 minuty\ndla ka¿dego z graczy!", font);
        choice1.setFillColor(sf::Color::Black);
        choice2.setFillColor(sf::Color::Black);
        choice1.setCharacterSize(15);
        choice2.setCharacterSize(15);

        choice2.setPosition(250.f, 100.f);
        choice1.setPosition(490.f, 100.f);




        NAME_player1.setPosition(-1000.f, -1000.f);
        NAME_player2.setPosition(-1000.f, -1000.f);


        renderWindow.draw(per_Game);
        renderWindow.draw(per_Move);
        renderWindow.draw(nextButton);
        renderWindow.draw(startText);

        renderWindow.draw(text);
        renderWindow.draw(choice1);
        renderWindow.draw(choice2);
    }

    else if (gameState == GameState::Menu)
    {
        if (gameOver)
        {
            winner_.setFillColor(sf::Color::Black);

        }

        nextText.setString(L"Zmieñ\ngraczy");
        changeText.setString(L"Zmieñ\ntryb");
        closeText.setString(L"Zamknij");

        nextText.setFillColor(sf::Color::Black);
        changeText.setFillColor(sf::Color::Black);
        closeText.setFillColor(sf::Color::Black);

        nextText.setCharacterSize(20U);
        changeText.setCharacterSize(20U);
        closeText.setCharacterSize(20U);

        nextText.setStyle(sf::Text::Bold);
        changeText.setStyle(sf::Text::Bold);
        closeText.setStyle(sf::Text::Bold);

        nextText.setPosition(nextButton.getPosition().x + 10.f, nextButton.getPosition().y);
        changeText.setPosition(changeButton.getPosition().x + 10.f, changeButton.getPosition().y);
        closeText.setPosition(closeButton.getPosition().x + 10.f, closeButton.getPosition().y);



        renderWindow.draw(closeButton);
        renderWindow.draw(changeButton);
        renderWindow.draw(nextButton);
        renderWindow.draw(startButton);
        renderWindow.draw(startText);
        renderWindow.draw(closeText);
        renderWindow.draw(changeText);
        renderWindow.draw(nextText);
    }



    else if (gameKind == GameKind::Limited_time_per_move)
    {
        // Wyœwietlanie pozosta³ego czasu na ruch

        // Tworzenie strumienia i formatowanie czasu
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(2) << 10. - moveTimer.getElapsedTime().asSeconds();

        // Ustawianie tekstu z informacj¹ o pozosta³ym czasie
        time.setString("Do koñca tury: " + stream.str());
        time.setPosition(1, 1);

        // Zmiana koloru nazw graczy na podstawie aktualnego gracza
        if (currentPlayer == player1.get())
        {
            NAME_player2.setFillColor(sf::Color(90, 239, 60));
            NAME_player1.setFillColor(sf::Color::Black);
        }
        else if (currentPlayer == player2.get())
        {
            NAME_player2.setFillColor(sf::Color::Black);
            NAME_player1.setFillColor(sf::Color(90, 239, 60));
        }

        // Rysowanie informacji o czasie
        renderWindow.draw(time);
    }
    else if (gameKind == GameKind::Limited_time_per_game)
    {
        // Oblicz czas wykorzystany przez graczy
        //sf::Time player1TimeUsed = player1TotalTime + (player1.get() == currentPlayer ? currentPlayer->getClock().getElapsedTime() : sf::Time::Zero);
        //sf::Time player2TimeUsed = player2TotalTime + (player2.get() == currentPlayer ? currentPlayer->getClock().getElapsedTime() : sf::Time::Zero);

        // Wyœwietl czas wykorzystany przez graczy
        std::ostringstream streamPlayer1, streamPlayer2;
        if (currentPlayer == player1.get())
            streamPlayer1 << std::fixed << std::setprecision(2) << 120. - player1TotalTime.asSeconds() - moveTimer.getElapsedTime().asSeconds();
        else
            streamPlayer1 << std::fixed << std::setprecision(2) << 120. - player1TotalTime.asSeconds();
        if (currentPlayer == player2.get())
            streamPlayer2 << std::fixed << std::setprecision(2) << 120. - player2TotalTime.asSeconds() - moveTimer.getElapsedTime().asSeconds();
        else
            streamPlayer2 << std::fixed << std::setprecision(2) << 120. - player2TotalTime.asSeconds();
        
        std::string player1TimeString = "Czas " + name_player1 + ": " + streamPlayer2.str();
        std::string player2TimeString = "Czas " + name_player2 + ": " + streamPlayer1.str();



        // Umieœæ tekst w odpowiednich pozycjach
        sf::Text player1TimeText(player1TimeString, font, 20);
        sf::Text player2TimeText(player2TimeString, font, 20);
        player1TimeText.setStyle(sf::Text::Bold);
        player2TimeText.setStyle(sf::Text::Bold);
        player1TimeText.setPosition(10.f, 10.f);
        player2TimeText.setPosition(10.f, renderWindow.getSize().y - 40.f);
        player1TimeText.setFillColor(sf::Color::Black);
        player2TimeText.setFillColor(sf::Color::Black);

        // Wyœwietl tekst
        renderWindow.draw(player1TimeText);
        renderWindow.draw(player2TimeText);
    }

    if (gameState == GameState::Gameplay)
    {
        // Rysowanie przycisków

        // Ustawienie koloru obramowania przycisków na czarny
        changeButton.setOutlineColor(sf::Color::Black);
        nextButton.setOutlineColor(sf::Color::Black);
        startButton.setOutlineColor(sf::Color::Black);
        giveMove.setOutlineColor(sf::Color::Black);

        // Ustawienie gruboœci obramowania przycisków na 3 piksele
        changeButton.setOutlineThickness(3);
        nextButton.setOutlineThickness(3);
        startButton.setOutlineThickness(3);
        giveMove.setOutlineThickness(3);

        // Ustawienie koloru wype³nienia przycisków na przezroczysty
        giveMove.setFillColor(sf::Color(0, 0, 0, 0));
        changeButton.setFillColor(sf::Color(0, 0, 0, 0));

        // Ustalenie pozycji przycisków na podstawie rozmiaru okna renderowania
        changeButton.setPosition(renderWindow.getSize().x - 175.f, renderWindow.getSize().y - 325.f);
        nextButton.setPosition(renderWindow.getSize().x - 175.f, renderWindow.getSize().y - 250.f);
        startButton.setPosition(renderWindow.getSize().x - 175.f, renderWindow.getSize().y - 100.f);
        giveMove.setPosition(renderWindow.getSize().x - 175.f, renderWindow.getSize().y - 175.f);

        // Utworzenie tekstu przycisków
        sf::Text buttonText1(L"Poddaj siê", font, 13);
        sf::Text buttonText2(L"Odpuœæ turê", font, 13);
        sf::Text buttonText3(L"WyjdŸ", font, 13);
        sf::Text buttonText4(L"Zmieñ pionek", font, 13);

        // Ustawienie koloru tekstu przycisków na czarny
        buttonText1.setFillColor(sf::Color::Black);
        buttonText2.setFillColor(sf::Color::Black);
        buttonText3.setFillColor(sf::Color::Black);
        buttonText4.setFillColor(sf::Color::Black);

        // Ustalenie pozycji tekstu przycisków
        buttonText3.setPosition(renderWindow.getSize().x - 175.f, renderWindow.getSize().y - 85.f);
        buttonText2.setPosition(renderWindow.getSize().x - 175.f, renderWindow.getSize().y - 160.f);
        buttonText1.setPosition(renderWindow.getSize().x - 175.f, renderWindow.getSize().y - 235.f);
        buttonText4.setPosition(renderWindow.getSize().x - 175.f, renderWindow.getSize().y - 310.f);

        // Rysowanie tekstu przycisków
        renderWindow.draw(buttonText1);
        renderWindow.draw(buttonText2);
        renderWindow.draw(buttonText3);
        renderWindow.draw(buttonText4);

        // Rysowanie przycisków
        renderWindow.draw(changeButton);
        renderWindow.draw(nextButton);
        renderWindow.draw(startButton);
        renderWindow.draw(giveMove);
    }

    // Rysowanie informacji o zwyciêzcy
    renderWindow.draw(winner_);

    // Wyœwietlanie renderowanej zawartoœci
    renderWindow.display();

}

bool Draughts::shouldFinish() const
{
    return isFinished_;
}

void Draughts::finish()
{
    renderWindow.close();
}


void Draughts::recreateBoard()
{
    board = std::make_unique<Board>(sideFieldsNumber);
    board->init();
    board->setPosition(0.0f, 0.0f);
}

void Draughts::recreatePieces()
{
    // Wyczyszczenie wektora pionków
    pieces.clear();
    // Zmiana rozmiaru wektora pionków na podstawie liczby pionków
    pieces.resize(piecesNumber);

    // Tworzenie pionków i przypisanie im odpowiednich kolorów
    for (int i = 0U; i < piecesNumber; ++i) {
        Player::Color color = Player::Color::White;
        if (i >= piecesNumber / 2)
            color = Player::Color::Black;
        pieces[i] = std::make_unique<Piece>(i, color);
    }

    // Inicjalizacja pionków i ustawienie marginesu pól
    for (const std::unique_ptr<Piece>& piece : pieces) {
        piece->init();

    }
}


void Draughts::prepareGame()
{
    // Resetowanie zmiennych i obiektów zwi¹zanych z rozgrywk¹
    gameStarted = false;
    gameOver = false;
    winner = nullptr;
    captures.clear();
    selectedPiece = nullptr;
    currentTurn.clear();
    narrowedTurns = nullptr;
    clearPossibleTurns();


    // Obliczanie liczby pionków na podstawie ustawieñ planszy
    piecesNumber = playerPiecesRowsNumber * 2 * sideFieldsNumber / 2;

    // Tworzenie planszy i pionków
    recreateBoard();
    recreatePieces();

    // Dopasowywanie widoku rozgrywki do rozmiaru okna
    const sf::Vector2u& renderWindowSize = renderWindow.getSize();
    float gameplayViewHeight = static_cast<float>(sideFieldsNumber) + 2.0f;
    fontScaleFactor = gameplayViewHeight / renderWindowSize.y;

    gameplayView.setCenter((gameplayViewHeight - 2.0f) / 2.0f, (gameplayViewHeight - 2.0f) / 2.0f);
    gameplayView.setSize(fontScaleFactor * renderWindowSize.x, gameplayViewHeight);
    gameplayView.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));
    viewLeftTopCorner.x = gameplayView.getCenter().x - gameplayView.getSize().x / 2.0f;
    viewLeftTopCorner.y = gameplayView.getCenter().y - gameplayView.getSize().y / 2.0f;
    gameplayTexture.setView(gameplayView);

    // Ustawienie gracza rozpoczynaj¹cego
    currentPlayer = player1.get();

    // Ustalenie pozycji pionków na planszy
    setPiecesPosition();

    // Generowanie mo¿liwych ruchów
    addPossibleTurns();



}


void Draughts::startGame()
{
    gameState = GameState::Gameplay;

    gameStarted = true;
}



void Draughts::setPiecesPosition()
{
    // Pobierz listê czarnych pól na planszy
    std::list<Field*> blackFields = board->getFieldsByColor(Field::Color::Black);

    // Przypisz pozycje pionków
    for (const std::unique_ptr<Piece>& piece : pieces)
    {
        Field* field;

        // Jeœli pionek jest bia³y, pobierz pierwsze czarne pole z listy
        if (piece->getColor() == Player::Color::White)
        {
            field = blackFields.front();
            blackFields.pop_front();
        }
        // Jeœli pionek jest czarny, pobierz ostatnie czarne pole z listy
        else
        {
            field = blackFields.back();
            blackFields.pop_back();
        }

        // Ustaw bie¿¹ce pole dla pionka i pionek dla pola
        piece->setCurrentField(field);
        field->setCurrentPiece(piece.get());

        // Ustaw pozycjê pionka na pozycjê pola
        piece->setPosition(field->getPosition());
    }
}

void Draughts::retrieveEvents()
{
    sf::Event event;
    while (renderWindow.pollEvent(event))
    {
        if (event.type == sf::Event::EventType::Closed)
        {

            isFinished_ = true;
            continue;
        }

        if (gameState == GameState::Menu)
        {
            // Obs³uga zdarzeñ w menu
            if (event.type == sf::Event::EventType::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                // Sprawdzanie, czy naciœniêcie nast¹pi³o na przycisku "Start"
                if (onClickButton(startButton))
                {

                    countPlayerStats();

                    NAME_player1.setString(name_player1 + " Win:" + std::to_string(player1->getWin()) + " Lose:" + std::to_string(player1->getLose()));
                    NAME_player2.setString(name_player2 + " Win:" + std::to_string(player2->getWin()) + " Lose:" + std::to_string(player2->getLose()));
                    gameState = GameState::Gameplay;
                    prepareGame();
                    moveTimer.restart();
                    player1TotalTime = sf::Time::Zero;
                    player2TotalTime = sf::Time::Zero;
                }
                else if (onClickButton(nextButton))
                {
                    gameState = GameState::Login;
                    NAME_player1.setString(name_player1);
                    NAME_player2.setString(name_player2);
                    changing = true;
                }
                else if (onClickButton(changeButton))
                {
                    gameState = GameState::Choice;
                }
                else if (onClickButton(closeButton))
                {

                    isFinished_ = true;
                    renderWindow.close();
                }
            }
        }
        else if (gameState == GameState::Gameplay)
        {


            if (event.type == sf::Event::EventType::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2f pos = gameplayTexture.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                    mouseClickedOnBoard(pos);

                    sf::Vector2i mousePos = sf::Mouse::getPosition(renderWindow);
                    if (nextButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                    {
                        // Poddanie siê
                        changeTurn(false);
                        playerWon();
                    }
                    else if (startButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                    {
                        //zakonczenie rozgrywki
                        changeTurn(false);
                        gameState = GameState::Menu;
                    }
                    else if (giveMove.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                    {
                        // Oddanie ruchu
                        changeTurn();
                    }
                    else if (changeButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                    {
                        //ponowne wybranie pionka
                        changeTurn(false);
                    }
                }
            }
        }
        else if (gameState == GameState::Login)
        {
            // Obs³uga zdarzeñ podczas logowania

            // Obs³uga wprowadzania tekstu
            if (event.type == sf::Event::TextEntered && current_player != nullptr)
            {
                if (event.text.unicode == '\b' && !current_player->getString().isEmpty())
                {
                    // Usuniêcie ostatniego znaku
                    std::string currentText = current_player->getString();
                    if (!currentText.empty())
                    {
                        currentText.pop_back();
                        current_player->setString(currentText);
                        *current__player = currentText;
                    }
                }
                else if (event.text.unicode == '\r' || event.text.unicode == '\n')
                {
                    // Przejœcie do drugiego imienia po wciœniêciu Enter
                    if (current_player == &NAME_player1)
                    {
                        current_player = &NAME_player2;
                        current__player = &name_player2;
                    }
                    else if (current_player == &NAME_player2)
                    {
                        current_player = &NAME_player1;
                        current__player = &name_player1;
                    }
                }
                else if (event.text.unicode < 128)
                {
                    // Dodanie znaku do aktualnego imienia
                    std::string currentText = current_player->getString();
                    currentText += static_cast<char>(event.text.unicode);
                    current_player->setString(currentText);
                    current__player->assign(currentText);


                }
            }
            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(renderWindow);
                if (nameBox.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                {
                    current_player = &NAME_player1;
                    current__player = &name_player1;
                }
                else if (nameBox2.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                {
                    current_player = &NAME_player2;
                    current__player = &name_player2;
                }
                else if (nextButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                {
                    if (validateName(NAME_player1.getString().toAnsiString()) && validateName(NAME_player2.getString().toAnsiString()) && !changing)
                    {
                        gameState = GameState::Choice;
                    }
                    else if (validateName(NAME_player1.getString().toAnsiString()) && validateName(NAME_player2.getString().toAnsiString()) && changing)
                    {

                        gameState = GameState::Menu;
                    }
                    else if (!validateName(NAME_player1.getString().toAnsiString()))
                    {
                        errorLogin = true;
                        std::cout << "Imie gracza 1 jest nie poprawne\n";
                    }
                    if (!validateName(NAME_player2.getString().toAnsiString()))
                    {
                        errorLogin = true;
                        std::cout << "Imie gracza 2 jest nie poprawne\n";
                    }
                }
            }
        }
        else if (gameState == GameState::Choice)
        {
            // Obs³uga zdarzeñ w menu wyboru

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(renderWindow);
                if (nextButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                {
                    if (gameKind != GameKind::None)
                        gameState = GameState::Menu;
                }
                else if (per_Game.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                {
                    per_Game.setFillColor(sf::Color(46, 179, 20));
                    per_Move.setFillColor(sf::Color(0, 0, 0, 0));
                    gameKind = GameKind::Limited_time_per_game;
                }
                else if (per_Move.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                {
                    per_Game.setFillColor(sf::Color(0, 0, 0, 0));
                    per_Move.setFillColor(sf::Color(46, 179, 20));
                    gameKind = GameKind::Limited_time_per_move;
                }
            }
        }
    }
}


void Draughts::mouseClickedOnBoard(const sf::Vector2f& pressPoint)
{
    // Pobierz pole, na które naciœniêto myszk¹
    Field* pressedField = board->getFieldByPosition(pressPoint);

    // Jeœli nie znaleziono pola, zakoñcz funkcjê
    if (pressedField == nullptr)
        return;


    // SprawdŸ, czy istnieje mo¿liwy ruch dla wybranej figury
    bool isPossibleTurn = false;
    for (const auto& pair : possibleTurns)
    {
        if (pair.first == selectedPiece)
        {
            if (pair.second->isValid)
            {
                isPossibleTurn = true;
                break;
            }
        }
    }

    // Wywo³aj metodê fieldClicked() aktualnego gracza, przekazuj¹c wciœniête pole
    currentPlayer->fieldClicked(pressedField);
}






void Draughts::addPossibleTurns()
{
    // Dla ka¿dej figury gracza
    for (const std::unique_ptr<Piece>& piece : pieces) {
        // SprawdŸ, czy kolor figury odpowiada obecnemu graczowi
        if (currentPlayer->getColor() != piece->getColor())
            continue;
        // Pobierz pole, na którym znajduje siê figura
        Field* field = piece->getCurrentField();
        // Jeœli figura nie znajduje siê na ¿adnym polu, przejdŸ do kolejnej figury
        if (field == nullptr)
            continue;
        // Utwórz mo¿liwy pusty ruch dla figury
        createPossibleEmptyTurn(piece.get());
        // Dodaj mo¿liwe ruchy figury
        addPossiblePieceMoves(piece.get());
        // Dodaj mo¿liwe bicia figury
        addPossiblePieceJumps(piece.get());
    }
    // Zweryfikuj mo¿liwe ruchy
    validatePossibleTurns();
    // Zresetuj zawê¿one mo¿liwe ruchy
    narrowedTurns = nullptr;
    // Jeœli istnieje mo¿liwy ruch, zakoñcz funkcjê
    if (isPossibleTurn())
        return;
    // W przeciwnym razie gracz wygra³
    playerWon();
    // Wyczyœæ zaznaczenie planszy
    board->clearSelection();
    return;
}

void Draughts::clearPossibleTurns()
{
    possibleTurns.clear();
}

void Draughts::createPossibleEmptyTurn(Piece* piece)
{
    possibleTurns[piece] = std::make_unique<StepTree>();
    possibleTurns[piece]->field = piece->getCurrentField();
}

void Draughts::addPossiblePieceMoves(Piece* piece)
{
    // Pobierz pole, na którym znajduje siê figura
    Field* field = piece->getCurrentField();
    // Pobierz pozycjê na planszy figury
    sf::Vector2i boardPosition = field->getBoardPostion();
    // Pobierz typ figury
    Piece::PieceType pieceType = piece->getPieceType();
    // Pobierz kolor figury
    Player::Color color = piece->getColor();
    // Okreœl kierunek ruchu do przodu w zale¿noœci od koloru figury
    int forwardDirection = 1;
    if (color == Player::Color::Black)
        forwardDirection = -1;
    // Pobierz poprzedni krok w drzewie kroków dla figury
    StepTree* previousStep = possibleTurns[piece].get();

    // Jeœli figura jest typu "Man"
    if (pieceType == Piece::PieceType::Man)
    {
        // Mo¿liwe przesuniêcia dla figury typu "Man"
        const size_t forwardMovesOffsetsNumber = 2U;
        const sf::Vector2i forwardMovesOffsets[forwardMovesOffsetsNumber] = { sf::Vector2i(-1, forwardDirection),
                                                                              sf::Vector2i(1, forwardDirection) };
        std::list<sf::Vector2i> forwardMoves;

        // SprawdŸ mo¿liwe przesuniêcia w przód
        for (const sf::Vector2i& offset : forwardMovesOffsets)
        {
            sf::Vector2i targetBoardPosition(boardPosition + offset);
            if (board->isBoardPositionInvalid(targetBoardPosition))
                continue;
            if (board->isBoardPositionOccupied(targetBoardPosition))
                continue;
            Field* targetField = board->getFieldByBoardPosition(targetBoardPosition);
            StepTree* targetStep = new StepTree();
            targetStep->parent = previousStep;
            targetStep->field = targetField;
            previousStep->nextStepList.push_back(targetStep);
        }
    }
    // Jeœli figura jest typu "King"
    else if (pieceType == Piece::PieceType::King)
    {
        std::list<sf::Vector2i> allDirectionMoves;

        // SprawdŸ mo¿liwe przesuniêcia we wszystkich kierunkach
        for (const sf::Vector2i& directionOffset : AllOffsets)
        {
            for (int i = 1; i <= sideFieldsNumber; ++i)
            {
                sf::Vector2i targetBoardPosition(boardPosition + i * directionOffset);
                if (board->isBoardPositionInvalid(targetBoardPosition))
                    break;
                if (board->isBoardPositionOccupied(targetBoardPosition))
                    break;
                allDirectionMoves.push_back(targetBoardPosition);
            }
        }

        // Dodaj mo¿liwe przesuniêcia dla figury typu "King"
        for (const sf::Vector2i& move : allDirectionMoves)
        {
            if (board->isBoardPositionOccupied(move))
                continue;
            Field* targetField = board->getFieldByBoardPosition(move);
            StepTree* targetStep = new StepTree();
            targetStep->parent = previousStep;
            targetStep->field = targetField;
            previousStep->nextStepList.push_back(targetStep);
        }
    }
}


void Draughts::addPossiblePieceJumps(Piece* piece)
{
    // Pobierz pole, na którym znajduje siê figura
    Field* field = piece->getCurrentField();
    // Pobierz typ figury
    Piece::PieceType pieceType = piece->getPieceType();
    // Pobierz drzewo kroków dla figury
    StepTree* stepTree = possibleTurns[piece].get();
    // Zbiór przechowuj¹cy figury, które zosta³y ju¿ zdobyte w bie¿¹cym ruchu
    std::set<Piece*> captures;

    // Jeœli figura jest typu "Man"
    if (pieceType == Piece::PieceType::Man)
    {
        // Dodaj mo¿liwe skoki dla figury typu "Man"
        addPossibleManJumps(stepTree, captures, piece);
    }
    // Jeœli figura jest typu "King"
    else if (pieceType == Piece::PieceType::King)
    {
        // Dodaj mo¿liwe skoki dla figury typu "King"
        addPossibleKingJumps(stepTree, captures, piece);
    }
}

void Draughts::addPossibleManJumps(StepTree* currentStep, std::set<Piece*>& currentCaptures, Piece* startPiece)
{
    // Pobierz kolor pocz¹tkowej figury
    Player::Color startColor = startPiece->getColor();
    // Pobierz pole pocz¹tkowe figury
    Field* startField = startPiece->getCurrentField();
    // Pobierz ostatni¹ pozycjê na planszy dla bie¿¹cego kroku
    sf::Vector2i lastBoardPosition = currentStep->field->getBoardPostion();
    // Pobierz pozycjê pocz¹tkow¹ na planszy dla figury
    sf::Vector2i startBoardPosition = startField->getBoardPostion();

    // Iteruj przez wszystkie dostêpne przesuniêcia
    for (const sf::Vector2i& offset : AllOffsets) {
        // Oblicz pozycjê ataku
        sf::Vector2i attackPosition = lastBoardPosition + offset;

        // Jeœli pozycja ataku jest nieprawid³owa, kontynuuj do nastêpnego przesuniêcia
        if (board->isBoardPositionInvalid(attackPosition))
            continue;

        // Jeœli pozycja ataku jest wolna, kontynuuj do nastêpnego przesuniêcia
        if (!board->isBoardPositionOccupied(attackPosition))
            continue;

        // Pobierz pole ataku
        Field* attackField = board->getFieldByBoardPosition(attackPosition);
        // Pobierz figurê ataku
        Piece* attackPiece = attackField->getCurrentPiece();

        // Jeœli figura ataku jest ju¿ uwzglêdniona w bie¿¹cych zdobyczach, kontynuuj do nastêpnego przesuniêcia
        if (currentCaptures.find(attackPiece) != currentCaptures.end())
            continue;

        // Pobierz kolor figury ataku
        Player::Color attackColor = attackPiece->getColor();

        // Jeœli kolor figury ataku jest taki sam jak kolor pocz¹tkowej figury, kontynuuj do nastêpnego przesuniêcia
        if (attackColor == startColor)
            continue;

        // Oblicz pozycjê celu
        sf::Vector2i targetPosition = lastBoardPosition + 2 * offset;

        // Jeœli pozycja celu jest nieprawid³owa, kontynuuj do nastêpnego przesuniêcia
        if (board->isBoardPositionInvalid(targetPosition))
            continue;

        // Jeœli pozycja celu nie jest równa pozycji pocz¹tkowej i jest zajêta, kontynuuj do nastêpnego przesuniêcia
        if (targetPosition != startBoardPosition && board->isBoardPositionOccupied(targetPosition))
            continue;

        // Pobierz pole celu
        Field* targetField = board->getFieldByBoardPosition(targetPosition);

        // Utwórz nowy krok dla celu
        StepTree* targetStep = new StepTree();
        targetStep->parent = currentStep;
        targetStep->field = targetField;
        targetStep->capture = attackPiece;

        // Dodaj figurê ataku do bie¿¹cych zdobyczy
        currentCaptures.insert(attackPiece);

        // Dodaj krok celu do listy nastêpnych kroków bie¿¹cego kroku
        currentStep->nextStepList.push_back(targetStep);


        addPossibleManJumps(targetStep, currentCaptures, startPiece);


        currentCaptures.erase(attackPiece);
    }
}


void Draughts::addPossibleKingJumps(StepTree* currentStep, std::set<Piece*>& currentCaptures, Piece* startPiece)
{
    // Pobierz kolor pocz¹tkowej figury
    Player::Color startColor = startPiece->getColor();
    // Pobierz pole pocz¹tkowe figury
    Field* startField = startPiece->getCurrentField();
    // Pobierz ostatni¹ pozycjê na planszy dla bie¿¹cego kroku
    sf::Vector2i lastBoardPosition = currentStep->field->getBoardPostion();
    // Pobierz pozycjê pocz¹tkow¹ na planszy dla figury
    sf::Vector2i startBoardPosition = startField->getBoardPostion();
    bool break2 = false;

    // Iteruj przez wszystkie dostêpne przesuniêcia
    for (const sf::Vector2i& offset : AllOffsets)
    {
        // Iteruj od 1 do liczby pól na boku planszy
        for (int i = 1; i < sideFieldsNumber; ++i)
        {
            // Oblicz pozycjê ataku
            sf::Vector2i attackPosition = lastBoardPosition + i * offset;

            // Jeœli pozycja ataku jest nieprawid³owa, przerwij pêtlê
            if (board->isBoardPositionInvalid(attackPosition))
                break;

            // Jeœli pozycja ataku jest wolna, kontynuuj do nastêpnego kroku
            if (!board->isBoardPositionOccupied(attackPosition))
                continue;

            // Pobierz pole ataku
            Field* attackField = board->getFieldByBoardPosition(attackPosition);
            // Pobierz figury na polu ataku
            Piece* attackPiece = attackField->getCurrentPiece();

            // Jeœli figura ataku jest ju¿ uwzglêdniona w bie¿¹cych zdobyczach, przerwij pêtlê
            if (std::find(currentCaptures.begin(), currentCaptures.end(), attackPiece) != currentCaptures.end())
                break;

            // Pobierz kolor figury ataku
            Player::Color attackColor = attackPiece->getColor();

            // Jeœli kolor figury ataku jest taki sam jak kolor pocz¹tkowej figury, przerwij pêtlê
            if (attackColor == startColor)
                break;

            // Iteruj od i+1 do liczby pól na boku planszy
            for (int j = i + 1; j < sideFieldsNumber; ++j)
            {
                // Oblicz pozycjê celu
                sf::Vector2i targetPosition = lastBoardPosition + j * offset;

                // Jeœli pozycja celu jest nieprawid³owa, przerwij pêtlê i przejdŸ do kolejnego przesuniêcia
                if (board->isBoardPositionInvalid(targetPosition))
                {
                    break2 = true;
                    break;
                }

                // Jeœli pozycja celu nie jest równa pozycji pocz¹tkowej i jest zajêta, przerwij pêtlê i przejdŸ do kolejnego przesuniêcia
                if (targetPosition != startBoardPosition && board->isBoardPositionOccupied(targetPosition))
                {
                    break2 = true;
                    break;
                }

                // Pobierz pole celu
                Field* targetField = board->getFieldByBoardPosition(targetPosition);

                // Utwórz nowy krok dla celu
                StepTree* targetStep = new StepTree();
                targetStep->parent = currentStep;
                targetStep->field = targetField;
                targetStep->capture = attackPiece;

                // Dodaj figurê ataku do bie¿¹cych zdobyczy
                currentCaptures.insert(attackPiece);

                // Dodaj krok celu do listy nastêpnych kroków bie¿¹cego kroku
                currentStep->nextStepList.push_back(targetStep);

                // Wywo³aj rekurencyjnie funkcjê dla nowego kroku celu
                addPossibleKingJumps(targetStep, currentCaptures, startPiece);

                // Usuñ figurê ataku z bie¿¹cych zdobyczy
                currentCaptures.erase(attackPiece);
            }

            // Jeœli flaga break2 jest ustawiona na true, przerwij pêtlê
            if (break2 == true)
            {
                break2 = false;
                break;
            }
        }
    }
}


void Draughts::narrowTurns()
{
    // Jeœli bie¿¹cy ruch jest pusty, nie ma ograniczonych ruchów
    if (currentTurn.empty()) {
        narrowedTurns = nullptr;
        return;
    }

    // Pobierz pole z ostatnim ruchem
    Field* currentField = currentTurn.back();

    // Jeœli bie¿¹cy ruch sk³ada siê tylko z jednego pola
    if (currentTurn.size() == 1U) {
        // Ograniczone ruchy s¹ zwi¹zane z t¹ pojedyncz¹ figur¹
        narrowedTurns = possibleTurns[currentField->getCurrentPiece()].get();
        return;
    }

    // W przeciwnym razie, iteruj przez kolejne kroki w ograniczonych ruchach
    for (StepTree* step : narrowedTurns->nextStepList) {
        // Jeœli pole kroku jest równe bie¿¹cemu polu
        if (step->field == currentField) {
            // Ustaw ograniczone ruchy na ten krok
            narrowedTurns = step;
            return;
        }
    }
}

void Draughts::validatePossibleTurns()
{
    // Zmienna przechowuj¹ca maksymaln¹ liczbê zbicia wœród mo¿liwych ruchów
    int maxCaptures = 0U;

    // Iteracja po wszystkich mo¿liwych ruchach
    for (std::pair<Piece*, const std::unique_ptr<StepTree>&> turnByPiece : possibleTurns) {
        StepTree* turn = turnByPiece.second.get();

        // Obliczanie liczby zbicia dla danego ruchu
        turn->evalCapturesNumber();

        // Aktualizowanie maksymalnej liczby zbicia
        maxCaptures = std::max(maxCaptures, turn->capturesNumber);
    }

    // Ponowna iteracja po mo¿liwych ruchach
    for (std::pair<Piece*, const std::unique_ptr<StepTree>&> turnByPiece : possibleTurns) {
        StepTree* turn = turnByPiece.second.get();

        // Walidacja ruchu, uwzglêdniaj¹c maksymaln¹ liczbê zbicia
        turn->validate(maxCaptures);
    }
}


bool Draughts::isPossibleTurn() const
{
    // Przeiteruj przez wszystkie mo¿liwe tury dla poszczególnych pionków.
    for (std::pair<Piece*, const std::unique_ptr<StepTree>&> turnByPiece : possibleTurns) {
        // Pobierz listê nastêpnych kroków dla danej tury.
        const std::list<StepTree*>& nextStepList = turnByPiece.second->nextStepList;

        // Przeiteruj przez wszystkie nastêpne kroki w liœcie.
        for (StepTree* nextStep : nextStepList) {
            // Jeœli krok jest prawid³owy, zwróæ true, co oznacza mo¿liwoœæ wykonania ruchu.
            if (nextStep->isValid) {
                return true;
            }
        }
    }

    // Jeœli ¿adne z mo¿liwych kroków nie jest prawid³owe, zwróæ false.
    return false;
}


void Draughts::highlightPossibleSteps()
{
    // Wyczyœæ zaznaczenie na planszy.
    board->clearSelection();

    // SprawdŸ, czy aktualna tura jest pusta.
    if (currentTurn.size() <= 0U) {
        // Przeiteruj przez wszystkie mo¿liwe tury dla poszczególnych pionków.
        for (std::pair<Piece*, const std::unique_ptr<StepTree>&> turnByPiece : possibleTurns) {
            // Pobierz pierwszy krok dla danej tury.
            StepTree* firstStep = turnByPiece.second.get();

            // Jeœli krok nie jest prawid³owy, przejdŸ do nastêpnej iteracji.
            if (!firstStep->isValid)
                continue;

            // Ustaw podœwietlenie pola, na którym znajduje siê pionek, jako dostêpne.
            firstStep->field->setHighlight(Field::Highlight::AvailablePiece);
        }
    }

    // Jeœli aktualna tura jest pusta, zakoñcz dzia³anie metody.
    if (currentTurn.empty())
        return;

    // Pobierz pole, na którym znajduje siê pionek w aktualnej turze.
    Field* currentField = currentTurn.back();

    // Ustaw podœwietlenie tego pola jako zaznaczone.
    currentField->setHighlight(Field::Highlight::Selected);

    // Przeiteruj przez wszystkie mo¿liwe kolejne kroki w zawê¿onej liœcie tur.
    for (StepTree* step : narrowedTurns->nextStepList) {
        // Jeœli krok nie jest prawid³owy, przejdŸ do nastêpnej iteracji.
        if (!step->isValid)
            continue;

        // Ustaw podœwietlenie pola jako dostêpne do ruchu.
        step->field->setHighlight(Field::Highlight::AvailableMove);
    }
}

void Draughts::changeTurn(bool reset)
{
    selectedPiece = nullptr;
    currentPlayer->turnFinished();

    // Zlicz czas wykorzystany przez aktualnego gracza


    if (currentPlayer == player1.get())
    {
        player1TotalTime += moveTimer.getElapsedTime();
    }
    else if (currentPlayer == player2.get())
    {
        player2TotalTime += moveTimer.getElapsedTime();
    }

    // Zresetuj zegar aktualnego gracza
    currentPlayer->getClock().restart();

    // Prze³¹cz graczy
    if (reset)
    {
        currentPlayer = (currentPlayer == player1.get()) ? player2.get() : player1.get();
    }

    // Wyczyœæ i przygotuj mo¿liwe ruchy
    currentTurn.clear();
    narrowedTurns = nullptr;
    clearPossibleTurns();
    addPossibleTurns();

    // Uruchom zegar ruchu
    moveTimer.restart();
}



void Draughts::removeCapturesFromBoard()
{
    // Rozpoczynamy od ostatniego ruchu i przechodzimy wstecz przez drzewo mo¿liwych ruchów
    StepTree* iter = narrowedTurns;
    for (; iter != nullptr; iter = iter->parent) {
        // Jeœli wêze³ drzewa nie zawiera informacji o zbitym pionku, kontynuujemy iteracjê
        if (iter->capture == nullptr)
            continue;

        // Usuwamy zbity pionek z planszy
        removeCapture(iter->capture);
    }
}

void Draughts::removeCapture(Piece* capture)
{
    // Usuwamy zbity pionek z planszy
    Field* field = capture->getCurrentField();
    field->setCurrentPiece(nullptr);

    // Usuwamy referencjê pola dla zbitego pionka
    capture->setCurrentField(nullptr);

    // Usuwamy pionek z listy pieces
    for (auto it = pieces.begin(); it != pieces.end(); ++it)
    {
        if ((*it).get() == capture)
        {
            pieces.erase(it);
            break;
        }
    }
}


bool Draughts::onClickButton(const sf::RectangleShape& button)
{
    if (gameState == GameState::Menu)
    {

        sf::Vector2i mousePosition = sf::Mouse::getPosition(renderWindow); // Pobranie pozycji kursora myszy
        sf::Vector2f mousePositionMapped = renderWindow.mapPixelToCoords(mousePosition); // Mapowanie pozycji myszy do wspó³rzêdnych okna

        // Sprawdzenie, czy kursor myszy znajduje siê na przycisku
        if (button.getGlobalBounds().contains(mousePositionMapped)) {
            return true;
        }
    }

    return false;
}

bool Draughts::validateName(const std::string& name)
{
    //wywo³anie tej metody znajduje sie w metodzie retrieve events ktora obsluguje zdarzenia
    //do tego wzorca pasuja ciagi znakow ktore zawieraja pierwsza litere wielka pozostale male, ³¹cznie mo¿e byæ 10 znaków
    std::regex reg("^[A-Z][a-z]{1,9}$");
    return std::regex_match(name, reg);

}




void Draughts::promoteIfPossible()
{
    int promotionLine = (currentPlayer->getColor() == Player::Color::White ? sideFieldsNumber - 1U : 0U);
    Field* targetField = currentTurn.back();
    Piece* movedPiece = targetField->getCurrentPiece();
    if (targetField->getBoardPostion().y == promotionLine) {
        movedPiece->upgrade();
    }
}

void Draughts::playerWon()
{
    gameOver = true;
    gameStarted = false;

    winner = player1.get();
    if (currentPlayer == player1.get())
    {
        winner = player2.get();
    }
    if (winner->getColor() == Player::Color::White)
    {
        std::string win = "           Zwyciezca: " + name_player2;
        winner_.setString(win);

    }
    else
    {
        std::string win = "           Zwyciezca: " + name_player1;
        winner_.setString(win);
        
    }

    //program wykorzystuje filesystem do stworzenia katalogu games w ktorym zapisywane sa informacje o rozgrywce
    
    // Zapisywanie informacji o rozgrywce do pliku
    // Zapisywanie nastepuje po zakonczeniu kazdej rozgrywki
    std::filesystem::create_directory("games"); // Tworzenie katalogu "games" (jeœli nie istnieje)
    std::string statsFile = "games/game_stats.txt";
    std::ofstream stats(statsFile, std::ios::app);
    
   
    if (stats.is_open()) {
        if (currentPlayer == player1.get()) {
            stats << name_player2 << ":lose\n" << name_player1 << ":win\n"; // Zapis wyniku dla gracza 1
        }
        else {
            stats << name_player1 << ":lose\n" << name_player2 << ":win\n"; // Zapis wyniku dla gracza 2
        }
        stats << std::endl;
        stats.close();
    }

    gameState = GameState::Menu;
}

void Draughts::selectPiece()
{
    selectedPiece = currentPlayer->getTurnPiece(possibleTurns);
    if (selectedPiece != nullptr) {
        Field* selectedField = selectedPiece->getCurrentField();
        currentTurn.push_back(selectedField);
    }
}

void Draughts::makeStep()
{
    StepTree* nextStep = currentPlayer->nextStep(selectedPiece, narrowedTurns);
    if (nextStep != nullptr) {
        Field* earlierSelectedField = currentTurn.back();
        Field* nextField = nextStep->field;

        selectedPiece->setCurrentField(nextField);
        earlierSelectedField->setCurrentPiece(nullptr);
        nextField->setCurrentPiece(selectedPiece);
        selectedPiece->transistToPosition(nextField->getPosition(), gameTime);

        currentTurn.push_back(nextField);
        narrowTurns();
        if (narrowedTurns->nextStepList.empty()) {
            removeCapturesFromBoard();
            promoteIfPossible();
            changeTurn();
        }
    }
}

void Draughts::countPlayerStats()
{
    std::string statsFile = "games/game_stats.txt";  // Œcie¿ka do pliku z danymi statystycznymi
    std::ifstream stats(statsFile);                 // Strumieñ do odczytu pliku

    std::string losePlayer1patt = name_player1 + ":lose";  // Wzorzec dla linii zawieraj¹cych informacje o przegranej gracza 1
    std::string losePlayer2patt = name_player2 + ":lose";  // Wzorzec dla linii zawieraj¹cych informacje o przegranej gracza 2
    std::string winPlayer1patt = name_player1 + ":win";    // Wzorzec dla linii zawieraj¹cych informacje o wygranej gracza 1
    std::string winPlayer2patt = name_player2 + ":win";    // Wzorzec dla linii zawieraj¹cych informacje o wygranej gracza 2

    int lose1 = 0;  // Licznik przegranych gracza 1
    int lose2 = 0;  // Licznik przegranych gracza 2
    int win1 = 0;   // Licznik wygranych gracza 1
    int win2 = 0;   // Licznik wygranych gracza 2

    if (stats.is_open())
    {
        std::string line;
        while (std::getline(stats, line))
        {
            if (line.find(losePlayer1patt) != std::string::npos)  // Jeœli linia zawiera informacje o przegranej gracza 1
            {
                lose1++;  // Inkrementuj licznik przegranych gracza 1
            }
            else if (line.find(losePlayer2patt) != std::string::npos)  // Jeœli linia zawiera informacje o przegranej gracza 2
            {
                lose2++;  // Inkrementuj licznik przegranych gracza 2
            }
            else if (line.find(winPlayer1patt) != std::string::npos)  // Jeœli linia zawiera informacje o wygranej gracza 1
            {
                win1++;  // Inkrementuj licznik wygranych gracza 1
            }
            else if (line.find(winPlayer2patt) != std::string::npos)  // Jeœli linia zawiera informacje o wygranej gracza 2
            {
                win2++;  // Inkrementuj licznik wygranych gracza 2
            }
        }
        stats.close();  // Zamknij plik
    }
    else
    {
        std::cout << "Nie mo¿na otworzyæ pliku z danymi.\n";  // Wyœwietl komunikat o b³êdzie, jeœli nie mo¿na otworzyæ pliku
    }

    player1->setLose(lose1);  // Ustaw liczbê przegranych gracza 1 w obiekcie player1
    player2->setLose(lose2);  // Ustaw liczbê przegranych gracza 2 w obiekcie player2
    player1->setWin(win1);    // Ustaw liczbê wygranych gracza 1 w obiekcie player1
    player2->setWin(win2);    // Ustaw liczbê wygranych gracza 2 w obiekcie player2
}
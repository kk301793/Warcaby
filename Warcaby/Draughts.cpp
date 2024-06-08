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
    error.setString(L"Uwaga: Imiona mog� sk�ada� si� wy��cznie z liter, pierwsza musi by� wielka [max 10 znakow]!");
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


    // Ustawienie ustawie� renderowania okna
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

    // Pobranie zdarze�
    retrieveEvents();

    // Aktualizacja planszy
    board->update(time);

    // Aktualizacja po�o�enia pionkow
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
                std::cout << "Nast�pi�a zmiana tur, brak ruchu w dozwolonym czasie\n";
            }
        }
        else if (gameKind == GameKind::Limited_time_per_game) {
            // Sprawdzenie limitu czasu na gr�

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

            // Ka�dy z graczy ma 2 minuty na gr�, je�li czas kt�rego� z nich minie, to drugi wygrywa
        }

        // Wyb�r i wykonanie ruchu
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
    gameplayTexture.clear(sf::Color(159U, 117U, 69U));   //kolor g��wnej tekstury

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

        // wszystkie pionki, kt�re si� nie ruszaj� i nie s� zbite
        for (const std::unique_ptr<Piece>& piece : pieces)
        {
            //pionek jest zbity
            if (piece->getCurrentField() == nullptr)
                continue;
            gameplayTexture.draw(*piece);
        }

        if (gameState == GameState::Menu)
        {

            //sf::RectangleShape startButton();  // Tworzenie prostok�ta jako przycisku
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
                startText.setString(L"Pon�w");

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
        sf::Text choice2(L"Czas na gr�\nwynosi 2 minuty\ndla ka�dego z graczy!", font);
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

        nextText.setString(L"Zmie�\ngraczy");
        changeText.setString(L"Zmie�\ntryb");
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
        // Wy�wietlanie pozosta�ego czasu na ruch

        // Tworzenie strumienia i formatowanie czasu
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(2) << 10. - moveTimer.getElapsedTime().asSeconds();

        // Ustawianie tekstu z informacj� o pozosta�ym czasie
        time.setString("Do ko�ca tury: " + stream.str());
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

        // Wy�wietl czas wykorzystany przez graczy
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



        // Umie�� tekst w odpowiednich pozycjach
        sf::Text player1TimeText(player1TimeString, font, 20);
        sf::Text player2TimeText(player2TimeString, font, 20);
        player1TimeText.setStyle(sf::Text::Bold);
        player2TimeText.setStyle(sf::Text::Bold);
        player1TimeText.setPosition(10.f, 10.f);
        player2TimeText.setPosition(10.f, renderWindow.getSize().y - 40.f);
        player1TimeText.setFillColor(sf::Color::Black);
        player2TimeText.setFillColor(sf::Color::Black);

        // Wy�wietl tekst
        renderWindow.draw(player1TimeText);
        renderWindow.draw(player2TimeText);
    }

    if (gameState == GameState::Gameplay)
    {
        // Rysowanie przycisk�w

        // Ustawienie koloru obramowania przycisk�w na czarny
        changeButton.setOutlineColor(sf::Color::Black);
        nextButton.setOutlineColor(sf::Color::Black);
        startButton.setOutlineColor(sf::Color::Black);
        giveMove.setOutlineColor(sf::Color::Black);

        // Ustawienie grubo�ci obramowania przycisk�w na 3 piksele
        changeButton.setOutlineThickness(3);
        nextButton.setOutlineThickness(3);
        startButton.setOutlineThickness(3);
        giveMove.setOutlineThickness(3);

        // Ustawienie koloru wype�nienia przycisk�w na przezroczysty
        giveMove.setFillColor(sf::Color(0, 0, 0, 0));
        changeButton.setFillColor(sf::Color(0, 0, 0, 0));

        // Ustalenie pozycji przycisk�w na podstawie rozmiaru okna renderowania
        changeButton.setPosition(renderWindow.getSize().x - 175.f, renderWindow.getSize().y - 325.f);
        nextButton.setPosition(renderWindow.getSize().x - 175.f, renderWindow.getSize().y - 250.f);
        startButton.setPosition(renderWindow.getSize().x - 175.f, renderWindow.getSize().y - 100.f);
        giveMove.setPosition(renderWindow.getSize().x - 175.f, renderWindow.getSize().y - 175.f);

        // Utworzenie tekstu przycisk�w
        sf::Text buttonText1(L"Poddaj si�", font, 13);
        sf::Text buttonText2(L"Odpu�� tur�", font, 13);
        sf::Text buttonText3(L"Wyjd�", font, 13);
        sf::Text buttonText4(L"Zmie� pionek", font, 13);

        // Ustawienie koloru tekstu przycisk�w na czarny
        buttonText1.setFillColor(sf::Color::Black);
        buttonText2.setFillColor(sf::Color::Black);
        buttonText3.setFillColor(sf::Color::Black);
        buttonText4.setFillColor(sf::Color::Black);

        // Ustalenie pozycji tekstu przycisk�w
        buttonText3.setPosition(renderWindow.getSize().x - 175.f, renderWindow.getSize().y - 85.f);
        buttonText2.setPosition(renderWindow.getSize().x - 175.f, renderWindow.getSize().y - 160.f);
        buttonText1.setPosition(renderWindow.getSize().x - 175.f, renderWindow.getSize().y - 235.f);
        buttonText4.setPosition(renderWindow.getSize().x - 175.f, renderWindow.getSize().y - 310.f);

        // Rysowanie tekstu przycisk�w
        renderWindow.draw(buttonText1);
        renderWindow.draw(buttonText2);
        renderWindow.draw(buttonText3);
        renderWindow.draw(buttonText4);

        // Rysowanie przycisk�w
        renderWindow.draw(changeButton);
        renderWindow.draw(nextButton);
        renderWindow.draw(startButton);
        renderWindow.draw(giveMove);
    }

    // Rysowanie informacji o zwyci�zcy
    renderWindow.draw(winner_);

    // Wy�wietlanie renderowanej zawarto�ci
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
    // Wyczyszczenie wektora pionk�w
    pieces.clear();
    // Zmiana rozmiaru wektora pionk�w na podstawie liczby pionk�w
    pieces.resize(piecesNumber);

    // Tworzenie pionk�w i przypisanie im odpowiednich kolor�w
    for (int i = 0U; i < piecesNumber; ++i) {
        Player::Color color = Player::Color::White;
        if (i >= piecesNumber / 2)
            color = Player::Color::Black;
        pieces[i] = std::make_unique<Piece>(i, color);
    }

    // Inicjalizacja pionk�w i ustawienie marginesu p�l
    for (const std::unique_ptr<Piece>& piece : pieces) {
        piece->init();

    }
}


void Draughts::prepareGame()
{
    // Resetowanie zmiennych i obiekt�w zwi�zanych z rozgrywk�
    gameStarted = false;
    gameOver = false;
    winner = nullptr;
    captures.clear();
    selectedPiece = nullptr;
    currentTurn.clear();
    narrowedTurns = nullptr;
    clearPossibleTurns();


    // Obliczanie liczby pionk�w na podstawie ustawie� planszy
    piecesNumber = playerPiecesRowsNumber * 2 * sideFieldsNumber / 2;

    // Tworzenie planszy i pionk�w
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

    // Ustawienie gracza rozpoczynaj�cego
    currentPlayer = player1.get();

    // Ustalenie pozycji pionk�w na planszy
    setPiecesPosition();

    // Generowanie mo�liwych ruch�w
    addPossibleTurns();



}


void Draughts::startGame()
{
    gameState = GameState::Gameplay;

    gameStarted = true;
}



void Draughts::setPiecesPosition()
{
    // Pobierz list� czarnych p�l na planszy
    std::list<Field*> blackFields = board->getFieldsByColor(Field::Color::Black);

    // Przypisz pozycje pionk�w
    for (const std::unique_ptr<Piece>& piece : pieces)
    {
        Field* field;

        // Je�li pionek jest bia�y, pobierz pierwsze czarne pole z listy
        if (piece->getColor() == Player::Color::White)
        {
            field = blackFields.front();
            blackFields.pop_front();
        }
        // Je�li pionek jest czarny, pobierz ostatnie czarne pole z listy
        else
        {
            field = blackFields.back();
            blackFields.pop_back();
        }

        // Ustaw bie��ce pole dla pionka i pionek dla pola
        piece->setCurrentField(field);
        field->setCurrentPiece(piece.get());

        // Ustaw pozycj� pionka na pozycj� pola
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
            // Obs�uga zdarze� w menu
            if (event.type == sf::Event::EventType::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                // Sprawdzanie, czy naci�ni�cie nast�pi�o na przycisku "Start"
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
                        // Poddanie si�
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
            // Obs�uga zdarze� podczas logowania

            // Obs�uga wprowadzania tekstu
            if (event.type == sf::Event::TextEntered && current_player != nullptr)
            {
                if (event.text.unicode == '\b' && !current_player->getString().isEmpty())
                {
                    // Usuni�cie ostatniego znaku
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
                    // Przej�cie do drugiego imienia po wci�ni�ciu Enter
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
            // Obs�uga zdarze� w menu wyboru

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
    // Pobierz pole, na kt�re naci�ni�to myszk�
    Field* pressedField = board->getFieldByPosition(pressPoint);

    // Je�li nie znaleziono pola, zako�cz funkcj�
    if (pressedField == nullptr)
        return;


    // Sprawd�, czy istnieje mo�liwy ruch dla wybranej figury
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

    // Wywo�aj metod� fieldClicked() aktualnego gracza, przekazuj�c wci�ni�te pole
    currentPlayer->fieldClicked(pressedField);
}






void Draughts::addPossibleTurns()
{
    // Dla ka�dej figury gracza
    for (const std::unique_ptr<Piece>& piece : pieces) {
        // Sprawd�, czy kolor figury odpowiada obecnemu graczowi
        if (currentPlayer->getColor() != piece->getColor())
            continue;
        // Pobierz pole, na kt�rym znajduje si� figura
        Field* field = piece->getCurrentField();
        // Je�li figura nie znajduje si� na �adnym polu, przejd� do kolejnej figury
        if (field == nullptr)
            continue;
        // Utw�rz mo�liwy pusty ruch dla figury
        createPossibleEmptyTurn(piece.get());
        // Dodaj mo�liwe ruchy figury
        addPossiblePieceMoves(piece.get());
        // Dodaj mo�liwe bicia figury
        addPossiblePieceJumps(piece.get());
    }
    // Zweryfikuj mo�liwe ruchy
    validatePossibleTurns();
    // Zresetuj zaw�one mo�liwe ruchy
    narrowedTurns = nullptr;
    // Je�li istnieje mo�liwy ruch, zako�cz funkcj�
    if (isPossibleTurn())
        return;
    // W przeciwnym razie gracz wygra�
    playerWon();
    // Wyczy�� zaznaczenie planszy
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
    // Pobierz pole, na kt�rym znajduje si� figura
    Field* field = piece->getCurrentField();
    // Pobierz pozycj� na planszy figury
    sf::Vector2i boardPosition = field->getBoardPostion();
    // Pobierz typ figury
    Piece::PieceType pieceType = piece->getPieceType();
    // Pobierz kolor figury
    Player::Color color = piece->getColor();
    // Okre�l kierunek ruchu do przodu w zale�no�ci od koloru figury
    int forwardDirection = 1;
    if (color == Player::Color::Black)
        forwardDirection = -1;
    // Pobierz poprzedni krok w drzewie krok�w dla figury
    StepTree* previousStep = possibleTurns[piece].get();

    // Je�li figura jest typu "Man"
    if (pieceType == Piece::PieceType::Man)
    {
        // Mo�liwe przesuni�cia dla figury typu "Man"
        const size_t forwardMovesOffsetsNumber = 2U;
        const sf::Vector2i forwardMovesOffsets[forwardMovesOffsetsNumber] = { sf::Vector2i(-1, forwardDirection),
                                                                              sf::Vector2i(1, forwardDirection) };
        std::list<sf::Vector2i> forwardMoves;

        // Sprawd� mo�liwe przesuni�cia w prz�d
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
    // Je�li figura jest typu "King"
    else if (pieceType == Piece::PieceType::King)
    {
        std::list<sf::Vector2i> allDirectionMoves;

        // Sprawd� mo�liwe przesuni�cia we wszystkich kierunkach
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

        // Dodaj mo�liwe przesuni�cia dla figury typu "King"
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
    // Pobierz pole, na kt�rym znajduje si� figura
    Field* field = piece->getCurrentField();
    // Pobierz typ figury
    Piece::PieceType pieceType = piece->getPieceType();
    // Pobierz drzewo krok�w dla figury
    StepTree* stepTree = possibleTurns[piece].get();
    // Zbi�r przechowuj�cy figury, kt�re zosta�y ju� zdobyte w bie��cym ruchu
    std::set<Piece*> captures;

    // Je�li figura jest typu "Man"
    if (pieceType == Piece::PieceType::Man)
    {
        // Dodaj mo�liwe skoki dla figury typu "Man"
        addPossibleManJumps(stepTree, captures, piece);
    }
    // Je�li figura jest typu "King"
    else if (pieceType == Piece::PieceType::King)
    {
        // Dodaj mo�liwe skoki dla figury typu "King"
        addPossibleKingJumps(stepTree, captures, piece);
    }
}

void Draughts::addPossibleManJumps(StepTree* currentStep, std::set<Piece*>& currentCaptures, Piece* startPiece)
{
    // Pobierz kolor pocz�tkowej figury
    Player::Color startColor = startPiece->getColor();
    // Pobierz pole pocz�tkowe figury
    Field* startField = startPiece->getCurrentField();
    // Pobierz ostatni� pozycj� na planszy dla bie��cego kroku
    sf::Vector2i lastBoardPosition = currentStep->field->getBoardPostion();
    // Pobierz pozycj� pocz�tkow� na planszy dla figury
    sf::Vector2i startBoardPosition = startField->getBoardPostion();

    // Iteruj przez wszystkie dost�pne przesuni�cia
    for (const sf::Vector2i& offset : AllOffsets) {
        // Oblicz pozycj� ataku
        sf::Vector2i attackPosition = lastBoardPosition + offset;

        // Je�li pozycja ataku jest nieprawid�owa, kontynuuj do nast�pnego przesuni�cia
        if (board->isBoardPositionInvalid(attackPosition))
            continue;

        // Je�li pozycja ataku jest wolna, kontynuuj do nast�pnego przesuni�cia
        if (!board->isBoardPositionOccupied(attackPosition))
            continue;

        // Pobierz pole ataku
        Field* attackField = board->getFieldByBoardPosition(attackPosition);
        // Pobierz figur� ataku
        Piece* attackPiece = attackField->getCurrentPiece();

        // Je�li figura ataku jest ju� uwzgl�dniona w bie��cych zdobyczach, kontynuuj do nast�pnego przesuni�cia
        if (currentCaptures.find(attackPiece) != currentCaptures.end())
            continue;

        // Pobierz kolor figury ataku
        Player::Color attackColor = attackPiece->getColor();

        // Je�li kolor figury ataku jest taki sam jak kolor pocz�tkowej figury, kontynuuj do nast�pnego przesuni�cia
        if (attackColor == startColor)
            continue;

        // Oblicz pozycj� celu
        sf::Vector2i targetPosition = lastBoardPosition + 2 * offset;

        // Je�li pozycja celu jest nieprawid�owa, kontynuuj do nast�pnego przesuni�cia
        if (board->isBoardPositionInvalid(targetPosition))
            continue;

        // Je�li pozycja celu nie jest r�wna pozycji pocz�tkowej i jest zaj�ta, kontynuuj do nast�pnego przesuni�cia
        if (targetPosition != startBoardPosition && board->isBoardPositionOccupied(targetPosition))
            continue;

        // Pobierz pole celu
        Field* targetField = board->getFieldByBoardPosition(targetPosition);

        // Utw�rz nowy krok dla celu
        StepTree* targetStep = new StepTree();
        targetStep->parent = currentStep;
        targetStep->field = targetField;
        targetStep->capture = attackPiece;

        // Dodaj figur� ataku do bie��cych zdobyczy
        currentCaptures.insert(attackPiece);

        // Dodaj krok celu do listy nast�pnych krok�w bie��cego kroku
        currentStep->nextStepList.push_back(targetStep);


        addPossibleManJumps(targetStep, currentCaptures, startPiece);


        currentCaptures.erase(attackPiece);
    }
}


void Draughts::addPossibleKingJumps(StepTree* currentStep, std::set<Piece*>& currentCaptures, Piece* startPiece)
{
    // Pobierz kolor pocz�tkowej figury
    Player::Color startColor = startPiece->getColor();
    // Pobierz pole pocz�tkowe figury
    Field* startField = startPiece->getCurrentField();
    // Pobierz ostatni� pozycj� na planszy dla bie��cego kroku
    sf::Vector2i lastBoardPosition = currentStep->field->getBoardPostion();
    // Pobierz pozycj� pocz�tkow� na planszy dla figury
    sf::Vector2i startBoardPosition = startField->getBoardPostion();
    bool break2 = false;

    // Iteruj przez wszystkie dost�pne przesuni�cia
    for (const sf::Vector2i& offset : AllOffsets)
    {
        // Iteruj od 1 do liczby p�l na boku planszy
        for (int i = 1; i < sideFieldsNumber; ++i)
        {
            // Oblicz pozycj� ataku
            sf::Vector2i attackPosition = lastBoardPosition + i * offset;

            // Je�li pozycja ataku jest nieprawid�owa, przerwij p�tl�
            if (board->isBoardPositionInvalid(attackPosition))
                break;

            // Je�li pozycja ataku jest wolna, kontynuuj do nast�pnego kroku
            if (!board->isBoardPositionOccupied(attackPosition))
                continue;

            // Pobierz pole ataku
            Field* attackField = board->getFieldByBoardPosition(attackPosition);
            // Pobierz figury na polu ataku
            Piece* attackPiece = attackField->getCurrentPiece();

            // Je�li figura ataku jest ju� uwzgl�dniona w bie��cych zdobyczach, przerwij p�tl�
            if (std::find(currentCaptures.begin(), currentCaptures.end(), attackPiece) != currentCaptures.end())
                break;

            // Pobierz kolor figury ataku
            Player::Color attackColor = attackPiece->getColor();

            // Je�li kolor figury ataku jest taki sam jak kolor pocz�tkowej figury, przerwij p�tl�
            if (attackColor == startColor)
                break;

            // Iteruj od i+1 do liczby p�l na boku planszy
            for (int j = i + 1; j < sideFieldsNumber; ++j)
            {
                // Oblicz pozycj� celu
                sf::Vector2i targetPosition = lastBoardPosition + j * offset;

                // Je�li pozycja celu jest nieprawid�owa, przerwij p�tl� i przejd� do kolejnego przesuni�cia
                if (board->isBoardPositionInvalid(targetPosition))
                {
                    break2 = true;
                    break;
                }

                // Je�li pozycja celu nie jest r�wna pozycji pocz�tkowej i jest zaj�ta, przerwij p�tl� i przejd� do kolejnego przesuni�cia
                if (targetPosition != startBoardPosition && board->isBoardPositionOccupied(targetPosition))
                {
                    break2 = true;
                    break;
                }

                // Pobierz pole celu
                Field* targetField = board->getFieldByBoardPosition(targetPosition);

                // Utw�rz nowy krok dla celu
                StepTree* targetStep = new StepTree();
                targetStep->parent = currentStep;
                targetStep->field = targetField;
                targetStep->capture = attackPiece;

                // Dodaj figur� ataku do bie��cych zdobyczy
                currentCaptures.insert(attackPiece);

                // Dodaj krok celu do listy nast�pnych krok�w bie��cego kroku
                currentStep->nextStepList.push_back(targetStep);

                // Wywo�aj rekurencyjnie funkcj� dla nowego kroku celu
                addPossibleKingJumps(targetStep, currentCaptures, startPiece);

                // Usu� figur� ataku z bie��cych zdobyczy
                currentCaptures.erase(attackPiece);
            }

            // Je�li flaga break2 jest ustawiona na true, przerwij p�tl�
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
    // Je�li bie��cy ruch jest pusty, nie ma ograniczonych ruch�w
    if (currentTurn.empty()) {
        narrowedTurns = nullptr;
        return;
    }

    // Pobierz pole z ostatnim ruchem
    Field* currentField = currentTurn.back();

    // Je�li bie��cy ruch sk�ada si� tylko z jednego pola
    if (currentTurn.size() == 1U) {
        // Ograniczone ruchy s� zwi�zane z t� pojedyncz� figur�
        narrowedTurns = possibleTurns[currentField->getCurrentPiece()].get();
        return;
    }

    // W przeciwnym razie, iteruj przez kolejne kroki w ograniczonych ruchach
    for (StepTree* step : narrowedTurns->nextStepList) {
        // Je�li pole kroku jest r�wne bie��cemu polu
        if (step->field == currentField) {
            // Ustaw ograniczone ruchy na ten krok
            narrowedTurns = step;
            return;
        }
    }
}

void Draughts::validatePossibleTurns()
{
    // Zmienna przechowuj�ca maksymaln� liczb� zbicia w�r�d mo�liwych ruch�w
    int maxCaptures = 0U;

    // Iteracja po wszystkich mo�liwych ruchach
    for (std::pair<Piece*, const std::unique_ptr<StepTree>&> turnByPiece : possibleTurns) {
        StepTree* turn = turnByPiece.second.get();

        // Obliczanie liczby zbicia dla danego ruchu
        turn->evalCapturesNumber();

        // Aktualizowanie maksymalnej liczby zbicia
        maxCaptures = std::max(maxCaptures, turn->capturesNumber);
    }

    // Ponowna iteracja po mo�liwych ruchach
    for (std::pair<Piece*, const std::unique_ptr<StepTree>&> turnByPiece : possibleTurns) {
        StepTree* turn = turnByPiece.second.get();

        // Walidacja ruchu, uwzgl�dniaj�c maksymaln� liczb� zbicia
        turn->validate(maxCaptures);
    }
}


bool Draughts::isPossibleTurn() const
{
    // Przeiteruj przez wszystkie mo�liwe tury dla poszczeg�lnych pionk�w.
    for (std::pair<Piece*, const std::unique_ptr<StepTree>&> turnByPiece : possibleTurns) {
        // Pobierz list� nast�pnych krok�w dla danej tury.
        const std::list<StepTree*>& nextStepList = turnByPiece.second->nextStepList;

        // Przeiteruj przez wszystkie nast�pne kroki w li�cie.
        for (StepTree* nextStep : nextStepList) {
            // Je�li krok jest prawid�owy, zwr�� true, co oznacza mo�liwo�� wykonania ruchu.
            if (nextStep->isValid) {
                return true;
            }
        }
    }

    // Je�li �adne z mo�liwych krok�w nie jest prawid�owe, zwr�� false.
    return false;
}


void Draughts::highlightPossibleSteps()
{
    // Wyczy�� zaznaczenie na planszy.
    board->clearSelection();

    // Sprawd�, czy aktualna tura jest pusta.
    if (currentTurn.size() <= 0U) {
        // Przeiteruj przez wszystkie mo�liwe tury dla poszczeg�lnych pionk�w.
        for (std::pair<Piece*, const std::unique_ptr<StepTree>&> turnByPiece : possibleTurns) {
            // Pobierz pierwszy krok dla danej tury.
            StepTree* firstStep = turnByPiece.second.get();

            // Je�li krok nie jest prawid�owy, przejd� do nast�pnej iteracji.
            if (!firstStep->isValid)
                continue;

            // Ustaw pod�wietlenie pola, na kt�rym znajduje si� pionek, jako dost�pne.
            firstStep->field->setHighlight(Field::Highlight::AvailablePiece);
        }
    }

    // Je�li aktualna tura jest pusta, zako�cz dzia�anie metody.
    if (currentTurn.empty())
        return;

    // Pobierz pole, na kt�rym znajduje si� pionek w aktualnej turze.
    Field* currentField = currentTurn.back();

    // Ustaw pod�wietlenie tego pola jako zaznaczone.
    currentField->setHighlight(Field::Highlight::Selected);

    // Przeiteruj przez wszystkie mo�liwe kolejne kroki w zaw�onej li�cie tur.
    for (StepTree* step : narrowedTurns->nextStepList) {
        // Je�li krok nie jest prawid�owy, przejd� do nast�pnej iteracji.
        if (!step->isValid)
            continue;

        // Ustaw pod�wietlenie pola jako dost�pne do ruchu.
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

    // Prze��cz graczy
    if (reset)
    {
        currentPlayer = (currentPlayer == player1.get()) ? player2.get() : player1.get();
    }

    // Wyczy�� i przygotuj mo�liwe ruchy
    currentTurn.clear();
    narrowedTurns = nullptr;
    clearPossibleTurns();
    addPossibleTurns();

    // Uruchom zegar ruchu
    moveTimer.restart();
}



void Draughts::removeCapturesFromBoard()
{
    // Rozpoczynamy od ostatniego ruchu i przechodzimy wstecz przez drzewo mo�liwych ruch�w
    StepTree* iter = narrowedTurns;
    for (; iter != nullptr; iter = iter->parent) {
        // Je�li w�ze� drzewa nie zawiera informacji o zbitym pionku, kontynuujemy iteracj�
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

    // Usuwamy referencj� pola dla zbitego pionka
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
        sf::Vector2f mousePositionMapped = renderWindow.mapPixelToCoords(mousePosition); // Mapowanie pozycji myszy do wsp�rz�dnych okna

        // Sprawdzenie, czy kursor myszy znajduje si� na przycisku
        if (button.getGlobalBounds().contains(mousePositionMapped)) {
            return true;
        }
    }

    return false;
}

bool Draughts::validateName(const std::string& name)
{
    //wywo�anie tej metody znajduje sie w metodzie retrieve events ktora obsluguje zdarzenia
    //do tego wzorca pasuja ciagi znakow ktore zawieraja pierwsza litere wielka pozostale male, ��cznie mo�e by� 10 znak�w
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
    std::filesystem::create_directory("games"); // Tworzenie katalogu "games" (je�li nie istnieje)
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
    std::string statsFile = "games/game_stats.txt";  // �cie�ka do pliku z danymi statystycznymi
    std::ifstream stats(statsFile);                 // Strumie� do odczytu pliku

    std::string losePlayer1patt = name_player1 + ":lose";  // Wzorzec dla linii zawieraj�cych informacje o przegranej gracza 1
    std::string losePlayer2patt = name_player2 + ":lose";  // Wzorzec dla linii zawieraj�cych informacje o przegranej gracza 2
    std::string winPlayer1patt = name_player1 + ":win";    // Wzorzec dla linii zawieraj�cych informacje o wygranej gracza 1
    std::string winPlayer2patt = name_player2 + ":win";    // Wzorzec dla linii zawieraj�cych informacje o wygranej gracza 2

    int lose1 = 0;  // Licznik przegranych gracza 1
    int lose2 = 0;  // Licznik przegranych gracza 2
    int win1 = 0;   // Licznik wygranych gracza 1
    int win2 = 0;   // Licznik wygranych gracza 2

    if (stats.is_open())
    {
        std::string line;
        while (std::getline(stats, line))
        {
            if (line.find(losePlayer1patt) != std::string::npos)  // Je�li linia zawiera informacje o przegranej gracza 1
            {
                lose1++;  // Inkrementuj licznik przegranych gracza 1
            }
            else if (line.find(losePlayer2patt) != std::string::npos)  // Je�li linia zawiera informacje o przegranej gracza 2
            {
                lose2++;  // Inkrementuj licznik przegranych gracza 2
            }
            else if (line.find(winPlayer1patt) != std::string::npos)  // Je�li linia zawiera informacje o wygranej gracza 1
            {
                win1++;  // Inkrementuj licznik wygranych gracza 1
            }
            else if (line.find(winPlayer2patt) != std::string::npos)  // Je�li linia zawiera informacje o wygranej gracza 2
            {
                win2++;  // Inkrementuj licznik wygranych gracza 2
            }
        }
        stats.close();  // Zamknij plik
    }
    else
    {
        std::cout << "Nie mo�na otworzy� pliku z danymi.\n";  // Wy�wietl komunikat o b��dzie, je�li nie mo�na otworzy� pliku
    }

    player1->setLose(lose1);  // Ustaw liczb� przegranych gracza 1 w obiekcie player1
    player2->setLose(lose2);  // Ustaw liczb� przegranych gracza 2 w obiekcie player2
    player1->setWin(win1);    // Ustaw liczb� wygranych gracza 1 w obiekcie player1
    player2->setWin(win2);    // Ustaw liczb� wygranych gracza 2 w obiekcie player2
}
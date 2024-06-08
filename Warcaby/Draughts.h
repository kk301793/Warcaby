#pragma once

#include "Board.h"
#include "StepTree.h"
#include "Transition.h"
#include "Field.h"
#include "Piece.h"
#include "Player.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <sstream>

#include <memory>
#include <set>
#include <regex>
#include <iomanip>



class Draughts
{
public:
    enum struct GameState { None = 0, Menu, Gameplay, Login, Choice };
    enum struct GameKind { None = 0, Limited_time_per_move, Limited_time_per_game };

    Draughts();
    ~Draughts();

    void init();
    void update(sf::Time time);
    void render();

    bool shouldFinish() const;
    void finish();

private:
    static const size_t AllOffsetsNumber = 4U;
    static const sf::Vector2i AllOffsets[AllOffsetsNumber];

    sf::RenderWindow renderWindow;
    sf::View gameplayView;
    sf::Vector2f viewLeftTopCorner;
    float fontScaleFactor;
    sf::RenderTexture gameplayTexture;
    sf::RectangleShape startButton;
    sf::RectangleShape per_Move;
    sf::RectangleShape per_Game;
    sf::RectangleShape nameBox;
    sf::RectangleShape nameBox2;
    sf::RectangleShape nextButton;
    sf::RectangleShape giveMove;
    sf::RectangleShape changeButton;
    sf::RectangleShape closeButton;
    sf::Text text;
    sf::Text startText;
    sf::Text nextText;
    sf::Text changeText;
    sf::Text closeText;
    sf::Text time;
    std::string name_player1;
    std::string name_player2;
    sf::Text white;
    sf::Text black;
    sf::Text winner_;
    sf::Text NAME_player1;
    sf::Text NAME_player2;
    sf::Text error;
    sf::Text* current_player;
    std::string* current__player;
    sf::Font font;

    bool gameStarted;
    bool gameOver;
    bool isFinished_;
    bool errorLogin;
    bool changing;
    int sideFieldsNumber;
    std::unique_ptr<Board> board;
    std::unique_ptr<Player> player1, player2;
    Player* winner;

    int playerPiecesRowsNumber;
    int piecesNumber;

    std::vector<std::unique_ptr<Piece>> pieces;


    Player* currentPlayer;
    bool waitingForInteraction;

    Piece* selectedPiece;
    std::list<Field*> currentTurn;
    std::map<Piece*, std::unique_ptr<StepTree>> possibleTurns;
    StepTree* narrowedTurns;
    std::list<Piece*> captures;

    GameState gameState;
    GameKind gameKind;

    sf::Time gameTime;
    sf::Time player1TotalTime;
    sf::Time player2TotalTime;
    sf::Clock moveTimer;

    void recreateBoard();
    void recreatePieces();
    void prepareGame();
    void startGame();


    void setPiecesPosition();

    void retrieveEvents();
    void mouseClickedOnBoard(const sf::Vector2f& pressPosition);
    bool onClickButton(const sf::RectangleShape& button);
    bool validateName(const std::string& name);


    void addPossibleTurns();
    void clearPossibleTurns();
    void createPossibleEmptyTurn(Piece* piece);
    void addPossiblePieceMoves(Piece* piece);
    void addPossiblePieceJumps(Piece* piece);
    void addPossibleManJumps(StepTree* currentStep, std::set<Piece*>& currentCaptures, Piece* startPiece);
    void addPossibleKingJumps(StepTree* currentStep, std::set<Piece*>& currentCaptures, Piece* startPiece);

    void narrowTurns();
    void validatePossibleTurns();
    bool isPossibleTurn() const;

    void highlightPossibleSteps();

    void changeTurn(bool reset = true);
    void removeCapturesFromBoard();
    void removeCapture(Piece* capture);
    void promoteIfPossible();
    void playerWon();
    void selectPiece();
    void makeStep();

    void countPlayerStats();

};

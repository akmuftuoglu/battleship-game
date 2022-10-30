//
//  Game.cpp
//  Project 3
//

#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>

using namespace std;

class GameImpl
{
  public:
    GameImpl(int nRows, int nCols);
    int rows() const;
    int cols() const;
    bool isValid(Point p) const;
    Point randomPoint() const;
    bool addShip(int length, char symbol, string name);
    int nShips() const;
    int shipLength(int shipId) const;
    char shipSymbol(int shipId) const;
    string shipName(int shipId) const;
    Player* play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause);
    

    
  private:
    int gameRows;
    int gameCols;
    int shipsForGame;
    
    struct Ship
    {
        int shipSize;
        string shipName;
        char shipSymbol;
    };
    
    vector<Ship> allShips;
    
};

void waitForEnter()
{
    cout << "Press enter to continue: ";
    cin.ignore(10000, '\n');
}

GameImpl::GameImpl(int nRows, int nCols) : gameRows(nRows), gameCols(nCols), shipsForGame(0) {}

int GameImpl::rows() const
{
    return gameRows;
}

int GameImpl::cols() const
{
    return gameCols;
}

bool GameImpl::isValid(Point p) const
{
    return p.r >= 0  &&  p.r < rows()  &&  p.c >= 0  &&  p.c < cols();
}

Point GameImpl::randomPoint() const
{
    return Point(randInt(rows()), randInt(cols()));
}

bool GameImpl::addShip(int length, char symbol, string name)
{
    
    Ship newShip;
    newShip.shipSize = length;
    newShip.shipSymbol = symbol;
    newShip.shipName = name;
    
    allShips.push_back(newShip);
    shipsForGame++;
    
    return true;
}

int GameImpl::nShips() const
{
    return shipsForGame;
}

int GameImpl::shipLength(int shipId) const
{
    return allShips[shipId].shipSize;
}

char GameImpl::shipSymbol(int shipId) const
{
    return allShips[shipId].shipSymbol;
}

string GameImpl::shipName(int shipId) const
{
    return allShips[shipId].shipName;
}

Player* GameImpl::play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause)
{    
    Player *t1, *t2;
    
    Board* b;
        
    bool p1Turn = true;
        
    if (p1->placeShips(b1) == false) {return nullptr;}
    if (p2->placeShips(b2) == false) {return nullptr;}
        
    while (!b1.allShipsDestroyed() && !b2.allShipsDestroyed())
    {
        bool isHit = false;
        bool isDestroyed = false;
        bool isValidShot = false;
        int shipId = 0;
        
        if (p1Turn)
        {
            t1 = p1;
            t2 = p2;
            b = &b2;
        }
        else
        {
            t1 = p2;
            t2 = p1;
            b = &b1;
        }
            
        cout << t1->name() << "'s turn. Board for " << t2->name() << ":" << endl;
        b->display(t1->isHuman());
            
        Point attackCoord = t1->recommendAttack();
        isValidShot = b->attack(attackCoord, isHit, isDestroyed, shipId);
        t1->recordAttackResult(attackCoord, isValidShot, isHit, isDestroyed, shipId);
         
        if (t1->isHuman() && shipId == -1)
            {
                cout << t1->name() << " wasted a shot at (" << attackCoord.r << "," << attackCoord.c << ")." << endl;
            }
        else
            {
                cout << t1->name() << " attacked (" << attackCoord.r << "," << attackCoord.c << ") and ";
                
                if (isHit == true && isDestroyed == true)
                    {
                        cout << "destroyed the " << this->shipName(shipId);
                    }
                else if (isHit == true)
                    {
                        cout << "hit something";
                    }
                else
                    {
                        cout << "missed";
                    }
                cout << ", resulting in:" << endl;
                
                b->display(t1->isHuman());
            }
         
         
            p1Turn = !p1Turn;
            

            if (b1.allShipsDestroyed() == true || b2.allShipsDestroyed() == true)
                {
                    break;
                }
           
            if (shouldPause == true)
                {
                    waitForEnter();
                }
        }
        
        
        if (b1.allShipsDestroyed())
            {
                t1 = p2;
            }
        else
            {
                t1 = p1;
            }
        
        cout << t1->name() << " wins!" << endl;
        return t1;
}

//******************** Game functions *******************************

// These functions for the most part simply delegate to GameImpl's functions.
// You probably don't want to change any of the code from this point down.

Game::Game(int nRows, int nCols)
{
    if (nRows < 1  ||  nRows > MAXROWS)
    {
        cout << "Number of rows must be >= 1 and <= " << MAXROWS << endl;
        exit(1);
    }
    if (nCols < 1  ||  nCols > MAXCOLS)
    {
        cout << "Number of columns must be >= 1 and <= " << MAXCOLS << endl;
        exit(1);
    }
    m_impl = new GameImpl(nRows, nCols);
}

Game::~Game()
{
    delete m_impl;
}

int Game::rows() const
{
    return m_impl->rows();
}

int Game::cols() const
{
    return m_impl->cols();
}

bool Game::isValid(Point p) const
{
    return m_impl->isValid(p);
}

Point Game::randomPoint() const
{
    return m_impl->randomPoint();
}

bool Game::addShip(int length, char symbol, string name)
{
    if (length < 1)
    {
        cout << "Bad ship length " << length << "; it must be >= 1" << endl;
        return false;
    }
    if (length > rows()  &&  length > cols())
    {
        cout << "Bad ship length " << length << "; it won't fit on the board"
             << endl;
        return false;
    }
    if (!isascii(symbol)  ||  !isprint(symbol))
    {
        cout << "Unprintable character with decimal value " << symbol
             << " must not be used as a ship symbol" << endl;
        return false;
    }
    if (symbol == 'X'  ||  symbol == '.'  ||  symbol == 'o')
    {
        cout << "Character " << symbol << " must not be used as a ship symbol"
             << endl;
        return false;
    }
    int totalOfLengths = 0;
    for (int s = 0; s < nShips(); s++)
    {
        totalOfLengths += shipLength(s);
        if (shipSymbol(s) == symbol)
        {
            cout << "Ship symbol " << symbol
                 << " must not be used for more than one ship" << endl;
            return false;
        }
    }
    if (totalOfLengths + length > rows() * cols())
    {
        cout << "Board is too small to fit all ships" << endl;
        return false;
    }
    return m_impl->addShip(length, symbol, name);
}

int Game::nShips() const
{
    return m_impl->nShips();
}

int Game::shipLength(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipLength(shipId);
}

char Game::shipSymbol(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipSymbol(shipId);
}

string Game::shipName(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipName(shipId);
}

Player* Game::play(Player* p1, Player* p2, bool shouldPause)
{
    if (p1 == nullptr  ||  p2 == nullptr  ||  nShips() == 0)
        return nullptr;
    Board b1(*this);
    Board b2(*this);
    return m_impl->play(p1, p2, b1, b2, shouldPause);
}

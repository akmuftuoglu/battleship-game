//
//  Board.cpp
//  Project 3
//

#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

class BoardImpl
{
  public:
    BoardImpl(const Game& g);
    void clear();
    void block();
    void unblock();
    bool placeShip(Point topOrLeft, int shipId, Direction dir);
    bool unplaceShip(Point topOrLeft, int shipId, Direction dir);
    void display(bool shotsOnly) const;
    bool attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId);
    bool allShipsDestroyed() const;
    
    void blockTest();

  private:
    const Game& m_game;
    char boardGrid[MAXROWS][MAXCOLS];
    vector<int> placedShips;
    int destroyedShips = 0;
    //int placedShips = 0;
};

BoardImpl::BoardImpl(const Game& g) : m_game(g)
{
    clear();
}

void BoardImpl::clear() // also can make a new board
{
    for (int r = 0; r < m_game.rows(); r++)
    {
        for (int c = 0; c < m_game.cols(); c++)
        {
            boardGrid[r][c] = '.';
        }
    }
}

void BoardImpl::blockTest()
{
    boardGrid[0][0] = '#';
   /* boardGrid[0][1] = '#';
    boardGrid[0][2] = '#';
    boardGrid[1][0] = '#';
    boardGrid[1][2] = '#';
    boardGrid[1][3] = '#';
    boardGrid[2][0] = '#';
    boardGrid[2][2] = '#';
    boardGrid[3][0] = '#';
    boardGrid[3][2] = '#'; */
}




void BoardImpl::block()
{
      // Block cells with 50% probability
    int maxBlocks = (m_game.rows()* m_game.cols())/2;
    int blockedCells = 0;
    
        for (int r = 0; r < m_game.rows(); r++)
        {
            for (int c = 0; c < m_game.cols(); c++)
            {
                if (randInt(2) == 0 && blockedCells != maxBlocks)
                {
                    boardGrid[r][c] = '#';
                    blockedCells++;
                }
            }
        }
    
}

void BoardImpl::unblock()
{
    
    for (int r = 0; r < m_game.rows(); r++)
    {
        for (int c = 0; c < m_game.cols(); c++)
        {
            if (boardGrid[r][c] == '#')
            {
                boardGrid[r][c] = '.';
            }
        }
    }
}




bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    
    int startRow = topOrLeft.r;
    int startCol = topOrLeft.c;
    
    if (shipId >= m_game.nShips() || shipId < 0)
    {
        return false;
    }
    
    if (m_game.shipLength(shipId) > m_game.rows() || m_game.shipLength(shipId) > m_game.cols())
    {
        return false;
    }
    
    if (dir == VERTICAL)
    {
        if (startRow + m_game.shipLength(shipId) > MAXROWS)
        {
            return false;
        }
    }
    
    if (dir == HORIZONTAL)
    {
        if (startCol + m_game.shipLength(shipId) > MAXCOLS)
        {
            return false;
        }
    }
    
    if (dir == VERTICAL)
    {
        for (int r = startRow; r < startRow + m_game.shipLength(shipId); r++)
        {
            if (boardGrid[r][startCol] != '.')
            {
                return false;
            }
        }
    }
    
    
    if (dir == HORIZONTAL)
    {
        for (int c = startCol; c < startCol + m_game.shipLength(shipId); c++)
        {
            if (boardGrid[startRow][c] != '.')
            {
                return false;
            }
        }
    }
    
    
    
    if (placedShips.size() != 0)
    {
        for (int i = 0; i < placedShips.size(); i++)
        {
            if (placedShips[i] == shipId)
            {
                return false;
            }
        }
    }
    
    if (dir == VERTICAL)
    {
        for (int r = startRow; r < startRow + m_game.shipLength(shipId); r++)
        {
            boardGrid[r][startCol] = m_game.shipSymbol(shipId);
        }
    }
    
    if (dir == HORIZONTAL)
    {
        for (int c = startCol; c < startCol + m_game.shipLength(shipId); c++)
        {
            boardGrid[startRow][c] = m_game.shipSymbol(shipId);
        }
    }
    
    
    placedShips.push_back(shipId);
    
    return true;
}

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    int startRow = topOrLeft.r;
    int startCol = topOrLeft.c;
    
    if (shipId >= m_game.nShips() || shipId < 0)
    {
        return false;
    }
    
    if (dir == VERTICAL)
    {
        for (int r = startRow; r < startRow + m_game.shipLength(shipId); r++)
        {
            if (boardGrid[r][startCol] != m_game.shipSymbol(shipId))
            {
                return false;
            }
        }
    }
    
    if (dir == HORIZONTAL)
    {
        for (int c = startCol; c < startCol + m_game.shipLength(shipId); c++)
        {
           if  (boardGrid[startRow][c] != m_game.shipSymbol(shipId))
           {
               return false;
           }
        }
    }
    
    if (dir == VERTICAL)
    {
        for (int r = startRow; r < startRow + m_game.shipLength(shipId); r++)
        {
            boardGrid[r][startCol] = '.';
        }
    }
    
    if (dir == HORIZONTAL)
    {
        for (int c = startCol; c < startCol + m_game.shipLength(shipId); c++)
        {
            boardGrid[startRow][c] = '.';
        }
    }
    
    
    return true;
}

void BoardImpl::display(bool shotsOnly) const   
{
    cout << "  ";
    
    for (int i = 0; i < m_game.cols(); i++)
    {
        cout << i;
    }
    
    cout << endl;
    
    for (int r = 0; r < m_game.rows(); r++)
    {
        cout << r << " ";
        
        for (int c = 0; c < m_game.cols(); c++)
        {
            if (shotsOnly == true)
            {
                if (boardGrid[r][c] == '.' || boardGrid[r][c] == 'o' || boardGrid[r][c] == 'X')
                {
                    cout << boardGrid[r][c];
                }
                else
                {
                    cout << '.';
                }
            }
            else
            {
                cout << boardGrid[r][c];
            }
        }
        
        cout << endl;
    }
}

bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    char symbolHit = boardGrid[p.r][p.c];
    
    if (p.r > m_game.rows() || p.c > m_game.cols())
    {
        shotHit = false;
        shipDestroyed = false;
        return false;
    }
    
    if (boardGrid[p.r][p.c] != '.' && boardGrid[p.r][p.c] != 'o')
    {
        shotHit = true;
        boardGrid[p.r][p.c] = 'X';
        
        for (int i = 0; i < m_game.nShips(); i++)
        {
            if (m_game.shipSymbol(i) == symbolHit)
            {
                shipId = i;
            }
        }
    }
    else
    {
        shotHit = false;
        boardGrid[p.r][p.c] = 'o';
    }
    
    
    for (int r = 0; r < m_game.rows(); r++)
    {
        for (int c = 0; c < m_game.cols(); c++)
        {
            if (boardGrid[r][c] == symbolHit)
            {
                shipDestroyed = false;
                return true;
            }
        }
        
    }
    
    shipDestroyed = true;
    destroyedShips++;
    
    return true;
}

bool BoardImpl::allShipsDestroyed() const
{
    if (destroyedShips == placedShips.size())
    {
        return true;
    }
    
    return false;
}

//******************** Board functions ********************************

// These functions simply delegate to BoardImpl's functions.
// You probably don't want to change any of this code.

Board::Board(const Game& g)
{
    m_impl = new BoardImpl(g);
}

Board::~Board()
{
    delete m_impl;
}

void Board::clear()
{
    m_impl->clear();
}

void Board::block()
{
    return m_impl->block();
}

void Board::blockTest()             // DELETE THIS!!!
{
    return m_impl->blockTest();
}

void Board::unblock()
{
    return m_impl->unblock();
}

bool Board::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->placeShip(topOrLeft, shipId, dir);
}

bool Board::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->unplaceShip(topOrLeft, shipId, dir);
}

void Board::display(bool shotsOnly) const
{
    m_impl->display(shotsOnly);
}

bool Board::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    return m_impl->attack(p, shotHit, shipDestroyed, shipId);
}

bool Board::allShipsDestroyed() const
{
    return m_impl->allShipsDestroyed();
}


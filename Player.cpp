//
//  Player.cpp
//  Project 3
//


#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>

using namespace std;

//*********************************************************************
//  AwfulPlayer
//*********************************************************************

class AwfulPlayer : public Player
{
  public:
    AwfulPlayer(string nm, const Game& g);
    
    virtual bool isHuman() const { return false; }
    
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
  private:
    Point m_lastCellAttacked;
};

AwfulPlayer::AwfulPlayer(string nm, const Game& g) : Player(nm, g), m_lastCellAttacked(0, 0) {}

bool AwfulPlayer::placeShips(Board& b)
{
      // Clustering ships is bad strategy
    for (int k = 0; k < game().nShips(); k++)
        if ( ! b.placeShip(Point(k,0), k, HORIZONTAL))
            return false;
    return true;
}

Point AwfulPlayer::recommendAttack()
{
    if (m_lastCellAttacked.c > 0)
        m_lastCellAttacked.c--;
    else
    {
        m_lastCellAttacked.c = game().cols() - 1;
        if (m_lastCellAttacked.r > 0)
            m_lastCellAttacked.r--;
        else
            m_lastCellAttacked.r = game().rows() - 1;
    }
    return m_lastCellAttacked;
}

void AwfulPlayer::recordAttackResult(Point /* p */, bool /* validShot */, bool /* shotHit */, bool /* shipDestroyed */, int /* shipId */)
{
      // AwfulPlayer completely ignores the result of any attack
}

void AwfulPlayer::recordAttackByOpponent(Point /* p */)
{
      // AwfulPlayer completely ignores what the opponent does
}

//*********************************************************************
//  HumanPlayer
//*********************************************************************

bool getLineWithTwoIntegers(int& r, int& c)
{
    bool result(cin >> r >> c);
    if (!result)
        cin.clear();  // clear error state so can do more input operations
    cin.ignore(10000, '\n');
    return result;
}


class HumanPlayer : public Player
{
  public:
    HumanPlayer(string nm, const Game& g);
    
    virtual bool isHuman() const { return true; }
    
    virtual bool placeShips(Board& b);  // need to check with play function
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
  private:
    Point m_lastCellAttacked;
};


HumanPlayer::HumanPlayer(string nm, const Game& g) : Player(nm, g), m_lastCellAttacked(0, 0) {}

bool HumanPlayer::placeShips(Board &b)
{
    for (int i = 0; i < game().nShips(); i++)
    {
        int row;
        int col;
        char direction;
        
        
        cout << "Enter h or v for direction" << game().shipName(i) << "(length" << game().shipLength(i) << "):";
        cin >> direction;
    
        while (direction != 'h' && direction != 'v')
        {
            cout << "Direction must be h or v." << endl;
            cout << "Enter h or v for direction" << game().shipName(i) << "(length" << game().shipLength(i) << "):";
            cin >> direction;
        }
            
        cout << "Enter row and column of leftmost cell (e.g., 3 5)";
        getLineWithTwoIntegers(row, col);
        
        if (direction == 'h')
        {
            b.placeShip(Point(row,col), i, HORIZONTAL);
        }
    
        if (direction == 'v')
        {
            b.placeShip(Point(row,col), i, VERTICAL);
        }
        
        while (getLineWithTwoIntegers(row, col) == false)
        {
            cout << "You must enter two integers." << endl;
            cout << "Enter row and column of leftmost cell (e.g., 3 5)";
            getLineWithTwoIntegers(row, col);
        }
        
        while (game().isValid(Point(row,col)) == false)
        {
            cout << "The ship can not be placed there." << endl;
            cout << "Enter row and column of leftmost cell (e.g., 3 5)";
            getLineWithTwoIntegers(row, col);
        }
        
    }
        
    return true;
}

Point HumanPlayer::recommendAttack()
{
    int row;
    int col;
    
    cout << "Enter the row and column to attack (e.g., 3 5):";
    getLineWithTwoIntegers(row, col);
    
    while (getLineWithTwoIntegers(row, col) == false)
    {
        cout << "You must enter two integers." << endl;
        cout << "Enter the row and column to attack (e.g., 3 5):";
        getLineWithTwoIntegers(row, col);
    }
    
    m_lastCellAttacked.r = row;
    m_lastCellAttacked.c = col;
    
    return m_lastCellAttacked;
}

void HumanPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{
    // does nothing
}


void HumanPlayer::recordAttackByOpponent(Point p)
{
    //does nothing
}


//*********************************************************************
//  MediocrePlayer
//*********************************************************************


class MediocrePlayer : public Player
{
  public:
    MediocrePlayer(string nm, const Game& g);
    
    virtual bool isHuman() const { return false; }
    
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
  private:
    bool placeFunc(Point p, Board& b, int shipId, bool whenToBackTrack);
    vector<Point> attackHistory;
    Point m_lastCellAttacked;
    int state;
    vector<Point> stateTransition;
    
};


MediocrePlayer::MediocrePlayer(string nm, const Game& g) : Player(nm, g), m_lastCellAttacked(0, 0), state(1) {}



bool MediocrePlayer::placeFunc(Point p, Board &b, int shipId, bool whenToBackTrack)
{
                                    // to see how the shipId changes
   
 if (whenToBackTrack == false)
 {
     //cout << "Trying to place shipId:" << shipId << " at Point: (" << p.r << "," << p.c << ")" << endl;
     
    if (shipId == game().nShips())  // we reached a shipId that means we placed all the ships
    {
        return true;
    }
    
    
    if (p.r == game().rows()-1 && p.c == game().cols()-1)       // last point
    {
        if (shipId < game().nShips() && shipId != 0)   // when we reach the end we check if we have to place a ship that is not the last one
        {
            whenToBackTrack = true;                    // will replace this with back track funciton once placeFunc is fixed
            //cout << "BackTrack needed" << endl;
            //cout << shipId << endl;
            placeFunc(p,b,shipId-=1,whenToBackTrack);          // time to backtrack
            return false;
        }
        
        if (shipId == 0)                        // we were unable to place any ships
        {
            return false;
        }
    }
    
    
    if (p.c == game().cols()-1 && p.r != game().rows()-1)   // we reached the last column of a row that is not last
    {
        if (b.placeShip(p,shipId,HORIZONTAL) == true)   // if the length is one, it can be placed horizontally which is the same as vertically (so check it horizontally first)
        {
            p.c = 0;
            p.r = 0;
            shipId+=1;
            placeFunc(p,b,shipId,whenToBackTrack);
            return true;
        }
        
        if (b.placeShip(p,shipId,VERTICAL) == true)
        {
            p.c = 0;
            p.r = 0;
            shipId+=1;
            placeFunc(p,b,shipId,whenToBackTrack);
            return true;
        }
            
            p.c = 0;
            p.r+=1;
            placeFunc(p,b,shipId,whenToBackTrack);
            return false;
    }
    

    
    if (p.r == game().rows()-1 && p.c == game().cols()-1)   // if it reaches the last (aka most bottom right point) and have not placed all the ships
    {
        if (b.placeShip(p,shipId,VERTICAL) == true)         // if the length is one, then if you can place it vertically, you can place it horizontally
        {
            p.c = 0;
            p.r = 0;
            shipId += 1;
            placeFunc(p,b,shipId,whenToBackTrack);                        // we will visit one more shipId to check the condition
            return true;
        }
    }
    
    
    if (b.placeShip(p,shipId,VERTICAL) == true)        // if it can be placed vertically, it will skip all of this
    {
        p.c = 0;
        p.r = 0;
        shipId += 1;
        placeFunc(p,b,shipId,whenToBackTrack);
        return true;
    }
    else if (b.placeShip(p,shipId,HORIZONTAL) == true)  // if it cannot be placed vertically, check if it can be placed horizontally
    {
        p.c = 0;
        p.r = 0;
        shipId += 1;
        placeFunc(p,b,shipId,whenToBackTrack);
        return true;
    }
    else                                            // if not (so cannot be placed at that point) move to next point and call function again
    {
        p.c+=1;
        placeFunc(p,b,shipId,whenToBackTrack);
        return false;
    }
 }

else
    {
        //cout << "Backtracking for shipId:" << shipId << endl;
        if (p.c == 0 && p.r != 0)
        {
            if (b.unplaceShip(p,shipId,VERTICAL) == true)
            {
                whenToBackTrack = false;
                
                if (b.placeShip(p,shipId, HORIZONTAL) == true)
                {
                    
                    placeFunc(p,b,shipId+=1,whenToBackTrack);
                    return true;
                }
                else
                {
                    
                    placeFunc(p,b,shipId,whenToBackTrack);
                    return true;
                    
                }
            }
            else if (b.unplaceShip(p, shipId, HORIZONTAL) == true)
            {
                whenToBackTrack = false;
                placeFunc(p,b,shipId,whenToBackTrack);
                return true;
            }
            else
            {
                p.c=game().cols()-1;
                p.r-=1;
                placeFunc(p,b,shipId,whenToBackTrack);
                return true;
            }
        }
         
        if (b.unplaceShip(p,shipId,VERTICAL) == true)
        {
            if (b.placeShip(p,shipId, HORIZONTAL) == true)
            {
                
                whenToBackTrack = false;
                placeFunc(p,b,shipId+=1,whenToBackTrack);
                return true;
            }
            else
            {
                
                whenToBackTrack = false;
                placeFunc(p,b,shipId,whenToBackTrack);
                return true;
                
            }
        }
        else if (b.unplaceShip(p, shipId, HORIZONTAL) == true)
        {
            whenToBackTrack = false;
            placeFunc(p,b,shipId,whenToBackTrack);
            return true;
        }
        else
        {
            
            p.c-=1;
            placeFunc(p,b,shipId,whenToBackTrack);
            return true;
        }
    }

    return true;   // this will never be visited
}



bool MediocrePlayer::placeShips(Board& b)
{
    Point p(0,0);
    return placeFunc(p, b, 0, false);
    
}



// Remember that Mediocre::placeShips(Board& b) must start by calling
// b.block(), and must call b.unblock() just before returning.



Point MediocrePlayer::recommendAttack()
{
    //state one
    if (state == 1 /* || SixOrMore ==  true */)
    {
        int row = randInt(game().rows());
        int col = randInt(game().rows());
        
        m_lastCellAttacked.r = row;
        m_lastCellAttacked.c = col;
    
    
        for (int i = 0; i < attackHistory.size(); i++)
        {
            if (attackHistory[i].r == m_lastCellAttacked.r && attackHistory[i].c == m_lastCellAttacked.c)
            {
                recommendAttack();
            }
        }
        
    }
    else // state 2
    {
        int maxRow = stateTransition[stateTransition.size()-1].r;
        int minRow = stateTransition[stateTransition.size()-1].r;
        
        int maxCol = stateTransition[stateTransition.size()-1].c;
        int minCol = stateTransition[stateTransition.size()-1].c;
        
        int counter = 0;
        while (maxRow != game().rows() && counter < 4)
        {
            maxRow+=1;
            counter+=1;
        }
        
        counter = 0;
        while (minRow != 0 && counter < 4)
        {
            minRow-=1;
            counter+=1;
        }
        
        counter = 0;
        while (maxCol != game().cols() && counter < 4)
        {
            maxCol+=1;
            counter+=1;
        }
        
        counter = 0;
        while (minCol != 0 && counter < 4)
        {
            minCol-=1;
            counter+=1;
        }
        
        
        int randRow = rand() % maxRow + minRow;
        int randCol = rand() % maxCol + minCol;
        
        for (int i = 0; i < attackHistory.size(); i++)
        {
            if (attackHistory[i].r == randRow && attackHistory[i].c == randCol)
            {
                recommendAttack();
            }
        }
        
        m_lastCellAttacked.r = randRow;
        m_lastCellAttacked.c = randCol;
        
    }

    
    attackHistory.push_back(m_lastCellAttacked);
    

    return m_lastCellAttacked;
}




void MediocrePlayer::recordAttackByOpponent(Point p)
{
    // does nothing
}


void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{
    if (state == 1 && shotHit == false)
    {
        state = 1;
    }
    
    if (state == 1 && shipDestroyed == true)
    {
        state = 1;
    }
    
    if (state == 1 && shotHit == true && shipDestroyed == false)
    {
        stateTransition.push_back(p);
        state = 2;
    }
    
    if (state == 2 && shotHit == false)
    {
        state = 2;
    }
    
    if (state == 2 && shotHit == true && shipDestroyed == false)
    {
        state = 2;
    }
    
    if (state == 2 && shotHit == true && shipDestroyed == true)
    {
        state = 1;
    }
}


//*********************************************************************
//  GoodPlayer
//*********************************************************************



class GoodPlayer : public Player
{
  public:
    GoodPlayer(string nm, const Game& g);
    
    virtual bool isHuman() const { return false; }
    
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
  private:
    Point m_lastCellAttacked;
    vector<Point> attackHistory;
    int state;
    vector<Point> stateTransition;
};




GoodPlayer::GoodPlayer(string nm, const Game& g) : Player(nm, g), m_lastCellAttacked(0, 0) {}


bool GoodPlayer::placeShips(Board &b) {
    
    Point toPlace(0,0);
    
    for (int i = 0; i < game().nShips(); i++)
    {
        toPlace.r = randInt(game().rows());
        toPlace.c = randInt(game().cols());
        
        if (b.placeShip(toPlace, i, VERTICAL) == true)
        {
            //places vertically
        }
        else if (b.placeShip(toPlace, i, HORIZONTAL) == true)
        {
            //passes horizontally
        }
        else
        {
            i--;
        }
    }
    
    
    return true;
    
}

Point GoodPlayer::recommendAttack() {
    
    if (state == 1)
    {
        int row = randInt(game().rows());
        int col = randInt(game().cols());
        
        m_lastCellAttacked.r = row;
        m_lastCellAttacked.c = col;
    
    
        for (int i = 0; i < attackHistory.size(); i++)
        {
            if (attackHistory[i].r == m_lastCellAttacked.r && attackHistory[i].c == m_lastCellAttacked.c)
            {
                recommendAttack();
            }
        }
        
    }
    else // state 2
    {
        int maxRow = stateTransition[stateTransition.size()-1].r;
        int minRow = stateTransition[stateTransition.size()-1].r;
        
        int maxCol = stateTransition[stateTransition.size()-1].c;
        int minCol = stateTransition[stateTransition.size()-1].c;
        
        maxRow+=1;
        minRow-=1;
        maxCol+=1;
        minCol-=1;

        int randRow = rand() % maxRow + minRow;
        int randCol = rand() % maxCol + minCol;
        
        for (int i = 0; i < attackHistory.size(); i++)
        {
            if (attackHistory[i].r == randRow && attackHistory[i].c == randCol)
            {
                recommendAttack();
            }
        }
        
        m_lastCellAttacked.r = randRow;
        m_lastCellAttacked.c = randCol;
        
    }

    attackHistory.push_back(m_lastCellAttacked);
    
    return m_lastCellAttacked;
}

void GoodPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId) {
    
    if (state == 1 && shotHit == false)
    {
        state = 1;
    }
    
    if (state == 1 && shipDestroyed == true)
    {
        state = 1;
    }
    
    if (state == 1 && shotHit == true && shipDestroyed == false)
    {
        stateTransition.push_back(p);
        state = 2;
    }
    
    if (state == 2 && shotHit == false)
    {
        state = 2;
    }
    
    if (state == 2 && shotHit == true && shipDestroyed == false)
    {
        state = 2;
    }
    
    if (state == 2 && shotHit == true && shipDestroyed == true)
    {
        state = 1;
    }
}

void GoodPlayer::recordAttackByOpponent(Point p) { /* does nothing */ }





//*********************************************************************
//  createPlayer
//*********************************************************************

Player* createPlayer(string type, string nm, const Game& g)
{
    static string types[] = {
        "human", "awful", "mediocre", "good"
    };
    
    int pos;
    for (pos = 0; pos != sizeof(types)/sizeof(types[0])  &&
    type != types[pos]; pos++)
        ;
    switch (pos)
    {
      case 0:  return new HumanPlayer(nm, g);
      case 1:  return new AwfulPlayer(nm, g);
      case 2:  return new MediocrePlayer(nm, g);
      case 3:  return new GoodPlayer(nm, g);
      default: return nullptr;
    }
}


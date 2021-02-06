#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath),m_nBacteria(0),m_score(0){}

string StudentWorld::getStatus()
//sets scoreboard
{
    ostringstream status;
    status.fill('0');
    int score = getScore();
    status << "Score: " << setw(6) << score << "  ";
    int level = getLevel();
    status << "Level: " << level << "  ";
    int lives = getLives();
    status << "Lives: " << lives << "  ";
    int health = m_socrates->getHealth();
    status << "Health: " << health << "  ";
    int sprays = m_socrates->getSprays();
    status << "Sprays: " << sprays << "  ";
    int flames = m_socrates->addFlamethrower(0);
    //addFlamethrower returns number of flames
    status << "Flames: " << flames;
    string s = status.str();
    return s;
}

 void StudentWorld::getRandomPos(double minOverlap, int maxR, double& x, double& y)
{
    //use rand int to find random position within circle of radius 120
    //uses equation x^2 + y^2 = radius
    x = randInt(0,maxR);
    y = randInt(0,sqrt(pow(maxR,2)-pow(x,2)));
    double xCenter = VIEW_WIDTH/2;
    double yCenter = VIEW_HEIGHT/2;
    //uses rand int to determine positive or negative
    int y1 = randInt(0,1);
    int x1 = randInt(0,1);
    if(y1)
        y *= -1;
    if(x1)
        x *= -1;
    list<Actor*>::const_iterator it;
    //if there is no overlap requirement, return
    if (minOverlap <= 0)
        return;
    //otherwise, make sure that the random position doesn't overlap with existing actors
    for (it = m_actors.begin(); it != m_actors.end(); it++)
    {
        //since checkOverlap changes value of parameters, must save minOverlap in temp
        double d = minOverlap;
        //if it does overlap, get a new position
        if ((*it)->checkOverlap(x+xCenter, y+yCenter, minOverlap))
            getRandomPos(d,maxR,x,y);
    }
}

bool StudentWorld::checkLevelUp()
//check if all enemies are gone to level up
{
    list<Actor*>::const_iterator it;
    for (it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it)->isEnemy())
            //if there are any enemies left, return false
            return false;
    }
    return true;
}

void StudentWorld::getRandCirclePos(double& x, double& y)
{
    int angle = randInt(0, 2*M_PI);
    x = VIEW_RADIUS*cos(angle) + VIEW_HEIGHT/2;
    y = VIEW_RADIUS*sin(angle) + VIEW_HEIGHT/2;
}

int StudentWorld::init()
{
    int L = getLevel();
    double x, y;
    double xCenter = VIEW_WIDTH/2;
    double yCenter = VIEW_HEIGHT/2;
    //allocate dirt first since they are allowed to overlap with eachother
    int nDirt = max(180-20*L, 20);
    for (int i = 0; i < nDirt; i++)
    {
        getRandomPos(0, 120, x, y);
        Actor* temp = new DirtPile(x+xCenter,y+yCenter,this);
        m_actors.push_front(temp);
    }
    //allocate socrates
    m_socrates = new Socrates(this);
    //allocate pits
    int nPits = L;
    for (int i = 0; i < nPits; i++)
    {
        getRandomPos(2*SPRITE_RADIUS, 120, x, y);
        Actor* temp = new Pit(x+xCenter, y+yCenter,this);
        m_actors.push_front(temp);
    }
    
    //allocate food
    int nFood = min(5*L,25);
    for (int i = 0; i < nFood; i++)
    {
        getRandomPos(2*SPRITE_RADIUS, 120, x, y);
        Actor* temp = new Food(x+xCenter, y+yCenter, this);
        m_actors.push_front(temp);
    }
   
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    list<Actor*>::const_iterator it;
    //iterate through all actors and have them do something
    for (it = m_actors.begin(); it != m_actors.end(); it++)
            (*it)->doSomething();
    m_socrates->doSomething();

    //if socrates is dead, return that he died
    if (m_socrates->isDead())
    {
        decLives();
        delete m_socrates;
        return GWSTATUS_PLAYER_DIED;
    }

    //iterate through all actors and delete the dead ones
    it = m_actors.begin();
    while (it != m_actors.end())
    {
        if ((*it)->isDead())
        {
            delete *it;
            it = m_actors.erase(it);
        }
        else
            it++;
    }
    //add goodies
    addGoodies();
    setGameStatText(getStatus());
    if(checkLevelUp())
    {
        return GWSTATUS_FINISHED_LEVEL;
    }
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    //delete all dynamically allocated actors
    list<Actor*>::const_iterator it = m_actors.begin();
    while (it != m_actors.end())
    {
        delete *it;
        it = m_actors.erase(it);
    }
}

void StudentWorld::addGoodies()
{
    //add new fungus
       int L = getLevel();
       double x,y;
       int chanceFungus = max(510-L*10,200);
       //rand int is inclusive, spec requires upper end not inclusive
       int num = randInt(0, chanceFungus-1);
       if (num == 0)
       {
           getRandCirclePos(x, y);
           Actor* temp = new Fungus(256,128,this);
           m_actors.push_back(temp);
       }
       int chanceGoodie = max(510-L*10,250);
       //rand int is inclusive, spec requires upper end not inclusive
       num = randInt(0, chanceGoodie-1);
       if (num == 0)
       {
           //get random position around circumference
           getRandCirclePos(x, y);
           //randomly choose which goodie will be spawned
           num = randInt(0, 10);
           //spawn goodies, and add to list of actors
           //get random position around circumference
           //in radians since cos and sin functions in radians
           if (num <= 6)
           {
               Actor* temp = new HealthGoodie(x,y,this);
               m_actors.push_front(temp);
           }
           else if (num <= 9)
           //since it's an else if statement, don't need to check if number is > 6
           {
               Actor* temp = new FlameGoodie(x,y,this);
               m_actors.push_front(temp);
           }
           else if (num == 10)
           {
               Actor* temp = new LifeGoodie(x,y,this);
               m_actors.push_front(temp);
           }
       }
}

bool StudentWorld::hasDamageOverlap(Actor* a, double dMin, const int damage)
//return true if there is overlap
{
    list<Actor*>::const_iterator it;
    Actor* current;
    //iterate through all of the actors
    for (it = m_actors.begin(); it != m_actors.end(); it++)
    {
        current = *it;
        //if they overlap within distance dMin of x and y from parameters
        if(current->checkOverlap(a->getX(), a->getY(), dMin)
           && current->isDamageableObject() && current->getHealth() > 0) // FIXED
        {
            //damage them
            current->takeDamage(damage);
            //return, only damage one
            return true;
        }
    }
    return false;
}

bool StudentWorld::checkSocratesOverlap(const double x, const double y, double dMin) const
{
    return m_socrates->checkOverlap(x, y, dMin);
}

void StudentWorld::findSocrates(double& x, double& y)
//sets x and y to position of socrates
{
    x = m_socrates->getX();
    y = m_socrates->getY();
}

void StudentWorld::damageSocrates(int damage)
//if damage is 0, restore health
//otherwise, decrement socrates' health by int damage
//if damage is negative, it restores health
{
    if (m_socrates->getHealth() - damage >= 100)
        //socrates health cannot exceed 100 when restoring healtDa
    {
        int health = m_socrates->getHealth();
        m_socrates->takeDamage(-(100-health));
    }
    else
        //otherwise, damage socrates
        m_socrates->takeDamage(damage);
}

bool StudentWorld::hasFoodOverlap(Actor* a)
//check for food overlap
{
    bool isFood = false;
    list<Actor*>::const_iterator it = m_actors.begin();
    //iterate through list
   for (; it != m_actors.end(); it++)
   {
        Actor* temp = (*it);
        //an actor is a food if it is not active (is still) and not damageable
        isFood = (!temp->isActiveActor() && !temp->isDamageableObject());
        //check for overlap
        if(temp->checkOverlap(a->getX(),a->getY(), 2*SPRITE_RADIUS) && isFood)
        //if it overlaps and is food, eat it (set food to not alive)
       {
            temp->takeDamage(1);
            return true;
       }
    }
    return false;
}



int StudentWorld::computeDistance(double x,double y, Actor *b)
{
    int xdis = x - b->getX();
    int ydis = y - b->getY();
    int distance = sqrt(pow(xdis,2) + pow(ydis,2));
    
    return distance;
}

bool StudentWorld::hasBlockingOverlap(double x, double y)
//return true if dirt is within dMin of position parameters
{
    list<Actor*>::const_iterator it;
    Actor* current;
    //iterate through all of the actors
    for (it = m_actors.begin(); it != m_actors.end(); it++)
    {
        bool isDirtOverlap = false;
        current = *it;
        //position overlaps with dirt if it is dirt and if it overlaps
        //blocksObjects() returns true for dirt only
        isDirtOverlap = current->blocksObjects() && current->checkOverlap(x, y, SPRITE_RADIUS);
        if(isDirtOverlap)
            return true;
    }
    //return false otherwise
    return false;
}

void StudentWorld::giveFlameGoodie(int num)
{
    m_socrates->addFlamethrower(num);
}

bool StudentWorld::findFood(Actor* a, double &x, double &y)
//sets x and y to position of closest food, if there is one within 128 pixels
{
    list<Actor*>::const_iterator it = m_actors.begin();
    int dOld = VIEW_RADIUS*2;
    Actor* old = nullptr;
    int dNew;
    bool isFood = false;
    for (; it != m_actors.end(); it++)
    {
        isFood = (!(*it)->isActiveActor() && !(*it)->isDamageableObject() && !(*it)->blocksObjects());
        if (isFood && (*it)->checkOverlap(a->getX(), a->getY(), 128))
        {
            dNew = computeDistance(a->getX(), a->getY(), *it);
            if (dNew < dOld)
            {
                dOld = dNew;
                old = *it;
                x = (*it)->getX();
                y = (*it)->getY();
            }
        }
    }
    if (old == nullptr)
        //if there was not any food within 128 pixels old will still be nullptr
        //return false, did not find any food
        return false;
    return true;
}

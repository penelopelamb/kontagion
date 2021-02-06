#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"

#include <string>
#include <list>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class Socrates;
class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    void addToList(Actor *a)
    {
        m_actors.push_back(a);
    }
    bool hasDamageOverlap(Actor* a, double dMin, const int damage);
    bool checkSocratesOverlap(const double x, const double y, double dMin) const;
    bool hasFoodOverlap(Actor* a);
    bool hasBlockingOverlap(double x, double y);
    void damageSocrates(int damage);
    void giveFlameGoodie(int num);
    bool findFood(Actor* a, double& x, double& y);
    void findSocrates(double& x, double& y);
    ~StudentWorld()
    {
        cleanUp();
    }
private:
    int computeDistance(double x, double y, Actor* b);
    void getRandomPos(double minOverlap, int maxR, double& x, double& y);
    void getRandCirclePos(double& x, double& y);
    bool checkLevelUp();
    void addGoodies();
    std::string getStatus();
    std::list<Actor*> m_actors;
    Socrates* m_socrates;
    int m_nBacteria;
    int m_score;    
};

#endif // STUDENTWORLD_H_

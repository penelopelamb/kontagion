#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"
#include <algorithm>

class Actor : public GraphObject
{
public:
    Actor(int imageID, double x, double y, Direction dir, int depth, int health, const int deathSound, const int damageSound, StudentWorld* sp)
        :GraphObject(imageID, x, y, dir, depth), m_x(x), m_y(y), m_health(health), m_deathSound(deathSound),m_damageSound(damageSound), m_StudentWorld(sp){}
    virtual void doSomething() = 0;
    virtual bool isDamageableObject() const = 0;
    virtual bool isEnemy() const
    {
        return false;
        //most actors aren't enemies
    }
    virtual bool blocksObjects() const
    {
        return false;
        //most actors do not block objects
        //however, since some do (ie dirt), it must be virtual function
    }
    virtual bool hasDeathSound() const
    {
        return true;
        //most actors make a sound when they die
    }
    virtual bool hasDamageSound() const
    {
        return isActiveActor() && isDamageableObject();
        //most active actors make sounds when they are damaged
    }
    virtual bool isActiveActor() const = 0;
    bool checkOverlap(const double otherX, const double otherY, double distance);
    void takeDamage(int damage);
    bool isDead()
    {
        return m_health <= 0;
    }
    int getHealth()
    {
        return m_health;
    }
    StudentWorld* getWorld() const
    {
        return m_StudentWorld;
    }
    virtual ~Actor(){};
private:
    int m_health;
    int m_deathSound;
    int m_damageSound;
    int m_x;
    int m_y;
    StudentWorld* m_StudentWorld;
};

class Socrates : public Actor
{
public:
    Socrates(StudentWorld* sp)
        :Actor(IID_PLAYER, 0, VIEW_HEIGHT/2, 0, 0, 100, SOUND_SALMONELLA_DIE, SOUND_SALMONELLA_HURT, sp), m_nFlamethrowers(5), m_nSpray(20){}
    virtual void doSomething();
    virtual bool isDamageableObject() const
    {
        return false;
        //socrates is not an object
    }
    int addFlamethrower(int num)
    {
        m_nFlamethrowers += num;
        return m_nFlamethrowers;
    }
    int getSprays()
    {
        return m_nSpray;
    }
    virtual bool isActiveActor() const
    {
        return true;
        //socrates can move
    }
    virtual ~Socrates(){}
private:
    int m_nFlamethrowers;
    int m_nSpray;
};

class StillActor : public Actor
{
public:
    StillActor(int imageID, double x, double y, int dir, StudentWorld* sp)
        :Actor(imageID, x, y, dir, 1,1, 0,0, sp) {}
   void doSomething()
    {
        return;
        //still actors don't do anything
    }
    virtual bool hasDeathSound() const
    {
        //still actors don't make any sounds when they die
        return false;
    }
    bool isActiveActor() const
    {
        return false;
    }
    virtual bool isDamageableObject() const = 0;
    virtual ~StillActor(){}
};

class DirtPile : public StillActor
{
public:
    DirtPile(double x, double y, StudentWorld* sp)
        :StillActor(IID_DIRT, x, y, 0, sp) {}
    virtual bool isDamageableObject() const
    {
        return true;
        //dirt pile is damageable
    }
    virtual bool blocksObjects() const
    {
        return true;
        //dirt pile blocks objects
    }
    virtual ~DirtPile(){}
};

class Food : public StillActor
{
public:
    Food(double x, double y, StudentWorld* sp)
        :StillActor(IID_FOOD, x, y, 90, sp) {}
    virtual bool isDamageableObject() const
    {
        return false;
        //food is not damageable
    }
    virtual ~Food(){}
};

class Projectile : public Actor
{
public:
    Projectile(int imageID, double x, double y, int dir, StudentWorld* sp, int dMax, int damage)
        :Actor(imageID, x, y, dir, 1,1,0,0, sp), m_maxdis(dMax), m_damage(damage), m_distance(0){}
    virtual void doSomething();
    //even though no projectile is damageable, function made pure virtual to prevent against instantiation
    //this makes the class abstract
    virtual bool isDamageableObject() const = 0;
    bool isActiveActor() const
    {
        return true;
        //projectiles can move
    }
    virtual bool hasDeathSound() const
    {
        return false;
        //projectiles don't make a sound when they are removed
    }
    virtual ~Projectile(){}
private:
    int m_maxdis;
    int m_damage;
    int m_distance;
};

class Flame : public Projectile
{
public:
    Flame(double x, double y, int dir, StudentWorld* sp)
        :Projectile(IID_FLAME,x,y,dir,sp,32,5){}
    virtual bool isDamageableObject() const
    {
        return false;
    }
    virtual ~Flame(){}
};

class Spray : public Projectile
{
public:
    Spray(double x, double y, int dir, StudentWorld* sp)
        :Projectile(IID_SPRAY,x,y,dir,sp,112,2){}
    virtual bool isDamageableObject() const
    {
        return false;
    }
    virtual ~Spray(){}
};

class Goodie : public Actor
{
public:
    Goodie(int imageID, double x, double y, int points, StudentWorld* sp)
    :Actor(imageID,x,y,0,1,std::max(rand()%(300-10*(sp->getLevel())),50),SOUND_GOT_GOODIE,SOUND_GOT_GOODIE,sp), m_points(points){}
    virtual void doSomething();
    virtual bool isDamageableObject() const
    {
        return true;
        //goodies are damageable
    }
    virtual void restore() = 0;
    virtual bool isActiveActor() const
    {
        return false;
        //goodies do not move
    }
    virtual ~Goodie(){}
private:
    int m_points;
};

class HealthGoodie : public Goodie
{
public:
    HealthGoodie(double x, double y, StudentWorld* sp)
        :Goodie(IID_RESTORE_HEALTH_GOODIE, x, y, 250, sp){}
    virtual void restore()
    {
        //damaging by negative value is same as restoring health
        //function only allows socrates' health to be 100 at max, though
        getWorld()->damageSocrates(-999);
    }
    virtual ~HealthGoodie(){}
};

class FlameGoodie : public Goodie
{
public:
    FlameGoodie(double x, double y, StudentWorld *sp)
        :Goodie(IID_FLAME_THROWER_GOODIE, x, y, 300, sp){}
    virtual void restore()
    {
        getWorld()->giveFlameGoodie(5);
    }
    virtual ~FlameGoodie(){}
};

class LifeGoodie : public Goodie
{
public:
    LifeGoodie(double x, double y, StudentWorld* sp)
        :Goodie(IID_EXTRA_LIFE_GOODIE, x, y, 500, sp){}
    virtual void restore()
    {
        getWorld()->incLives();
    }
    virtual ~LifeGoodie(){}
};

class Fungus : public Goodie
{
public:
    Fungus(double x, double y, StudentWorld *sp)
        :Goodie(IID_FUNGUS, x, y, -50, sp){}
    virtual void restore()
    {
        getWorld()->damageSocrates(20);
    }
    virtual bool hasDeathSound() const
    {
        return false;
    }
    virtual ~Fungus(){}
};

class Pit : public Actor
{
public:
    Pit(double x, double y, StudentWorld* sp)
        :Actor(IID_PIT,x,y,0,1,1,0,0,sp),
        m_nSalmonella(5),m_nAggSalmonella(3),m_nEColi(2){}
    virtual void doSomething();
    virtual bool hasDeathSound() const
    {
        return false;
    }
    virtual bool isEnemy() const
    {
        return true;
    }
    virtual bool isDamageableObject() const
    {
        return false;
    }
    virtual bool isActiveActor() const
    {
        //it is active since it spawns bacteria
        return true;
    }
    virtual ~Pit(){}
private:
    bool chooseRandomBacteria();
    int m_nSalmonella;
    int m_nAggSalmonella;
    int m_nEColi;
};

class Bacteria : public Actor
{
public:
    Bacteria(int imageID, double x, double y, int health, int damage, const int movement, const int deathSound, const int damageSound, StudentWorld* sp)
        :Actor(imageID,x,y,90,0,health, deathSound, damageSound, sp),m_plan(0), m_food(0), m_damage(damage),m_movement(movement){}
    virtual void doSomething();
    virtual void spawnNew() = 0;
    virtual bool isEnemy() const
    {
        return true;
    }
    void getNewCoord(double& x, double& y);
    virtual bool isDamageableObject() const
    {
        //all bacterias can be damaged
        return true;
    }
    virtual bool isAggressive() const
    {
        //most bacterias are not aggressive
        return false;
    }
    virtual bool hasMPlan() const
    {
        return true;
        //most bacterias have movement plan
    }
    virtual bool isActiveActor() const
    {
        return true;
        //bacteria move
    }
    virtual bool isFoodChaser() const
    {
        //most bacteria chase food, only regular ecoli dont
        return true;
    }
    virtual ~Bacteria(){}
private:
    bool checkPath(double x, double y, double dir);
    int getAngleToChase(double xNew, double yNew);
    bool isWithinDish(double x, double y);
    int m_damage;
    int m_plan;
    int m_food;
    int m_movement;
};

class Salmonella : public Bacteria
{
public:
  Salmonella(double x, double y, StudentWorld* sp)
    :Bacteria(IID_SALMONELLA, x,y,4,1,3,SOUND_SALMONELLA_DIE,SOUND_SALMONELLA_HURT,sp){}
    virtual void spawnNew();
    virtual ~Salmonella(){}
};

class AggSalmonella : public Bacteria
{
public:
  AggSalmonella(double x, double y, StudentWorld* sp)
    :Bacteria(IID_SALMONELLA,x,y,10,2,3,SOUND_SALMONELLA_DIE,SOUND_SALMONELLA_HURT,sp){}
    virtual void spawnNew();
    virtual bool isAggressive() const
    {
        return true;
    }
    virtual ~AggSalmonella(){}
};

class EColi : public Bacteria
{
public:
  EColi(double x, double y, StudentWorld* sp)
    :Bacteria(IID_ECOLI,x,y,5,4,2,SOUND_ECOLI_DIE,SOUND_ECOLI_HURT,sp){}
    virtual void spawnNew();
    virtual bool isFoodChaser() const
    {
        return false;
    }
    virtual bool hasMPlan() const
    {
        return false;
    }
    virtual ~EColi(){}
};





// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

#endif // ACTOR_H_

#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>
using namespace std;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
bool Actor::checkOverlap(const double otherX, const double otherY, double minimum)
//returns true if this actor is within minimum of other actor
{
    //computes the distance between two actors (or an actor and another position)
    double xdis = otherX - getX();
    double ydis = otherY - getY();
    double distance = sqrt(pow(xdis,2) + pow(ydis,2));
    //check if distance is within minimum
    if (distance <= minimum)
        return true;
    return false;
}

void Actor::takeDamage(int damage)
{
    m_health -= damage;
    //lower health by the parameter damage
    
    if (damage < 0 || !isActiveActor())
        //if damage has positive effect (not a goodie restoring health)
        //only active actors have sounds so if it isn't active, it should return
        return;
    
    
    //play sounds accordingly
    if (m_health <= 0 && hasDeathSound())
        //if the health is 0 or less (actor is dead) and the actor plays a sound when it dies, play it
    {
        getWorld()->playSound(m_deathSound);
        //play the sound
        if (isEnemy())
            //if it is active and enemy (not pit)
        {
            //increase the player score for killing a bacteria
            getWorld()->increaseScore(100);
            //50% that dead bacteria turns into food
            int spawnFood = randInt(0, 1);
            if (spawnFood == 0)
            {
                Actor* temp = new Food(getX(),getY(),getWorld());
                getWorld()->addToList(temp);
            }
        }
    }
    else if (hasDamageSound())
        //if it makes a sound when it is damaged, play it
    {
        getWorld()->playSound(m_damageSound);
    }
}

void Socrates::doSomething()
{
    int ch;
    if (getWorld()->getKey(ch))
    {
        //use polar coordinates to calculate new x,y
        double radius = 128;
        double Xcenter = VIEW_WIDTH/2;
        double Ycenter = VIEW_HEIGHT/2;
        //since cmath cos and sin functions are in radians, convert to radians
        double dr = (5*M_PI/180);
        double r = ((getDirection()+180)*M_PI/180);
        double x,y;
        double dir = getDirection();
        
        switch (ch)
        {
            case KEY_PRESS_SPACE:
            {
                //if there isn't spray left, break
                if(m_nSpray < 1)
                    break;
                //otherwise, spawn spray
                getPositionInThisDirection(dir, 2*SPRITE_RADIUS, x, y);
                Spray* s = new Spray(x,y,dir,getWorld());
                //add spray to actors list
                getWorld()->addToList(s);
                //play sound
                getWorld()->playSound(SOUND_PLAYER_SPRAY);
                //decrement spray
                m_nSpray--;
                break;
            }
            case KEY_PRESS_ENTER:
            {
                //if there isn't flamethrower left, break
                if(m_nFlamethrowers < 1)
                    break;
                //otherwise, spawn 16 flames
                for (int i = 0; i < 16; i++, dir += 22)
                {
                getPositionInThisDirection(dir, 2*SPRITE_RADIUS, x, y);
                Flame* f = new Flame(x,y,dir,getWorld());
                getWorld()->addToList(f);
                }
                //play sound
                getWorld()->playSound(SOUND_PLAYER_FIRE);
                //decrement flamethrowers
                m_nFlamethrowers--;
                break;
            }
            case KEY_PRESS_LEFT:
            {
                double newRadians = r - dr;
                double newDegrees = getDirection()-5;
                //move to new coordinate on circle
                //eqn used based on polar coordinate to cartesian coordinate conversion
                moveTo(radius*cos(newRadians)+Xcenter,
                       radius*sin(newRadians)+Ycenter);
                setDirection(newDegrees);
                break;
            }
            case KEY_PRESS_RIGHT:
            {
                double newRadians = r + dr;
                double newDegrees = getDirection()+5;
                //move to new coordinate on circle
                //eqn used based on polar coordinate to cartesian coordinate conversion
                moveTo(radius*cos(newRadians)+Xcenter,
                       radius*sin(newRadians)+Ycenter);
                setDirection(newDegrees);
                break;
            }
        }
    }
    else if (m_nSpray < 20)
        //if no keys were pressed, and spray is less than 20, increase the spray
        m_nSpray++;
}

void Projectile::doSomething()
{
    //if projectile is dead, do nothing
    if(isDead())
        return;
    //if projectile overlaps with any damageable objects, damage them and set to not alive
    if(getWorld()->hasDamageOverlap(this, SPRITE_RADIUS*2, m_damage))
        //projectiles have initial health of 1, so decrementing health by 1 is same setting it to not alive
    {
        takeDamage(1);
        return;
    }
    //otherwise, move forward
    moveAngle(getDirection(),2*SPRITE_RADIUS);
    //increment distance travelled
    m_distance += 2*SPRITE_RADIUS;
    
    //if distance travelled is the max, projectile dissipates
    //set status to not alive
    if (m_distance >= m_maxdis)
        takeDamage(1);
}

void Goodie::doSomething()
{
    if (isDead())
        //if it is dead, it shouldn't do something
        return;
    double x = getX();
    double y = getY();
    StudentWorld* sp = getWorld();
    if (sp->checkSocratesOverlap(x,y,2*SPRITE_RADIUS))
        //if the goodie overlaps with socrates
    {
        //change the points accordingly
        getWorld()->increaseScore(m_points);
        //set status of goodie to not alive
        //which is the same as damaging goodie by it's health
        takeDamage(getHealth());
        if (hasDeathSound())
            //don't play for fungus
            sp->playSound(SOUND_GOT_GOODIE);
        restore();
        return;
    }
    //a tick has passed, decrement health
    //for gooidies, health is essentially time alive
    takeDamage(1);
}

void Bacteria::getNewCoord(double &x, double &y)
//spawn new bacteria in same location and return true
{
    //get x coordinate of new
    if (x < VIEW_WIDTH/2)
        x += SPRITE_RADIUS;
    if (x > VIEW_WIDTH/2)
        x -= SPRITE_RADIUS;
    //if x == view width/2, do nothing
    
    //get y coordinate of new
    if (y < VIEW_HEIGHT/2)
        y += SPRITE_RADIUS;
    if (y > VIEW_HEIGHT/2)
        y -= SPRITE_RADIUS;
    //if y == view height/2, do nothing
}

bool Bacteria::isWithinDish(double x, double y)
//check if the x and y position is within the petri dish
{
    bool isWithinDish = false;
    double radius = VIEW_RADIUS;
    double xdis = x-VIEW_WIDTH/2;
    double ydis = y-VIEW_HEIGHT/2;
    double dis = sqrt(pow(xdis,2) + pow(ydis,2));
    if (dis <= radius)
        isWithinDish = true;
    return isWithinDish;
}

bool Bacteria::checkPath(double x, double y, double dir)
//checks potential path for dirt
//return true if path is valid (if no dirt)
{
    StudentWorld* sp = getWorld();
    getPositionInThisDirection(dir, m_movement, x, y);
    if (!sp->hasBlockingOverlap(x, y) && isWithinDish(x, y))
    {
        return true;
    }
    return false;
}

int Bacteria::getAngleToChase(double xNew, double yNew)
{
    //compute angle to chase other object
    double dx = xNew-getX();
    double dy = yNew-getY();
    double radians = atan2(dy,dx);
    double degrees = radians * (180/M_PI);
    if (degrees < 0)
        //degrees should be greater than 0 since setDirection takes degrees from 0-359
    {
        degrees += 360;
    }
    // cerr << (int)degrees << endl;
    return (int)degrees;
}

void Salmonella::spawnNew()
//spawn a new salmonella and add it to actor list
{
    double x = getX();
    double y = getY();
    getNewCoord(x, y);
    StudentWorld* sp = getWorld();
    Actor* a = new Salmonella(x,y,sp);
    sp->addToList(a);
}

void AggSalmonella::spawnNew()
//spawn new aggressive salmonella and add to actor list
{
    double x = getX();
    double y = getY();
    getNewCoord(x, y);
    StudentWorld* sp = getWorld();
    Actor* temp = new AggSalmonella(x,y,sp);
    sp->addToList(temp);
}

void EColi::spawnNew()
//spawn new ecoli and add to actor list
{
    double x = getX();
    double y = getY();
    getNewCoord(x, y);
    StudentWorld* sp = getWorld();
    Actor* a = new EColi(x,y,sp);
    sp->addToList(a);
}

void Pit::doSomething()
{
    if (m_nEColi == 0 && m_nSalmonella == 0 && m_nAggSalmonella == 0)
        //if there are no bacteria left to spawn
    {
        takeDamage(1);
        //destroy pit
        return;
    }
    if(chooseRandomBacteria())
        //try to spawn a bacteria
    {
        //play the sound of a bacteria spawning
        getWorld()->playSound(SOUND_BACTERIUM_BORN);
    }
}

bool Pit::chooseRandomBacteria()
{
    Actor* temp = nullptr;
    int rand = randInt(1, 50);
    //one in fifty chance of spawning bacteria
    if (rand == 1)
    {
        while (m_nAggSalmonella > 0 || m_nSalmonella > 0 || m_nEColi > 0)
            //loop through until one of the bacteria is chosen to be spawned
        {
            rand = randInt(1, 3);
            if (rand == 1 && m_nEColi > 0)
                //cannot spawn unless number of ecoli is > 0
                //if < 0, it will go back through loop until available bacteria is chosen
            {
                temp = new EColi(getX(),getY(),getWorld());
                getWorld()->addToList(temp);
                m_nEColi--;
                return true;
            }
            if (rand == 2 && m_nSalmonella > 0)
                //cannot spawn unless number of salmonella is > 0
                //if < 0, it will go back through loop until available bacteria is chosen
            {
                temp = new Salmonella(getX(),getY(),getWorld());
                getWorld()->addToList(temp);
                m_nSalmonella--;
                return true;
            }
            if (rand == 3 && m_nAggSalmonella > 0)
                //cannot spawn unless number of aggressive salmonella is > 0
                //if < 0, it will go back through loop until available bacteria is chosen
            {
                temp = new AggSalmonella(getX(),getY(),getWorld());
                getWorld()->addToList(temp);
                m_nAggSalmonella--;
                return true;
            }
        }
    }
    return false;
}

void Bacteria::doSomething()
{
    
    if (isDead())
       //a dead bacteria cannot do anything
        return;
    
    StudentWorld* sp = getWorld();
    bool chasedSocrates = false;
    double x,y;
    bool isOnSocrates = sp->checkSocratesOverlap(getX(), getY(), 2*SPRITE_RADIUS);
    
    if (isOnSocrates && !isAggressive())
    //if bacteria overlaps with socrates, damage him
    {
        sp->damageSocrates(m_damage);
    }
    
    else if (isAggressive() && sp->checkSocratesOverlap(getX(), getY(), 72))
        //if the bacteria is aggressive and it is within 72 pixels
    {
        //find socrates and chase him
        sp->findSocrates(x, y);
        int newAngle = getAngleToChase(x, y);
        chasedSocrates = true;
        if (checkPath(getX(), getY(), newAngle) || sp->checkSocratesOverlap(getX(), getY(), 2))
        //if the path to chase socrates is clear or the bacteria is already on socrates
        {
            if (sp->checkSocratesOverlap(getX(), getY(), 2))
            {
                //damage socrates
                sp->damageSocrates(m_damage);
            }
            else
            {
                //move towards him
                setDirection(newAngle);
                moveAngle(newAngle, m_movement);
            }
        }
    }
    
    if (m_food >= 3)
    {
        //if bacteria has eaten 3 food, divide into more bacteria by spawning new
        spawnNew();
        m_food = 0;
        if (chasedSocrates)
            return;
    }
    if (sp->hasFoodOverlap(this))
         //calling hasFoodOverlap deletes the food it overlaps with
    {
        m_food++;
        if (chasedSocrates)
            return;
    }
    if (hasMPlan() && m_plan > 0 && !chasedSocrates)
        //if it is not ecoli and there is a movement plan
    {
        x = getX();
        y = getY();
        
        if(checkPath(getX(), getY(), getDirection()))
            //if the path is clear
        {
            //move there
            moveAngle(getDirection(),m_movement);
            m_plan--;
        }
        else
        {
            //get new direction otherwise
            setDirection(randInt(0,359));
            m_plan = 10;
        }
        return;
    }
    else if (isFoodChaser() && m_plan <= 0)
        //if it has ability to chase food and food is within 128 pixels, chase it
        //then return
    {
        if (sp->findFood(this, x, y))
            //if theres a food
        {
            //chase it
            int angle = getAngleToChase(x, y);
            if (checkPath(getX(), getY(), angle))
                //if the path is clear
            {
                //chase the food
                setDirection(angle);
                moveAngle(angle, m_movement);
                return;
            }
        }
        //otherwise set new direction
        setDirection(randInt(0,359));
        m_plan = 10;
    }
    else if (!hasMPlan())
        //if it isnt a food chaser, chase socrates (ecoli)
    {
        //find socrates and compute angle to chase him
        sp->findSocrates(x, y);
        int newAngle = getAngleToChase(x, y);
        if (checkPath(getX(), getY(), newAngle))
            //if the path is clear to socrates
        {
            //chase him
            getPositionInThisDirection(newAngle, m_movement, x, y);
            setDirection(newAngle);
            moveTo(x, y);
        }
        else
        {
        for (int i = 0; i < 10; i++)
            //otherwise get new angle by increasing direction towards socrates by 10 degrees
        {
            newAngle += 10;
            if(checkPath(getX(), getY(), newAngle))
            {
                setDirection(newAngle);
                moveAngle(newAngle,m_movement);
                m_plan = 10;
                return;
            }
        }
        }
    }
}

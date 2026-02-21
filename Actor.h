#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject {
public:
    Actor(StudentWorld* world, int imageID, Coord startCoord);
    virtual void doSomething() = 0;
    virtual bool isBrick() const { return false; }
    
    StudentWorld* world() const;
private:
    StudentWorld* m_world;
};

class FloorBrick : public Actor {
public:
    FloorBrick(StudentWorld* world, Coord startCoord);
    virtual void doSomething() {};
    virtual bool isBrick() const { return true; }
};

class IceMonster : public Actor {
public:
    IceMonster(StudentWorld* world, Coord startCoord);
    virtual void doSomething();
    
    /*
     
     The ice monster must use the appropriate image ID for an ice monster (IID_ICE_MONSTER), and must move left and right along the platform it starts on. On each tick, its doSomething() method should attempt to move one square in its current horizontal direction. If moving forward would cause it to walk into a wall or step off the edge of its platform, it should reverse direction and remain in its current square for that tick.
     
     */

};


#endif // ACTOR_H_

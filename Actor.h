#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject {
public:
    Actor(StudentWorld* world, int imageID, Coord startCoord, int dir);
    virtual void doSomething() = 0;
    
    StudentWorld* world() const;
    
    bool isAlive() { return m_alive; }
    void setDead() { m_alive = false; }
    
    virtual bool isBrick() const { return false; }
    virtual bool isAttractor() const { return false; }
    virtual bool isClimbable() const { return false; }
    virtual bool isBounceable() const { return false; }
    
    virtual bool canBeLaunched() const { return false; }
    bool isValidCoord(Coord c);
    
private:
    StudentWorld* m_world;
    bool m_alive;
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
};

class Player : public Actor {
public:
    Player(StudentWorld* world);
    virtual void doSomething();
private:
    bool isDirection(int value) const;
    bool isTool(int value) const;
    bool inBounds(Coord c) const;
};

class LemmingFactory : public Actor {
public:
    LemmingFactory(StudentWorld* world, Coord startCoord);
    virtual void doSomething();
};

class Lemming : public Actor {
public:
    Lemming(StudentWorld* world, Coord startCoord);
    virtual void doSomething();
    virtual bool isBounceable() const { return true; }
    bool canBeLaunched() const { return m_state != BOUNCING; }
    
    int getBounceHeight() const;
    
    // mutators
    void launch(int bounceDist);
private:
    int m_state;
    
    static const int WALKING = 0;
    static const int FALLING = 1;
    static const int CLIMBING = 2;
    static const int BOUNCING = 3;
    static const int IDLE = 4;
    
    int fallDistance;
    int m_targetBounceDistance;
    int m_upwardStepsAttempted;
    
    // A lemming starts in the walking movement state.
    // 5. A lemming starts with its internal movement-timing counters reset (described below).
    /*
     A lemming is not solid; if asked if it blocks movement by another actor, it must indicate that it
     does not block movement. Multiple lemmings and ice monsters can occupy the same square,
     and lemmings can overlap with non-solid tools and hazards (e.g., pheromones, nets, bonfires,
     exits, one-way doors). A lemming is launchable from a trampoline or spring. A lemming is not
     climbable.
     */
};

class Bonfire : public Actor {
public:
    Bonfire(StudentWorld* world, Coord startCoord);
    virtual void doSomething();
};

class Trampoline : public Actor {
public:
    Trampoline(StudentWorld* world, Coord startCoord);
    virtual void doSomething();
};

class Net : public Actor {
public:
    Net(StudentWorld* world, Coord startCoord);
    virtual void doSomething();
    virtual bool isClimbable() const { return true; }
};

class OneWayDoor : public Actor {
public:
    OneWayDoor(StudentWorld* world, Coord startCoord, bool isLeft);
    virtual void doSomething();
};

class Pheromone : public Actor {
public:
    Pheromone(StudentWorld* world, Coord startCoord);
    virtual void doSomething();
    virtual bool isAttractor() const { return true; }
};

class Spring : public Actor {
public:
    Spring(StudentWorld* world, Coord startCoord);
    virtual void doSomething();
};

class Exit : public Actor {
public:
    Exit(StudentWorld* world, Coord startCoord);
    virtual void doSomething();
};

#endif // ACTOR_H_

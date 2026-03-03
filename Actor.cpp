#include "Actor.h"
#include "StudentWorld.h"

#include <cmath>
using namespace std;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

Actor::Actor(StudentWorld* world, int imageID, Coord startCoord, int dir = right) : GraphObject(imageID, startCoord, dir), m_world(world), m_alive(true)
{}

FloorBrick::FloorBrick(StudentWorld* world, Coord startCoord) : Actor(world, IID_FLOOR, startCoord)
{}

IceMonster::IceMonster(StudentWorld* world, Coord startCoord) : Actor(world, IID_ICE_MONSTER, startCoord)
{}

Player::Player(StudentWorld* world) : Actor(world, IID_PLAYER, Coord(VIEW_WIDTH/2, VIEW_HEIGHT/2))
{}

LemmingFactory::LemmingFactory(StudentWorld* world, Coord startCoord) : Actor(world, IID_LEMMING_FACTORY, startCoord), m_maxLemmings(10)
{}

Lemming::Lemming(StudentWorld* world, Coord startCoord) : Actor(world, IID_LEMMING, startCoord), m_state(WALKING), fallDistance(0)
{} // Normally, startCoord is the lemming factory’s Coord, since the factory spawns lemmings onto its own square.

Bonfire::Bonfire(StudentWorld* world, Coord startCoord) : Actor(world, IID_BONFIRE, startCoord)
{}

Trampoline::Trampoline(StudentWorld* world, Coord startCoord) : Actor(world, IID_TRAMPOLINE, startCoord)
{}

Net::Net(StudentWorld* world, Coord startCoord) : Actor(world, IID_NET, startCoord)
{}

OneWayDoor::OneWayDoor(StudentWorld* world, Coord startCoord, bool isLeft) : Actor (world, IID_ONE_WAY_DOOR, startCoord, isLeft ? GraphObject::left : GraphObject::right)
{}

Pheromone::Pheromone(StudentWorld* world, Coord startCoord) : Actor(world, IID_PHEROMONE, startCoord)
{}

Spring::Spring(StudentWorld* world, Coord startCoord) : Actor(world, IID_SPRING, startCoord)
{}

Exit::Exit(StudentWorld* world, Coord startCoord) : Actor(world, IID_EXIT, startCoord)
{}

// ------------------------ IMPLEMENTATIONS OF FUNCTIONS START HERE ----------------------------
StudentWorld* Actor::world() const {
    return m_world;
}

bool Actor::isValidCoord(Coord c) {
    if (c.x < 0 || c.x >= 20)
        return false;
    
    if (c.y < 0 || c.y >= 20)
        return false;
    
    return true;
}

void IceMonster::doSomething() {
    int currTick = 2000 - world()->returnTimeLeft();
    
    if (world()->isFreezableAt(getCoord())) {
        world()->freezeLemmingAt(getCoord());
    } else if (currTick > 0 && currTick % 10 == 0) {
        // ice monster can move
        Coord front = getTargetCoord(getDirection());
        if(world()->hasSolidBrick(front)) {
            setDirection(getDirection() + 180);
        } else if (!world()->hasSolidBrick(getTargetCoord(front, down))) {
            setDirection(getDirection() + 180);
        } else {
            moveForward();
        }
    }
    
}

void Player::doSomething() {
    
    int key;
    if (world()->getKey(key)) {
        
        int dir = GraphObject::none;
        switch(key) {
            case KEY_PRESS_UP: dir = GraphObject::up; break;
            case KEY_PRESS_DOWN: dir = GraphObject::down; break;
            case KEY_PRESS_LEFT: dir = GraphObject::left; break;
            case KEY_PRESS_RIGHT: dir = GraphObject::right; break;
        }
        
        if (isDirection(key) && inBounds(getTargetCoord(dir))) {
            moveTo(getTargetCoord(dir));
            
        } else if (isTool(key)) {
            
            Coord curr = getCoord();
            if(world()->toolAvailable(key) && world()->isEmpty(curr))
                world()->placeTool(key, curr);
            
        }
    }
}

bool Player::isDirection(int value) const {
    if (value == KEY_PRESS_UP || value == KEY_PRESS_DOWN || value == KEY_PRESS_LEFT || value == KEY_PRESS_RIGHT)
        return true;
    
    return false;
}

bool Player::isTool(int value) const {
    switch(value) {
        case 'T': case 't':
            return true;
        case 'N': case 'n':
            return true;
        case 'P': case 'p':
            return true;
        case 'S': case 's':
            return true;
        case '<':
            return true;
        case '>':
            return true;
    }
    
    return false;
}

bool Player::inBounds(Coord c) const {
    if (c.x < 1 || c.x > VIEW_WIDTH - 2)
        return false;
    
    if (c.y < 1 || c.y > VIEW_HEIGHT - 2)
        return false;
    
    return true;
}

void LemmingFactory::doSomething() {
    int currLemmings = world()->returnSpawnedLemmings();
    int currTick = 2000 - world()->returnTimeLeft();
    
    if (currTick > 0 && currTick % 100 == 0 && currLemmings < m_maxLemmings) {
        world()->spawnLemming(getCoord());
    }
}

void Lemming::doSomething() {
    
    if (!isAlive() || m_state == IDLE)
        return;
        
    // determine pheromone attraction
    int newDirection = world()->determineAttractionDirection(getCoord());
    if (newDirection != -1)
        setDirection(newDirection);
    
    // coordinate definitions
    Coord next = getTargetCoord(getDirection());
    Coord belowNext = getTargetCoord(next, down);
    Coord below = getTargetCoord(down);
    Coord above = getTargetCoord(up);
    
    // movement
    if (m_state == WALKING) {
        if (!(world()->returnTimeLeft() % 4 == 0))           // implement WALK
            return;
            
        if (world()->isClimbableAt(getCoord())) {
            m_state = CLIMBING;
            return;
        }
        
        if (world()->hasSolidBrick(next)) {
            setDirection(getDirection()+180);         // reverses direction
            return;
        }
        
        if (world()->hasSolidBrick(belowNext)) {
            moveTo(next);
        } else {
            m_state = FALLING;
            fallDistance = 0;
            moveTo(next);
        }
    } else if (m_state == FALLING) {                       // implement FALL
        
        if (!(world()->returnTimeLeft() % 2 == 0))
            return;
        
        if (world()->isClimbableAt(getCoord())) {
            m_state = CLIMBING;
            return;
        }
        
        if (!isValidCoord(below) || world()->hasSolidBrick(below)) {
            // landed: resolve fall
            if (fallDistance > 5) {
                setDead();
                world()->playSound(SOUND_LEMMING_DIE);
                world()->incrementDeadLemmings();
            } else {
                m_state = WALKING;   // survive, don't move this tick
                fallDistance = 0;
            }
            return;
        }

        // 4) otherwise keep falling
        fallDistance++;
        moveTo(below);
        return;
        
    } else if (m_state == CLIMBING ) {

        if (!(world()->returnTimeLeft() % 2 == 0))
            return;
        
        if (!world()->isClimbableAt(getCoord())) {
            m_state = WALKING;
            return;
        }
        
        if (isValidCoord(above) && !world()->hasSolidBrick(above)) {
            moveTo(above);
        }
    } else if (m_state == BOUNCING) {
        if (world()->returnTimeLeft() % 2 != 0)
                return;

            if (world()->isClimbableAt(getCoord())) {
                m_state = CLIMBING;
                return;
            }

            // upward phase: attempt to move up until we've taken the desired number of steps.
            // if we can't move up (blocked or out of bounds), the upward phase ends early.
            bool upwardPhaseCompleteThisTick = false;
            if (m_upwardStepsAttempted < m_targetBounceDistance) {
                if (isValidCoord(above) && !world()->hasSolidBrick(above)) {
                    moveTo(above);
                    m_upwardStepsAttempted++;
                    if (m_upwardStepsAttempted >= m_targetBounceDistance)
                        upwardPhaseCompleteThisTick = true;
                } else {
                    // blocked: end upward phase immediately
                    m_upwardStepsAttempted = m_targetBounceDistance;
                    upwardPhaseCompleteThisTick = true;
                }

                if (!upwardPhaseCompleteThisTick)
                    return;
            }

            // apex phase (same tick that the upward phase completes)
            next = getTargetCoord(getDirection());
            if (isValidCoord(next) && !world()->hasSolidBrick(next)) {
                moveTo(next);
            } else {
                // reverse direction without moving
                setDirection(getDirection() == right ? left : right);
            }

            // bounce ends after apex
            m_state = FALLING;
            fallDistance = 0;
            return;
        
    }
    
    return;
}

int Lemming::getBounceHeight() const {
    if (fallDistance - 1 < 0)
        return 0;
    
    return fallDistance - 1;
}

void Lemming::launch(int bounceDist) {
    m_targetBounceDistance = bounceDist;
    m_upwardStepsAttempted = 0;
    
    m_state = BOUNCING;
}

void Bonfire::doSomething() {
    world()->burnLemmingsAt(getCoord());
}

void Trampoline::doSomething() {
    
    Lemming* l = world()->getBounceableLemmingAt(getCoord());
    
    if (l == nullptr)
        return;
    
    l->launch(l->getBounceHeight());
    world()->playSound(SOUND_BOUNCE);
}

void Net::doSomething() {
    
}

void OneWayDoor::doSomething() {
    // always attempt to redirect any redirectable actor standing on this square.
    // (checking "empty" is unreliable because this actor itself occupies the square.)
    world()->switchActorDirection(getCoord(), getDirection());
}

void Pheromone::doSomething() {
    
}

void Spring::doSomething() {
    Lemming* l = world()->getBounceableLemmingAt(getCoord());
    
    if (l == nullptr)
        return;
    
    l->launch(15);
    world()->playSound(SOUND_BOUNCE);
}

void Exit::doSomething() {
    world()->saveLemmingAt(getCoord());
}

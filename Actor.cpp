#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

Actor::Actor(StudentWorld* world, int imageID, Coord startCoord) : GraphObject(imageID, startCoord), m_world(world)
{}

FloorBrick::FloorBrick(StudentWorld* world, Coord startCoord) : Actor(world, IID_FLOOR, startCoord)
{}

IceMonster::IceMonster(StudentWorld* world, Coord startCoord) : Actor(world, IID_ICE_MONSTER, startCoord)
{}

// IMPLEMENTATIONS OF FUNCTIONS START HERE
StudentWorld* Actor::world() const {
    return m_world;
}

void IceMonster::doSomething() {
    Coord nextCoord = getTargetCoord(getDirection());
    if(world()->hasSolidBrick(nextCoord)) {
        setDirection(getDirection() + 180);
    } else if (!world()->hasSolidBrick(getTargetCoord(nextCoord, down))) {
        setDirection(getDirection() + 180);
    } else {
        moveForward();
    }
}
/*

On each tick, its doSomething() method should attempt to move one square in its current horizontal direction. If moving forward would cause it to walk into a wall or step off the edge of its platform, it should reverse direction and remain in its current square for that tick.
 
 void doSomething()
  {
      If a lemming is on my square, then
      Kill the lemming
      Else if it is time for me to move, then
      If the square in front of me is a solid brick, then
      Reverse my direction, but don’t move this tick
      Else if the square in front of me has no solid ground beneath it, then
      Reverse my direction, but don’t move this tick
      Else
      Move one square in my current direction
  }

*/

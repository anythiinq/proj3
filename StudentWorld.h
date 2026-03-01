#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <map>

using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    bool hasSolidBrick(Coord c);
    
    // accessors
    int returnTimeLeft() { return m_timeLeft; }
    
    int determineAttractionDirection(Coord c_lemming);
    bool isClimbableAt(Coord c);
    Lemming* getBounceableLemmingAt(Coord c);
    bool isEmpty(Coord c);                             // returns if a square at c is empty or not (no actors at all)
    
    map<char, int> m_toolCounts;
    
    bool toolAvailable(char c);
    void consumeTool(char c);
    void placeTool(char tool, Coord c);
    
private:
    string getCurrentLevel();
    vector<Actor*> m_actors;
    Player* m_player;
    
    int m_timeLeft;
    int m_nDeadLemmings;
    int m_nSavedLemmings;
    int m_nSpawnedLemmings;
    int countLemmingsAlive();
    bool isLemming();
};

#endif // STUDENTWORLD_H_

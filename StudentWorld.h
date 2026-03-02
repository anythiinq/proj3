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
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    bool hasSolidBrick(Coord c);
    
    // accessors
    int returnTimeLeft() const { return m_timeLeft; }
    int returnSpawnedLemmings() const { return m_nSpawnedLemmings; }
    
    int determineAttractionDirection(Coord c_lemming);
    bool isClimbableAt(Coord c);
    Lemming* getBounceableLemmingAt(Coord c);
    bool isBurnableAt(Coord c);
    bool isFreezableAt(Coord c);
    bool isEmpty(Coord c);                             // returns if a square at c is empty or not (no actors at all)
    
    bool toolAvailable(char c);
    void placeTool(char tool, Coord c);
    
    void incrementDeadLemmings() { m_nDeadLemmings++; }
    
    void spawnLemming(Coord c);
    void saveLemmingAt(Coord c);
    void burnLemmingsAt(Coord c);
    void freezeLemmingAt(Coord c);
    
    void switchActorDirection(Coord c, int dir);
    
private:
    string getCurrentLevel();
    string builtStatText();
    
    string getToolsRemainingString();
    
    vector<Actor*> m_actors;
    Player* m_player = nullptr;

    map<char, int> m_toolCounts;
    
    int m_timeLeft;
    int m_nDeadLemmings;
    int m_nSavedLemmings;
    int m_nSpawnedLemmings;
    int countLemmingsAlive();
};

#endif // STUDENTWORLD_H_

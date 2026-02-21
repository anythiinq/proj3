#include "StudentWorld.h"
#include "GameConstants.h"
#include "Level.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp
// Do not change or remove the createStudentWorld implementation above.

/*
 Fourth, you must implement a limited version of the StudentWorld class. This class is
 responsible for managing the world and the actors in it. You should add whatever private data
 members are necessary to keep track of all floor bricks and ice monsters in the level. You may
 ignore all other game elements such as lemmings, exits, bonfires, tools, scoring, lives, or time
 limits for Part 1
 
 Your StudentWorld constructor should initialize your data members, and your destructor must
 free any dynamically allocated memory that still exists when the StudentWorld object is
 destroyed.
 */

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{}

// done
int StudentWorld::init()
{
    // read current level file
    Level lev(assetPath()); // note: GameWorld provides assetPath()
    string currLevel = getCurrentLevel();
    Level::LoadResult result = lev.loadLevel(currLevel);
    if (result == Level::load_fail_file_not_found) {
        cerr << lev.getErrorMessage() << endl;
        return GWSTATUS_PLAYER_WON;
    } else if (result == Level::load_fail_bad_format) {
        cerr << lev.getErrorMessage() << endl;
    } else if (result == Level::load_success) {
        cerr << "Successfully loaded level\n";
    }
    
    // create FloorBrick and IceMonster objects at the appropriate grid locations specified by the level data
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 20; j++) {
            Coord c(i, j);
            Level::MazeEntry me = lev.getContentsOf(c);
            switch (me){
                case Level::floor:
                    m_actors.push_back(new FloorBrick(this, c));
                    break;
                case Level::ice_monster:
                    m_actors.push_back(new IceMonster(this, c));
                    break;
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

// done
int StudentWorld::move()
{
    for (int i = 0; i < m_actors.size(); i++) {
        m_actors[i]->doSomething();
    }
    
    // This code is here merely to allow the game to build, run, and terminate after you type q

    setGameStatText("Game will end when you type q");
    
    return GWSTATUS_CONTINUE_GAME;
}

// done
void StudentWorld::cleanUp()
{
    for  (int i = 0; i < m_actors.size(); i++)
        delete m_actors[i];
    
    m_actors.clear();
}

// returns true if there is a brick at Coord c
bool StudentWorld::hasSolidBrick(Coord c) {
    for (int i = 0; i < m_actors.size(); i++) {
        if (m_actors[i]->isBrick() && m_actors[i]->getCoord() == c)
            return true;
    }
    
    return false;
}

/*
 PRIVATE MEMBER FUNCTIONS START HERE
 */
string StudentWorld::getCurrentLevel() {
    int lvl = getLevel();
    
    if (lvl < 10)
        return "level0" + to_string(lvl) + ".txt";
    else {
        return "level" + to_string(lvl) + ".txt";
    }
}

#include "StudentWorld.h"
#include "GameConstants.h"
#include "Level.h"
#include <string>
#include <cctype>
#include <sstream>
#include <iomanip>
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
: GameWorld(assetPath), m_timeLeft(0)
{}

int StudentWorld::init()
{
    // reset all variables
    m_timeLeft = 2000;
    m_nSpawnedLemmings = 0;
    m_nDeadLemmings = 0;
    m_nSavedLemmings = 0;
    m_actors.clear();
    m_toolCounts.clear();
    
    // read current level file
    Level lev(assetPath()); // note: GameWorld provides assetPath()
    string currLevel = getCurrentLevel();
    Level::LoadResult result = lev.loadLevel(currLevel);
    if (result == Level::load_fail_file_not_found) {
        cerr << lev.getErrorMessage() << endl;
        return GWSTATUS_PLAYER_WON;
    } else if (result == Level::load_fail_bad_format) {
        cerr << lev.getErrorMessage() << endl;
        return GWSTATUS_LEVEL_ERROR;
    } else if (result == Level::load_success) {
        cerr << "Successfully loaded level\n";
    }
    
    string tools = lev.getTools();
    
    for (char c : tools) {            // initiate tool line
        m_toolCounts[c]++;
    }
    
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
                case Level::lemming_factory:
                    m_actors.push_back(new LemmingFactory(this, c));
                    break;
                case Level::bonfire:
                    m_actors.push_back(new Bonfire(this, c));
                    break;
                case Level::trampoline:
                    m_actors.push_back(new Trampoline(this, c));
                    break;
                case Level::net:
                    m_actors.push_back(new Net(this, c));
                    break;
                case Level::left_one_way_door:
                    m_actors.push_back(new OneWayDoor(this, c, true));   // isLeft = true
                    break;
                case Level::right_one_way_door:
                    m_actors.push_back(new OneWayDoor(this, c, false));  // isLeft = false
                    break;
                case Level::lemming_exit:
                    m_actors.push_back(new Exit(this, c));
                    break;
                case Level::pheromone:
                    m_actors.push_back(new Pheromone(this, c));
                    break;
                case Level::spring:
                    m_actors.push_back(new Spring(this, c));
                    break;
                case Level::empty:
                    continue;
                
            }
        }
    }
    
    m_player = new Player(this);
    m_timeLeft = 2000;
    m_nDeadLemmings = m_nSavedLemmings = m_nSpawnedLemmings = 0;
    
    return GWSTATUS_CONTINUE_GAME;
}

// START HERE, NEXT TIME
int StudentWorld::move()
{
    if (m_player != nullptr)
            m_player->doSomething();
    
    for (int i = 0; i < m_actors.size(); i++) {
        if (!m_actors[i]->isAlive()) // and it's dead, don't do anything (skip it)
            continue;
        m_actors[i]->doSomething();
    }
    m_timeLeft--;
    
    // delete dead actors here!
    for (int i = 0; i < m_actors.size(); i++) {
        if (!m_actors[i]->isAlive()) {
            delete m_actors[i];                    // free memory
            m_actors.erase(m_actors.begin() + i);  // remove pointer from vector
        }
        else {
            i++;   // only increment if you did NOT erase
        }
    }
    
    // This code is here merely to allow the game to build, run, and terminate after you type q
    setGameStatText("Game will end when you type q");
    
    if (m_nDeadLemmings > 5) {
        decLives();
        return GWSTATUS_PLAYER_DIED;
    } else if (m_nSavedLemmings > 5 && countLemmingsAlive() == 0 && m_nSpawnedLemmings == 10) {
        increaseScore(m_timeLeft);
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    if (m_timeLeft == 0) {
        if (m_nSavedLemmings < 5) {
            decLives();
            return GWSTATUS_PLAYER_DIED;
        } else {
            increaseScore(0);                      // no time left on the clock!
            playSound(SOUND_FINISHED_LEVEL);
            return GWSTATUS_FINISHED_LEVEL;
        }
    }
    
    setGameStatText(builtStatText());
    
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

int StudentWorld::determineAttractionDirection(Coord c_lemming) {
    
    vector<Coord> c_pheromones;                  // contains the coordinates of all the pheromones that are attracted by the lemming
    for (int i = 0; i < m_actors.size(); i++) {
        if (m_actors[i]->isAttractor()) {
            Coord c_pheromone = m_actors[i]->getCoord();
            
            if (c_lemming.y != c_pheromone.y)
                continue;
            
            int dx = abs(c_lemming.x - c_pheromone.x);
            if (dx > 5 || dx == 0)
                continue;
            
            c_pheromones.push_back(c_pheromone);
        }
    }
    
    int min = 6;
    int currDir = GraphObject::none;
    for (int i = 0; i < c_pheromones.size(); i++) {
        int displacement = c_lemming.x - c_pheromones[i].x;
        if (abs(displacement) < min) {
            min = abs(displacement);
            if (displacement < 0)   // pheromone is to the right of lemming
                currDir = GraphObject::right;        // right = 0
            else
                currDir = GraphObject::left;      // left = 180
        }
    }
    
    return currDir;
}

bool StudentWorld::isClimbableAt(Coord c) {
    for (int i = 0; i < m_actors.size(); i++) {
        if (m_actors[i]->isClimbable()) {
            if (m_actors[i]->getCoord() ==  c)
                return true;
        }
    }
    return false;
}

Lemming* StudentWorld::getBounceableLemmingAt(Coord c) {
    for (int i = 0; i < m_actors.size(); i++) {
        if (m_actors[i]->isBounceable()) {
            if (m_actors[i]->getCoord() == c && m_actors[i]->canBeLaunched()) {
                return static_cast<Lemming*>(m_actors[i]);
            }
        }
    }
    return nullptr;
}

bool StudentWorld::isBurnableAt(Coord c) {
    for (auto a : m_actors) {
        if (a->isAlive() && a->isBurnable() && a->getCoord() == c)
            return true;
    }
    
    return false;
}

bool StudentWorld::isFreezableAt(Coord c) {
    for (auto a : m_actors) {
        if (a->isAlive() && a->isFreezable() && a->getCoord() == c)
            return true;
    }
    
    return false;
}

bool StudentWorld::isEmpty(Coord c) {
    for (auto a : m_actors) {
        if (a->isAlive() && a->getCoord() == c)
            return false;
    }
    return true;
}

bool StudentWorld::toolAvailable(char c) {
    c = toupper(c);
    return m_toolCounts[c] > 0;
}

void StudentWorld::placeTool(char tool, Coord c) {
    Actor* a = nullptr;
    tool = toupper(tool);
    
    switch (tool) {
      case 'T': a = new Trampoline(this, c); break;
      case 'N': a = new Net(this, c); break;
      case 'P': a = new Pheromone(this, c); break;
      case 'S': a = new Spring(this, c); break;
      case '<': a = new OneWayDoor(this, c, GraphObject::left); break;
      case '>': a = new OneWayDoor(this, c, GraphObject::right); break;
    }

    if (a != nullptr) {
        m_actors.push_back(a);
        m_toolCounts[tool]--;
    }
}

void StudentWorld::spawnLemming(Coord c) {
    m_nSpawnedLemmings++;
    m_actors.push_back(new Lemming(this, c));
}

void StudentWorld::saveLemmingAt(Coord c) {
    for (Actor* a : m_actors) {
        if (a->isAlive() && a->getCoord() == c && a->canExit())
        {
            a->beSaved();
            increaseScore(100);
            m_nSavedLemmings++;
            playSound(SOUND_LEMMING_SAVED);
            return;
        }
    }
}

void StudentWorld::burnLemmingsAt(Coord c) {
    
    for (auto a : m_actors) {
        if (a->isAlive() && a->isBurnable() && a->getCoord() == c) {
            playSound(SOUND_LEMMING_DIE);
            m_nDeadLemmings++;
            a->setDead();
        }
    }
}

void StudentWorld::freezeLemmingAt(Coord c) {
    
    for (auto a : m_actors) {
        if (a->isAlive() && a->isFreezable() && a->getCoord() == c) {
            playSound(SOUND_LEMMING_DIE);
            m_nDeadLemmings++;
            a->setDead();
        }
    }
    
}

void StudentWorld::switchActorDirection(Coord c, int dir) {
    for (auto a : m_actors) {
        if (a->isAlive() && a->getCoord() == c) {
            a->setDirection(dir);
        }
    }
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

string StudentWorld::builtStatText() {
    
    ostringstream oss;
    oss << "Score: " << setw(5) << setfill('0') << getScore()
        << "  Level: " << setw(2) << setfill('0') << getLevel()
        << "  Lives: " << setw(2) << setfill('0') << getLives()
        << "  Saved: " << setw(2) << setfill('0') << m_nSavedLemmings
        << "  Tools: " << getToolsRemainingString()
        << "  Time left: " << setw(4) << setfill('0') << m_timeLeft;
    return oss.str();
    
}

string StudentWorld::getToolsRemainingString() {
    
    string s;

    auto addMany = [&](char c)
    {
        auto it = m_toolCounts.find(c);
        if (it != m_toolCounts.end())
            s.append(it->second, c);
    };

    addMany('T');
    addMany('N');
    addMany('P');
    addMany('S');
    addMany('<');
    addMany('>');

    if (s.empty()) return "None";
    return s;
    
}

int StudentWorld::countLemmingsAlive() {
    return m_nSpawnedLemmings - m_nDeadLemmings - m_nSavedLemmings;
}

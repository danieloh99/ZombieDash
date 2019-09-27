#include "StudentWorld.h"
#include "GameConstants.h"
#include "Level.h"
#include "Actor.h"
#include <vector>
#include <string>
#include <cmath>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}
StudentWorld::~StudentWorld() {
	cleanUp();
}
int StudentWorld::init()
{
	Level lev(assetPath()); // Makes an empty 16x16 grid.

	int levelInt = getLevel();
	string levelFile;
	if (levelInt < 10) {
		levelFile = "level0" + to_string(levelInt) + ".txt";
	}
	else
		levelFile = "level" + to_string(levelInt) + ".txt";
	Level::LoadResult result = lev.loadLevel(levelFile); // Attempt to load the level

	// If level doesn't load:
	if (result == Level::load_fail_file_not_found || levelInt == 100)
		return GWSTATUS_PLAYER_WON;
	else if (result == Level::load_fail_bad_format)
		return GWSTATUS_LEVEL_ERROR;
	// If the level does load:
	else if (result == Level::load_success) {
		cerr << "Successfully loaded " << levelFile << endl;

		finishedLevel = false; // When the level is loaded, it starts unfinished.
		int numObjectsCreated = 0; // DEBUGGING PURPOSES
		numCitizens = 0;
		// Read the level file
		Level::MazeEntry gameObject;
		for (int i = 0; i < LEVEL_HEIGHT; i++) {
			for (int j = 0; j < LEVEL_WIDTH; j++) {
				gameObject = lev.getContentsOf(i, j); // Reads char in text file, assigns to gameObject
				cerr << "Location " << i * SPRITE_WIDTH << "," << j * SPRITE_HEIGHT;

				switch (gameObject) {
				case Level::empty:
					cerr << " is empty." << endl;
					break;
				case Level::player:
					cerr << " is where Penelope starts." << endl;
					m_penelope = new Player(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT);
					numObjectsCreated++; // DEBUGGING PURPOSES
					break;
				case Level::wall:
					cerr << " holds a wall." << endl;
					allActors.push_back(new Wall(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
					numObjectsCreated++; // DEBUGGING PURPOSES
					break;
				case Level::exit:
					cerr << " holds an exit." << endl;
					allActors.push_back(new Exit(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
					numObjectsCreated++; // DEBUGGING PURPOSES
					break;
				case Level::citizen:
					cerr << " starts with a citizen." << endl;
					allActors.push_back(new Citizen(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
					numObjectsCreated++; // DEBUGGING PURPOSES
					numCitizens++;
					break;
				case Level::vaccine_goodie:
					cerr << " holds a vaccine goodie." << endl;
					allActors.push_back(new Vaccine(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
					numObjectsCreated++;
					break;
				case Level::gas_can_goodie:
					cerr << " holds a gas can goodie." << endl;
					allActors.push_back(new GasCan(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
					numObjectsCreated++;
					break;
				case Level::landmine_goodie:
					cerr << " holds a landmine goodie." << endl;
					allActors.push_back(new LandmineGoodie(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
					numObjectsCreated++;
					break;
				case Level::pit:
					cerr << " holds a pit." << endl;
					allActors.push_back(new Pit(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
					numObjectsCreated++;
					break;
				case Level::dumb_zombie:
					cerr << " starts with a dumb Zombie." << endl;
					allActors.push_back(new Dumb(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
					numObjectsCreated++;
					break;
				case Level::smart_zombie:
					cerr << " starts with a smart Zombie." << endl;
					allActors.push_back(new Smart(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
					numObjectsCreated++;
					break;
				}
			}
		} // Finish reading level
		cerr << "Num Objects Created: " << numObjectsCreated << endl; // DEBUGGING PURPOSES
		cerr << "Num Citizens: " << numCitizens << endl;
	}
	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	// This code is here merely to allow the game to build, run, and terminate after you hit enter.
	// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
	m_penelope->doSomething();
	if (!m_penelope->isAlive()) {
		return GWSTATUS_PLAYER_DIED;
	}
	list<Actor*>::iterator it = allActors.begin();
	while (it != allActors.end()) {
		if ((*it)->isAlive())
			(*it)->doSomething();
		if (!m_penelope->isAlive()) {
			return GWSTATUS_PLAYER_DIED;
		}
		else if (finishedLevel)
			return GWSTATUS_FINISHED_LEVEL;
		it++;
	}

	// Remove newly-dead actors after each tick
	it = allActors.begin();
	while (it != allActors.end()) {
		if (!(*it)->isAlive()) {
			delete *it;
			it = allActors.erase(it);
		}
		else it++;
	}
	// Update the game status line
	double score = getScore();
	ostringstream oss;
	if (score >= 0) { oss << setw(6) << setfill('0') << score; }
	else { oss << "-" << setw(5) << setfill('0') << score * -1; }
	setGameStatText("Score: " + oss.str() + "  Level: " + to_string(getLevel())
		+ "  Lives: " + to_string(getLives()) + "  Vaccines: " + to_string(m_penelope->getVaccines())
		+ "  Flames: " + to_string(m_penelope->getFlames()) + "  Mines: " + to_string(m_penelope->getLandmines())
		+ "  Infected: " + to_string(m_penelope->getInfectionCount()) );

	// the player hasn’t completed the current level and hasn’t died, so
	// continue playing the current level
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	// Delete anything still in the list. Delete m_penelope if she hasn't been deleted.
	list<Actor*>::iterator it = allActors.begin();
	while (it != allActors.end()) {
		delete *it;
		it = allActors.erase(it);
	}
	if (m_penelope != nullptr) {
		delete m_penelope;
		m_penelope = nullptr;
	}
	cerr << "List size after destruction: " << allActors.size() << endl; // DEBUGGING PURPOSES
}

// Called each time a mover attempts to move
bool StudentWorld::blockingActorExists(double moverX, double moverY, double travelDistance, Direction dir) {
	// Check if Penelope blocks mover
	if (doesBlock(m_penelope->getY(), m_penelope->getY() + SPRITE_HEIGHT - 1, m_penelope->getX(),
		m_penelope->getX() + SPRITE_WIDTH - 1, moverX, moverY, travelDistance, dir))
		return true;

	list<Actor*>::iterator it = allActors.begin();

	while (it != allActors.end()) { 
		// Go through allActors until reaching an actor that blocks movement
		if ((*it)->isAlive() && (*it)->blocksMovement()) {
			// Check if Actor blocks mover
			if (doesBlock((*it)->getY(), (*it)->getY() + SPRITE_HEIGHT - 1, (*it)->getX(),
				(*it)->getX() + SPRITE_WIDTH - 1, moverX, moverY, travelDistance, dir))
				return true;
		}
		it++;
	}
	return false;
}

void StudentWorld::saveOverlapCitizens(const Actor* overlapee) {
	list<Actor*>::iterator it = allActors.begin();
	// Called during Exit's doSomething() to save any overlapping citizens.
	while (it != allActors.end()) {
		if ((*it)->isAlive() && (*it)->canExit() && doesOverlap(*it, overlapee->getX(), overlapee->getY())) {
			(*it)->setDead();
			playSound(SOUND_CITIZEN_SAVED);
			increaseScore(500);
		}
		it++;
	}
}
bool StudentWorld::killOverlapMovers(const Actor* overlapee) {
	// Called by pits to kill anything that moves into it
	list<Actor*>::iterator it = allActors.begin();
	bool killed = false;
	while (it != allActors.end()) {
		if ((*it)->isAlive() && (*it)->canMove() && doesOverlap(*it, overlapee->getX(), overlapee->getY())) {
			(*it)->setDead();
			killed = true;
			if ((*it)->canExit()) {
				playSound(SOUND_CITIZEN_DIE);
				increaseScore(-1000);
			}
		}
		it++;
	}
	return killed;
}
bool StudentWorld::killOverlapDamageables(const Actor* overlapee) {
	// Called by flames to kill anything that overlap with it and are damageable by flame
	bool killed = false;
	list<Actor*>::iterator it = allActors.begin();

	while (it != allActors.end()) {
		if ((*it)->isAlive() && (*it)->isDamageable() && doesOverlap(*it, overlapee->getX(), overlapee->getY())) {
			(*it)->setDead();
			killed = true;
			if ((*it)->canExit()) {
				playSound(SOUND_CITIZEN_DIE);
				increaseScore(-1000);
			}
		}
		it++;
	}
	return killed;
}
void StudentWorld::infectOverlapCitizens(const Actor* overlapee) {
	// Called by Vomit to infect any Citizens that overlap with it
	list<Actor*>::iterator it = allActors.begin();
	while (it != allActors.end()) {
		if ((*it)->isAlive() && (*it)->canExit() && !(*it)->isInfected() && doesOverlap(*it, overlapee->getX(), overlapee->getY())) {
			(*it)->setInfectionStatus(true);
			playSound(SOUND_CITIZEN_INFECTED);
		}
		it++;
	}
}
// Called for infecting player when vomit overlaps with player
void StudentWorld::infectPlayer() { m_penelope->setInfectionStatus(true); }
// Called for all Activatables
bool StudentWorld::overlapsWithPlayer(const Actor* overlapee) {
	return doesOverlap(m_penelope, overlapee->getX(), overlapee->getY());
}
// When player dies, the tasks in this function occur every time
void StudentWorld::killPlayer() {
	m_penelope->setDead();
	decLives();
	playSound(SOUND_PLAYER_DIE);
}

// Returns distance from xPos,yPos to player
double StudentWorld::distToPlayer(double xPos, double yPos) {
	double xDist = m_penelope->getX() - xPos;
	double yDist = m_penelope->getY() - yPos;
	return computeDistance(xDist, yDist);
}
// Called by Citizen when determining direction away from the closest zombie
Direction StudentWorld::getDirAwayClosestZombie(double xPos, double yPos, double travelDist) {
	list<Actor*>::iterator it = allActors.begin();
	double distToOrigClosest = 0, currDist;
	int xDir, yDir, finalDir = -1;

	// Find distance to closest zombie and the 2 possible directions
	// away from closest zombie.
	while (it != allActors.end()) {
		if ((*it)->isAlive() && (*it)->canInfect()) {
			currDist = computeDistance((*it)->getX() - xPos, (*it)->getY() - yPos);
			if (currDist < distToOrigClosest || distToOrigClosest == 0) {
				distToOrigClosest = currDist;
				xDir = getXDirAwayActor(xPos, *it);
				yDir = getYDirAwayActor(yPos, *it);
			}
		}
		it++;
	}
	// If distance to closest zombie > 80, return 'no direction'.
	if (distToOrigClosest > 80) return -1;

	// Find which direction would get furthest away from Zombie.
	double greatestDistAway = distToOrigClosest;
	double newDistAway;
	if (!blockingActorExists(xPos, yPos, travelDist, 0)) {
		newDistAway = distToClosestZombie(xPos + travelDist, yPos);
		if (newDistAway > greatestDistAway) {
			greatestDistAway = newDistAway;
			finalDir = 0;
		}
	}
	if (!blockingActorExists(xPos, yPos, travelDist, 180)) {
		newDistAway = distToClosestZombie(xPos - travelDist, yPos);
		if (newDistAway > greatestDistAway) {
			greatestDistAway = newDistAway;
			finalDir = 180;
		}
	}
	if (!blockingActorExists(xPos, yPos, travelDist, 90)) {
		newDistAway = distToClosestZombie(xPos, yPos + travelDist);
		if (newDistAway > greatestDistAway) {
			greatestDistAway = newDistAway;
			finalDir = 90;
		}
	}
	if (!blockingActorExists(xPos, yPos, travelDist, 270)) {
		newDistAway = distToClosestZombie(xPos, yPos - travelDist);
		if (newDistAway > greatestDistAway) {
			greatestDistAway = newDistAway;
			finalDir = 270;
		}
	}

	// If the distance away from the zombie is the same
	// in all directions, return 'no direction'.
	if (greatestDistAway == distToOrigClosest) return -1;

	return finalDir;
}

// Called by Smart Zombie, when determining direction
Direction StudentWorld::getDirToClosestHuman(double xPos, double yPos) {
	list<Actor*>::iterator it = allActors.begin();

	// Find direction(s) to closest Human. Start with penelope.
	// Find distance to Penelope.
	double minDist = distToPlayer(xPos, yPos);
	int xDir = getXDirToPlayer(xPos);
	int yDir = getYDirToPlayer(yPos);

	// Then compare distance to every citizen. If the citizen is closer,
	// reset the distance and directions.
	double currDist;
	while (it != allActors.end()) {
		if ((*it)->isAlive() && (*it)->canExit()) {
			currDist = computeDistance((*it)->getX() - xPos, (*it)->getY() - yPos);
			if (currDist < minDist) {
				minDist = currDist;
				xDir = getXDirToActor(xPos, *it);
				yDir = getYDirToActor(yPos, *it);
			}
		}
		it++;
	}

	// If closest Human is more than 80 pixels away, return rand dir
	if (minDist > 80) return randInt(0, 3) * 90;
	
	// If closest human is in same row or column, return the only dir possible
	if (xDir == -1) return yDir;
	else if (yDir == -1) return xDir;

	// Else pick one of the two dirs randomly, return it
	else {
		int rand = randInt(0, 1);
		if (rand == 0) return xDir;
		return yDir;
	}
}

// Called by Citizen to get direction toward Player
int StudentWorld::getXDirToPlayer(double xPos) { return getXDirToActor(xPos, m_penelope); }
int StudentWorld::getYDirToPlayer(double yPos) { return getYDirToActor(yPos, m_penelope); }

// Give Player Goodies
void StudentWorld::givePlayerVacc(int howMuch) { m_penelope->incVaccines(howMuch); }
void StudentWorld::givePlayerGasCan(int howMuch) { m_penelope->incFlames(howMuch); }
void StudentWorld::givePlayerLandmine(int howMuch) { m_penelope->incLandmines(howMuch); }

// Add damagers at specific locations
void StudentWorld::addPit(double xPos, double yPos) { allActors.push_back(new Pit(this, xPos, yPos)); }
void StudentWorld::addLandmine(double xPos, double yPos) { allActors.push_back(new Landmine(this, xPos, yPos)); }
bool StudentWorld::attemptAddFlame(double xPos, double yPos, Direction dir) {
	if (overlapsFlameBlocker(xPos, yPos)) return false;
	allActors.push_back(new Flame(this, xPos, yPos, dir));
	return true;
}

// Run the random number for adding vomit (1 in 3). Returns true
// if vomit was added, otherwise returns false.
bool StudentWorld::addedVomit(double xPos, double yPos, Direction dir) {
	int rand = randInt(1, 3);
	if (rand == 1) {
		allActors.push_back(new Vomit(this, xPos, yPos, dir));
		playSound(SOUND_ZOMBIE_VOMIT);
		return true;
	}
	return false;
}

// Add vomit at specific location. More specific conditions
bool StudentWorld::attemptAddVomit(double xPos, double yPos, Direction dir) {
	list<Actor*>::iterator it = allActors.begin();
	bool vomitted = false;
	// If Penelope is alive and overlaps with the location where Vomit would be added,
		// If the game adds Vomit at that location, then vomitted = true.
	if (m_penelope->isAlive() && doesOverlap(m_penelope, xPos, yPos)) {
		if (addedVomit(xPos, yPos, dir))
			vomitted = true;
	}
	// If a living citizen overlaps with the location where vomit would be added,
		// If the game adds Vomit at that location, then vomitted = true. 
	while (it != allActors.end()) {
		if ((*it)->isAlive() && (*it)->canExit() && doesOverlap(*it, xPos, yPos)) {
			if (addedVomit(xPos, yPos, dir))
				vomitted = true;
		}
		it++;
	}
	return vomitted;
}

// Attempt to add vaccine when dumb zombie dies and carries vaccine
bool StudentWorld::attemptAddVaccine(double xPos, double yPos) {
	if (doesOverlap(m_penelope, xPos, yPos)) return false;
	list<Actor*>::iterator it = allActors.begin();
	// If any object in the game would overlap with the desired location of the vaccine
	// do nothing
	while (it != allActors.end()) {
		if ((*it)->isAlive() && doesOverlap(*it, xPos, yPos))
			return false;
		it++;
	}
	// Otherwise, add a vaccine at the desired location
	allActors.push_back(new Vaccine(this, xPos, yPos));
	return true;
}

// When Citizen dies from infection, zombie must be added at its location
void StudentWorld::addDumbZombie(double xPos, double yPos) { allActors.push_back(new Dumb(this, xPos, yPos)); }
void StudentWorld::addSmartZombie(double xPos, double yPos) { allActors.push_back(new Smart(this, xPos, yPos)); }




// PRIVATE FUNCTIONS --------------------------------------------------------------------------------------------

// Getters for possible directions toward an Actor
int StudentWorld::getXDirToActor(double xPos, const Actor* actor) {
	if (xPos > actor->getX()) return 180;
	else if (xPos < actor->getX()) return 0;
	else return -1;
}
int StudentWorld::getYDirToActor(double yPos, const Actor* actor) {
	if (yPos > actor->getY()) return 270;
	else if (yPos < actor->getY()) return 90;
	else return -1;
}
int StudentWorld::getXDirAwayActor(double xPos, const Actor* actor) {
	int dir = (getXDirToActor(xPos, actor) + 180) % 360;
	if (dir != 0 && dir != 180)
		return -1;
	return dir;
}
int StudentWorld::getYDirAwayActor(double yPos, const Actor* actor) {
	int dir = (getYDirToActor(yPos, actor) + 180) % 360;
	if (dir != 0 && dir != 180)
		return -1;
	return dir;
}
double StudentWorld::distToClosestCitizen(double xPos, double yPos) {
	double minDist = 0, currDist;
	list<Actor*>::iterator it = allActors.begin();
	while (it != allActors.end()) {
		if ((*it)->isAlive() && (*it)->canExit()) {
			currDist = computeDistance((*it)->getX() - xPos, (*it)->getY() - yPos);
			if (currDist < minDist || minDist == 0)
				minDist = currDist;
		}
		it++;
	}
	return minDist;
}
double StudentWorld::distToClosestZombie(double xPos, double yPos) {
	list<Actor*>::iterator it = allActors.begin();
	double minDist = 0, currDist;

	while (it != allActors.end()) {
		if ((*it)->isAlive() && (*it)->canInfect()) {
			currDist = computeDistance((*it)->getX() - xPos, (*it)->getY() - yPos);
			if (currDist < minDist || minDist == 0)
				minDist = currDist;
		}
		it++;
	}
	return minDist;
}

// Checks if mover's yPos is within the blocker's y-Range
bool StudentWorld::inHeightRange(double moverBottom, double moverTop, double blockerBottom, double blockerTop) const {
	return ( (moverBottom >= blockerBottom && moverBottom <= blockerTop) ||
		(moverTop >= blockerBottom && moverTop <= blockerTop) );
}
// Checks if mover's xPos is within the blocker's x-Range
bool StudentWorld::inWidthRange(double moverLeft, double moverRight, double blockerLeft, double blockerRight) const {
	return ( (moverLeft >= blockerLeft && moverLeft <= blockerRight) ||
		(moverRight >= blockerLeft && moverRight <= blockerRight) );
}
// Checks if blocker blocks mover
bool StudentWorld::doesBlock(double blockerBot, double blockerTop, double blockerLeft,
	double blockerRight, double moverX, double moverY, double travelDistance, Direction dir) const {

	// Get direction and position of mover
	double moverBot = moverY;
	double moverTop = moverY + SPRITE_HEIGHT - 1;
	double moverLeft = moverX;
	double moverRight = moverX + SPRITE_WIDTH - 1;
	
	switch (dir) {

	case 180:
		// If the blocker's right side is to the right of the mover's left side
		// ignore the blocker.
		if (blockerRight >= moverLeft)
			break;

		// If the blocker's right side is equal to or past the left side of the
		// mover's new position and mover's yPos is within the blocker's y-Range, return true.
		if (blockerRight >= moverLeft - travelDistance &&
			inHeightRange(moverBot, moverTop, blockerBot, blockerTop))
			return true;
		break;

	case 0:
		// If the blocker's left side is to the left of the mover's right side
		// ignore the blocker.
		if (blockerLeft <= moverRight)
			break;
		// If the blocker's left side is equal to or past the right side of the
		// mover's new position and mover's yPos is within the blocker's y-Range, return true.
		if (blockerLeft <= moverRight + travelDistance &&
			inHeightRange(moverBot, moverTop, blockerBot, blockerTop))
			return true;
		break;

	case 90:
		// If the blocker's bottom is below the mover's top, ignore the blocker.
		if (blockerBot <= moverTop)
			break;
		// If the blocker's bottom is past the top side of the mover's 
		// new position and mover's xPos is within the blocker's x-Range, return true.
		if (blockerBot <= moverTop + travelDistance &&
			inWidthRange(moverLeft, moverRight, blockerLeft, blockerRight))
			return true;
		break;

	case 270:
		// If the blocker's top is above the mover's bottom, ignore the blocker.
		if (blockerTop >= moverBot)
			break;
		// If the blocker's top is past the bottom side of the mover's 
		// new position and mover's xPos is within the blocker's x-Range, return true.
		if (blockerTop >= moverBot - travelDistance &&
			inWidthRange(moverLeft, moverRight, blockerLeft, blockerRight))
			return true;
		break;
	}
	return false;
}

// Checks if two actors overlap
bool StudentWorld::doesOverlap(const Actor* overlapper, const double overlapeeX, const double overlapeeY) {
	double overlapeeCenterX = (2 * overlapeeX + SPRITE_WIDTH - 1) / 2;
	double overlapeeCenterY = (2 * overlapeeY + SPRITE_HEIGHT - 1) / 2;
	double overlapperCenterX = (2 * overlapper->getX() + SPRITE_WIDTH - 1) / 2;
	double overlapperCenterY = (2 * overlapper->getY() + SPRITE_HEIGHT - 1) / 2;

	return (pow(overlapperCenterX - overlapeeCenterX, 2) +
		pow(overlapperCenterY - overlapeeCenterY, 2) <= 100);
}
bool StudentWorld::overlapsFlameBlocker(double xPos, double yPos) {
	list<Actor*>::iterator it = allActors.begin();
	while (it != allActors.end()) {
		if ((*it)->isAlive() && (*it)->blocksFlame() && doesOverlap(*it, xPos, yPos))
			return true;
		it++;
	}
	return false;
}
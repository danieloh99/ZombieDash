#include "StudentWorld.h"
#include "GameConstants.h"
#include "Level.h"
#include "Actor.h"
#include <vector>
#include <string>
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

	// Probably want to change this, if level is finished. Use getLevel() in GameWorld.h
	string levelFile = "level01.txt"; 

	Level::LoadResult result = lev.loadLevel(levelFile); // Attempt to load the level

	// If level doesn't load:
	if (result == Level::load_fail_file_not_found)
		cerr << "Cannot find " << levelFile << " data file" << endl;
	else if (result == Level::load_fail_bad_format)
		cerr << "Your level was improperly formatted" << endl;
	// If the level does load:
	else if (result == Level::load_success) {
		cerr << "Successfully loaded level" << endl;

		int numObjectsCreated = 0; // DEBUGGING PURPOSES

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
				case Level::player: {
					cerr << " is where Penelope starts." << endl;
					Player* penelope = new Player(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT);
					allActors.push_back(penelope);
					numObjectsCreated++; // DEBUGGING PURPOSES
					break;
				}
				case Level::wall: {
					cerr << " holds a wall." << endl;
					Wall* wall = new Wall(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT);
					allActors.push_back(wall);
					numObjectsCreated++; // DEBUGGING PURPOSES
					break;
				}
				}
			}
		} // Finish reading level
		cerr << "Num Objects Created: " << numObjectsCreated << endl; // DEBUGGING PURPOSES
	}
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
	for (unsigned int i = 0; i < allActors.size(); i++) {
		allActors[i]->doSomething();
	}
	return GWSTATUS_CONTINUE_GAME;
    decLives();
    return GWSTATUS_PLAYER_DIED;
}

void StudentWorld::cleanUp()
{
	int numObjectsDestroyed = 0; // DEBUGGING PURPOSES
	vector<Actor*>::iterator it = allActors.begin();
	while (it != allActors.end()) {
		delete *it;
		it = allActors.erase(it);
		numObjectsDestroyed++; // DEBUGGING PURPOSES
	}
	cerr << "Num objects destroyed: " << numObjectsDestroyed << endl; // DEBUGGING PURPOSES
	cout << "Vector size after destruction: " << allActors.size() << endl; // DEBUGGING PURPOSES
}

bool StudentWorld::blockingActorExists(const Actor* actor) {

	// Get actor's direction and position
	Direction dir = actor->getDirection();
	double actorBottom = actor->getY();
	double actorTop = actor->getY() + SPRITE_HEIGHT - 1;
	double actorLeft = actor->getX();
	double actorRight = actor->getX() + SPRITE_WIDTH - 1;

	// Temporary boundaries
	double blockerBottom = 0;
	double blockerTop = 0;
	double blockerLeft = 0;
	double blockerRight = 0;

	vector<Actor*>::iterator it = allActors.begin();

	while (it != allActors.end()) { 
		// Go through Actors until reaching an actor that blocks movement
		if ((*it)->blocksMovement()) {

			// Store boundaries of blocker
			blockerBottom = (*it)->getY();
			blockerTop = (*it)->getY() + SPRITE_HEIGHT - 1;
			blockerLeft = (*it)->getX();
			blockerRight = (*it)->getX() + SPRITE_WIDTH - 1;

			switch (dir) {

			case actor->left:
				// If the blocker's right side is past the left side of the
				// actor's current position, ignore the blocker.
				if (blockerRight >= actorLeft)
					break;
				// If the blocker's right side is equal to or past the left side of the
				// actor's new position and actor's yPos is within the blocker's y-Range, return true.
				if (blockerRight >= actorLeft - 4 &&
					( inHeightRange(actorBottom, actorTop, blockerBottom, blockerTop) ))
					return true;
				// Hello

			case actor->right:
				// If the blocker's left side is past the right side of the
				// actor's current position, ignore the blocker.
				if (blockerLeft <= actorRight)
					break;
				// If the blocker's left side is equal to or past the right side of the
				// actor's new position and actor's yPos is within the blocker's y-Range, return true.
				if (blockerLeft <= actorRight + 4 &&
					( inHeightRange(actorBottom, actorTop, blockerBottom, blockerTop) ) )
					return true;
			}
		}
		it++;
	}

	return false;
}

// Private function, checks if mover's yPos is within the blocker's y-Range
bool StudentWorld::inHeightRange(double moverBottom, double moverTop, double blockerBottom, double blockerTop) const {
	return ( (moverBottom >= blockerBottom && moverBottom <= blockerTop) ||
		(moverTop >= blockerBottom && moverTop <= blockerTop) );
}

// Private function, checks if mover's xPos is within the blocker's x-Range
bool StudentWorld::inWidthRange(double moverLeft, double moverRight, double blockerLeft, double blockerRight) const {
	return ( (moverLeft >= blockerLeft && moverLeft <= blockerRight) ||
		(moverRight >= blockerLeft && moverRight <= blockerRight) );
}
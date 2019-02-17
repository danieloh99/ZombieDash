#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
#include <iostream>
#include "GameWorld.h"
class StudentWorld;
using namespace std;

// Actor constructor
Actor::Actor(StudentWorld* world, int imageID, double startX, double startY, Direction dir = right, int depth = 0) 
	: m_world(world), GraphObject(imageID, startX, startY, dir, depth) {}
bool Actor::blocksMovement() { return false; }

// Player implementations
Player::Player(StudentWorld* world, double startX, double startY) 
	: Actor(world, IID_PLAYER, startX, startY) {}
Player::~Player() { 
	cerr << "Destroying Penelope at " << getX() << "," << getY() << endl;
}
bool Player::blocksMovement() { return true; }

void Player::doSomething() {
	// Check if she's alive
		// if not, return immediately
	// Check if she's infected
		// if so ...

	int ch;
	if (getWorld()->getKey(ch))
	{
		// user hit a key during this tick!
		switch (ch)
		{
		/*
		i. Set Penelope’s direction to the specified movement direction.

		ii. Determine Penelope’s destination location (dest_x, dest_y) which
		will be exactly 4 pixels in the direction Penelope is facing. So for
		example, if Penelope is at (x=16, y=16) and is facing down, her
		destination would be (dest_x=16, dest_y=12).

		iii. If the movement to (dest_x, dest_y) would not cause Penelope’s
		bounding box to intersect with the bounding box3 of any wall,
		citizen or zombie objects, then update Penelope’s location to the
		specified location with the GraphObject class’s moveTo() method.
		*/
		case KEY_PRESS_LEFT:
			cerr << "left" << endl;

			GraphObject::setDirection(left);
			// if (there are no blocking Actors at xpos getX() - 4)
			if (getWorld()->blockingActorExists(this) == false)
				GraphObject::moveTo(getX() - 4, getY());
			break;
		case KEY_PRESS_RIGHT:
			cerr << "right" << endl;

			GraphObject::setDirection(right);
			// if (there are no objects with depth 0 and x-pos: getX() + 4)
			if (getWorld()->blockingActorExists(this) == false)
				GraphObject::moveTo(getX() + 4, getY());
			
			break;
		case KEY_PRESS_DOWN:
			cerr << "down" << endl;

			GraphObject::setDirection(down);
			// if (there are no objects with depth 0 and y-pos: getY() - 4)
			if (getWorld()->blockingActorExists(this) == false)
				GraphObject::moveTo(getX(), getY() - 4);
			break;
		case KEY_PRESS_UP:
			cerr << "up" << endl;

			GraphObject::setDirection(up);
			// if (there are no objects with depth 0 and y-pos: getY() + 4)
			if (getWorld()->blockingActorExists(this) == false)
				GraphObject::moveTo(getX(), getY() + 4);
			break;
		case KEY_PRESS_SPACE:
			cerr << "space" << endl;
			// add flames in front of Penelope
			break;
		}
	}
	return;
}



// Wall implementations
Wall::Wall(StudentWorld* world, double posX, double posY)
	: Actor(world, IID_WALL, posX, posY) {}

Wall::~Wall()
{
	cerr << "Destroying Wall at " << getX() << "," << getY() << endl;
}
bool Wall::blocksMovement() { return true; }
void Wall::doSomething() { return; }


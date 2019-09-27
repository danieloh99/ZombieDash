#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
#include <iostream>
#include "GameWorld.h"
using namespace std;

// Actor implementations
Actor::Actor(StudentWorld* world, int imageID, double startX, double startY, 
	bool canDie, Direction dir, int depth) : m_world(world), m_alive(true), 
	GraphObject(imageID, startX, startY, dir, depth), m_canDie(canDie) {}

// Wall implementations
Wall::Wall(StudentWorld* world, double xPos, double yPos)
	: Actor(world, IID_WALL, xPos, yPos) {}
Wall::~Wall() { cerr << "Destroying Wall at " << getX() << "," << getY() << endl; }

// Activatable implementations: base for all objects that check if they overlap with player
Activatable::Activatable(StudentWorld* world, int imageID, double xPos, double yPos, 
	bool canDie, Direction dir, int depth) : Actor(world, imageID, xPos, yPos, canDie, dir, depth) {}
Activatable::~Activatable() {}
void Activatable::doSomething() {
	if (!isAlive())
		return;
	if (doFirst())
		return;
	if (getWorld()->overlapsWithPlayer(this)) {
		doSomethingOverlapPlayer();
	}
	doRest();
}

// Mover implementations: base for Humans and Zombies
void Mover::move(Direction dir) {
	if (dir == right) moveTo(getX() + getTravelDist(), getY());
	else if (dir == up) moveTo(getX(), getY() + getTravelDist());
	else if (dir == left) moveTo(getX() - getTravelDist(), getY());
	else moveTo(getX(), getY() - getTravelDist());
}
void Mover::doSomething() {
	if (!isAlive()) return;
	if (doBeforeParalysis())
		return;
	if (checkParalysis())
		return;
	doAfterParalysis();
}
bool Mover::checkParalysis() {
	m_paralyzed = !m_paralyzed;
	if (m_paralyzed) return true;
	return false;
}

// Human implementations: base for Player and Citizen
bool Human::doBeforeParalysis() {
	// cerr << "Checking if human is infected.\n";
	if (isInfected()) {
		// cerr << "Human is infected. Increasing infection count.\n";
		infectionCount += 1;
	}
	return diedFromInfection();
}

// Player implementations
Player::Player(StudentWorld* world, double startX, double startY) 
	: numLandmines(0), numFlames(0), numVaccines(0), Human(world, IID_PLAYER, startX, startY, 4) {}
Player::~Player() { cerr << "Destroying Player at " << getX() << "," << getY() << endl; }

// Getters
int Player::getLandmines() const { return numLandmines; }
int Player::getFlames() const {	return numFlames; }
int Player::getVaccines() const { return numVaccines; }
// Setters (if player uses resource, pass in negative integer)
void Player::incLandmines(int howMuch) { numLandmines += howMuch; }
void Player::incFlames(int howMuch) { numFlames += howMuch; }
void Player::incVaccines(int howMuch) { numVaccines += howMuch; }

bool Player::diedFromInfection() {
	if (getInfectionCount() == 500) {
		getWorld()->killPlayer();
		return true;
	}
	return false;
}
void Player::doAfterParalysis() {
	// If user presses a key during the tick
	int ch;
	if (getWorld()->getKey(ch))
	{
		switch (ch)
		{
		// For movement cases, set the desired direction, move if possible.
		case KEY_PRESS_LEFT:
			//cerr << "left" << endl;
			setDirection(left);
			if (!getWorld()->blockingActorExists(getX(), getY(), getTravelDist(), left))
				move(left);
			break;
		case KEY_PRESS_RIGHT:
			//cerr << "right" << endl;
			setDirection(right);
			if (!getWorld()->blockingActorExists(getX(), getY(), getTravelDist(), right))
				move(right);
			break;
		case KEY_PRESS_DOWN:
			//cerr << "down" << endl;
			setDirection(down);
			if (!getWorld()->blockingActorExists(getX(), getY(), getTravelDist(), down))
				move(down);
			break;
		case KEY_PRESS_UP:
			//cerr << "up" << endl;
			setDirection(up);
			if (!getWorld()->blockingActorExists(getX(), getY(), getTravelDist(), up))
				move(up);
			break;
		case KEY_PRESS_SPACE:
			cerr << "space: Fire!" << endl;
			if (numFlames > 0) {
				numFlames--;
				getWorld()->playSound(SOUND_PLAYER_FIRE);
				useFlamethrower();
				cerr << "flames created!" << endl;
			}
			break;
		case KEY_PRESS_TAB:
			cerr << "tab: Landmine!" << endl;
			if (numLandmines > 0) {
				getWorld()->addLandmine(getX(), getY());
				cerr << "landmine placed!" << endl;
				numLandmines--;
			}
			break;
		case KEY_PRESS_ENTER:
			cerr << "enter: Vaccine!" << endl;
			if (numVaccines > 0) {
				setInfectionStatus(false);
				setInfectionCount(0);
				cerr << "vaccine used!" << endl;
				numVaccines--;
			}
			break;
		}
	}
	return;
}
void Player::useFlamethrower() {
	//Direction dir = getDirection();
	/*double xPos = getX();
	double yPos = getY();*/
	double nextFlamePos;
	switch (getDirection()) {
	case right:
		for (int i = 1; i <= 3; i++) {
			nextFlamePos = getX() + i * SPRITE_WIDTH;
			if (!getWorld()->attemptAddFlame(nextFlamePos, getY(), right))
				break;
		}
		break;
	case left:
		for (int i = 1; i <= 3; i++) {
			nextFlamePos = getX() - i * SPRITE_WIDTH;
			if (!getWorld()->attemptAddFlame(nextFlamePos, getY(), left))
				break;
		}
		break;
	case up:
		for (int i = 1; i <= 3; i++) {
			nextFlamePos = getY() + i * SPRITE_WIDTH;
			if (!getWorld()->attemptAddFlame(getX(), nextFlamePos, up))
				break;
		}
		break;
	case down:
		for (int i = 1; i <= 3; i++) {
			nextFlamePos = getY() - i * SPRITE_WIDTH;
			if (!getWorld()->attemptAddFlame(getX(), nextFlamePos, down))
				break;
		}
		break;
	}
}

// Citizen implementations
Citizen::Citizen(StudentWorld* world, double startX, double startY)
	: Human(world, IID_CITIZEN, startX, startY, 2) {}
Citizen::~Citizen() { 
	cerr << "Destroying Citizen at " << getX() << "," << getY() << endl;
	getWorld()->incNumCitizens(-1);
}

bool Citizen::diedFromInfection() {
	if (getInfectionCount() == 500) {
		setDead();
		getWorld()->playSound(SOUND_ZOMBIE_BORN); // Play sound effect
		getWorld()->increaseScore(-1000); // Decrease score by 1000
		// Add zombie object at Citizen's position to Student World
		// 70% chance dumb zombie, 30% chance smart.
		int rand = randInt(1, 10);
		if (rand <= 7) { getWorld()->addDumbZombie(getX(), getY());	}
		else { getWorld()->addSmartZombie(getX(), getY()); }
		return true;
	}
	return false;
}
void Citizen::doAfterParalysis() {
	//cerr << "Determining distance from Penelope.\n";
	double distP = getWorld()->distToPlayer(getX(),getY());
	// Step 5: Determine distance from closest zombie
	double distZ = -1; // function should return -1 if there are no zombies

	// Step 6: Determine citizen's movement when citizen closer to Player than zombies
	bool moveBlocked = false;
	if ((distP < distZ || distZ == -1) && distP <= 80) { // 80 is the constant set in Specs
		
		int dirToPlayer;
		int xDirToPlayer = getWorld()->getXDirToPlayer(getX());
		int yDirToPlayer = getWorld()->getYDirToPlayer(getY());

		// If citizen is in same row or col as player
		if (xDirToPlayer == -1 || yDirToPlayer == -1) { 
			if (yDirToPlayer == -1) dirToPlayer = xDirToPlayer; // Same row
			else dirToPlayer = yDirToPlayer; // Same col

			// If the citizen can move 2 pixels toward the player
			if ( !(getWorld()->blockingActorExists(getX(), getY(), 2, dirToPlayer)) ) {
				setDirection(dirToPlayer); // Set citizen direction
				move(dirToPlayer);
				return;
			}
			moveBlocked = true;
		}
		// If citizen is not in same row or col as player
		else {
			// Randomly choose 1 of the two possible directions that the
			// citizen can move
			int randNum = randInt(0, 1);
			if (randNum == 0) dirToPlayer = xDirToPlayer;
			else dirToPlayer = yDirToPlayer;

			// If the citizen can move in the randomly chosen direction, we're good.
			if (!getWorld()->blockingActorExists(getX(), getY(), 2, dirToPlayer)) {}
			// If not, if the citizen can move in the other chosen direction, we're good.
			else if (dirToPlayer == yDirToPlayer && !getWorld()->blockingActorExists(getX(), getY(), 2, xDirToPlayer))
				dirToPlayer = xDirToPlayer;
			else if (dirToPlayer == xDirToPlayer && !getWorld()->blockingActorExists(getX(), getY(), 2, yDirToPlayer))
				dirToPlayer = yDirToPlayer;
			// If the citizen can't move in either direction, we're not done.
			else
				moveBlocked = true;

			if (!moveBlocked) {
				setDirection(dirToPlayer);
				move(dirToPlayer);
				return;
			}
		}
	}
	// Step 7: Determine citizen's movement when zombie is close.
	Direction newDir = getWorld()->getDirAwayClosestZombie(getX(), getY(), getTravelDist());
	if (newDir == -1) return;
	setDirection(newDir);
	move(getDirection());
	return;
}

// Zombie implementations
void Zombie::doAfterParalysis() {
	// Check if a Human is in front of the zombie
	
	//a. It will compute vomit coordinates in the direction it is facing,
	//SPRITE_WIDTH pixels away if it is facing left or right, or
	//SPRITE_HEIGHT pixels away if it is facing up or down. So if the smart
	//zombie is at position (x,y) facing left, it would compute the vomit
	//coordinates (x-SPRITE_WIDTH, y), i.e., (x-16, y).
	Direction dir = getDirection();
	double vomitX = getX();
	double vomitY = getY();
	switch (dir) {
	case right:
		vomitX += SPRITE_WIDTH;
		break;
	case left:
		vomitX -= SPRITE_WIDTH;
		break;
	case up:
		vomitY += SPRITE_HEIGHT;
		break;
	case down:
		vomitY -= SPRITE_HEIGHT;
		break;
	}
	if (getWorld()->attemptAddVomit(vomitX, vomitY, dir))
		return;
	
	if (movePlan == 0) {
		movePlan = randInt(3, 10);
		setRandDir(); // Pure virtual for different Zombie types.
	}
	if (!getWorld()->blockingActorExists(getX(), getY(), 1, getDirection())) {
		move(getDirection());
		movePlan--;
	}
	else movePlan = 0;
}

// Dumb Zombie implementations
Dumb::~Dumb() { cerr << "Destroying Dumb Zombie (+1000) at " << getX() << "," << getY() << endl; }
void Dumb::setRandDir() { setDirection(randInt(0,3) * 90); }
void Dumb::setDead() {
	Actor::setDead();
	int rand = randInt(1, 10);
	if (rand == 1) {
		switch (getDirection()) {
		case left:
			getWorld()->attemptAddVaccine(getX() - SPRITE_WIDTH, getY());
			break;
		case right:
			getWorld()->attemptAddVaccine(getX() + SPRITE_WIDTH, getY());
			break;
		case up:
			getWorld()->attemptAddVaccine(getX(), getY() + SPRITE_HEIGHT);
			break;
		case down:
			getWorld()->attemptAddVaccine(getX(), getY() - SPRITE_HEIGHT);
			break;
		}
	}
	getWorld()->playSound(SOUND_ZOMBIE_DIE);
	getWorld()->increaseScore(1000);
}
// Smart Zombie implementations
Smart::~Smart() { cerr << "Destroying Smart Zombie (+2000) at " << getX() << "," << getY() << endl; }
void Smart::setRandDir() { setDirection(getWorld()->getDirToClosestHuman(getX(), getY())); }
void Smart::setDead() {
	Actor::setDead();
	getWorld()->playSound(SOUND_ZOMBIE_DIE);
	getWorld()->increaseScore(2000);
}

// Exit implementations
Exit::~Exit() { cerr << "Destroying Exit at " << getX() << "," << getY() << endl; }
void Exit::doSomethingOverlapPlayer() {
	// If there are no citizens remaining
	if (getWorld()->getNumCitizens() == 0) {
		cerr << "Player exitted" << endl;
		getWorld()->playSound(SOUND_LEVEL_FINISHED);
		getWorld()->setFinishedLevelTrue();
	}
}
void Exit::doRest() { getWorld()->saveOverlapCitizens(this); }

// Pit implementations
Pit::Pit(StudentWorld* world, double xPos, double yPos)
	: Activatable(world, IID_PIT, xPos, yPos, false, right, 0) {}
Pit::~Pit() { cerr << "Destroying Pit at " << getX() << ", " << getY() << ".\n"; }
void Pit::doSomethingOverlapPlayer() { getWorld()->killPlayer(); }
void Pit::doRest() { getWorld()->killOverlapMovers(this); }

// Projectile implementation
Projectile::Projectile(StudentWorld* world, int imageID, double xPos, double yPos,
	Direction dir, int lifespan, int depth)
	: Activatable(world, imageID, xPos, yPos, false, dir, 0), m_lifespan(lifespan) {}
Projectile::~Projectile() {}

bool Projectile::doFirst() {
	if (m_lifespan > 0) {
		m_lifespan--;
		if (m_lifespan == 0) {
			setDead();
			return true;
		}
	}
	return false;
}

// Vomit implementation
Vomit::Vomit(StudentWorld* world, double xPos, double yPos, Direction dir)
	: Projectile(world, IID_VOMIT, xPos, yPos, dir) {}
Vomit::~Vomit() {cerr << "Destroying Vomit at " << getX() << ", " << getY() << ".\n"; }
void Vomit::doSomethingOverlapPlayer() { getWorld()->infectPlayer(); }
void Vomit::doRest() { getWorld()->infectOverlapCitizens(this); }

// Flame implementation
Flame::Flame(StudentWorld* world, double xPos, double yPos, Direction dir)
	: Projectile(world, IID_FLAME, xPos, yPos, dir) {}
Flame::~Flame() { cerr << "Destroying Flame at " << getX() << "," << getY() << endl; }
void Flame::doSomethingOverlapPlayer() { getWorld()->killPlayer(); }
void Flame::doRest() { getWorld()->killOverlapDamageables(this); }

// Landmine implementation
Landmine::Landmine(StudentWorld* world, double xPos, double yPos)
	: Activatable(world, IID_LANDMINE, xPos, yPos, true, right, 1), m_ticks(30) {}
Landmine::~Landmine() { cerr << "Destroying Landmine at " << getX() << ", " << getY() << ".\n"; }
void Landmine::setDead() {
	Actor::setDead();
	getWorld()->playSound(SOUND_LANDMINE_EXPLODE);

	// Introduce a flame object at the same and surrounding locations as the landmine
	getWorld()->attemptAddFlame(getX(), getY(), up); 
	getWorld()->attemptAddFlame(getX() + SPRITE_WIDTH, getY(), up); // east
	getWorld()->attemptAddFlame(getX() - SPRITE_WIDTH, getY(), up); // west
	getWorld()->attemptAddFlame(getX(), getY() + SPRITE_HEIGHT, up); // north
	getWorld()->attemptAddFlame(getX(), getY() - SPRITE_HEIGHT, up); // south
	getWorld()->attemptAddFlame(getX() + SPRITE_WIDTH, getY() + SPRITE_HEIGHT, up); // north-east
	getWorld()->attemptAddFlame(getX() - SPRITE_WIDTH, getY() + SPRITE_HEIGHT, up); // north-west
	getWorld()->attemptAddFlame(getX() - SPRITE_WIDTH, getY() - SPRITE_HEIGHT, up); // south-west
	getWorld()->attemptAddFlame(getX() + SPRITE_WIDTH, getY() - SPRITE_HEIGHT, up); // south-east

	// Introduce a pit object at the same location as the landmine
	getWorld()->addPit(getX(), getY());
}
bool Landmine::doFirst() { 
	if (m_ticks > 0) {
		m_ticks--;
		return true;
	}
	return false;
}
void Landmine::doSomethingOverlapPlayer() {
	getWorld()->killPlayer();
	setDead();
}
void Landmine::doRest() {
	if (getWorld()->killOverlapMovers(this)) {
		setDead();
	}
}

// Goodie implementations
Goodie::Goodie(StudentWorld* world, int imageID, double xPos, double yPos)
	: Activatable(world, imageID, xPos, yPos, true, right, 1) {}
Goodie::~Goodie() {}
void Goodie::doSomethingOverlapPlayer() {
	cerr << "Goodie Overlaps with player." << endl;
	getWorld()->increaseScore(50);
	setDead();
	getWorld()->playSound(SOUND_GOT_GOODIE);
	doSpecific();
}
// Vaccine implementations
Vaccine::Vaccine(StudentWorld* world, double xPos, double yPos)
	: Goodie(world, IID_VACCINE_GOODIE, xPos, yPos) {}
Vaccine::~Vaccine() { cerr << "Destroying Vaccine at " << getX() << "," << getY() << ".\n"; }
void Vaccine::doSpecific() {
	cerr << "Giving player vacc" << endl;
	getWorld()->givePlayerVacc(1);
}
// GasCan implementations
GasCan::GasCan(StudentWorld* world, double xPos, double yPos)
	: Goodie(world, IID_GAS_CAN_GOODIE, xPos, yPos) {}
GasCan::~GasCan() { cerr << "Destroying GasCan at " << getX() << "," << getY() << ".\n"; }
void GasCan::doSpecific() {
	getWorld()->givePlayerGasCan(5);
}
// Landmine Goodie implementations
LandmineGoodie::LandmineGoodie(StudentWorld* world, double xPos, double yPos)
	: Goodie(world, IID_LANDMINE_GOODIE, xPos, yPos) {}
LandmineGoodie::~LandmineGoodie() { cerr << "Destroying Landmine Goodie at " << getX() << "," << getY() << ".\n"; }
void LandmineGoodie::doSpecific() {
	getWorld()->givePlayerLandmine(2);
}
#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class StudentWorld;

class Actor : public GraphObject {
public:
	Actor(StudentWorld* world, int imageID, double startX, double startY, 
		bool canDie = false, Direction dir = right, int depth = 0);
	virtual ~Actor() {}
	StudentWorld* getWorld() const { return m_world; }

	bool isAlive() const { return m_alive; }
	virtual void setDead() { m_alive = false; }
	bool isDamageable() const { return m_canDie; }

	virtual bool isInfected() const { return false; }
	virtual void setInfectionStatus(bool status) { return; }
	virtual bool blocksMovement() const = 0;
	virtual bool canExit() const { return false; }
	virtual bool canInfect() const { return false; }
	virtual bool canMove() const = 0;
	virtual bool blocksFlame() const { return false; }

	virtual void doSomething() = 0;
private:
	StudentWorld* m_world;
	bool m_alive;
	bool m_canDie;
};

class Wall : public Actor {
public:
	Wall(StudentWorld* world, double xPos, double yPos);
	virtual ~Wall();
private:
	virtual bool blocksMovement() const { return true; }
	virtual bool canMove() const { return false; }
	virtual bool blocksFlame() const { return true; }

	virtual void doSomething() { return; }
};

// Abstract Base for all objects that check for overlap with Player
class Activatable : public Actor {
public:
	Activatable(StudentWorld* world, int imageID, double xPos, double yPos, bool canDie, Direction dir, int depth);
	virtual ~Activatable();

private:
	// Inherited from Actor
	virtual bool blocksMovement() const { return false; }
	virtual bool canMove() const { return false; }
	virtual void doSomething();

	// New functions
	virtual bool doFirst() = 0;
	virtual void doSomethingOverlapPlayer() = 0;
	virtual void doRest() = 0;
};

// Abstract Base for Moving objects
class Mover : public Actor {
public:
	Mover(StudentWorld* world, int imageID, double startX, double startY, double travelDist)
		: Actor(world, imageID, startX, startY, true), m_paralyzed(false), m_travelDist(travelDist) {}
	virtual ~Mover() {}

	double getTravelDist() const { return m_travelDist; }
	//void setTravelDist(double newDist) { m_travelDist = newDist; }
	double isParalyzed() const { return m_paralyzed; }
	void setParalyzed(bool status) { m_paralyzed = status; }
	void move(Direction dir);

	virtual void doSomething(); // must be public, used by m_penelope pointer in StudentWorld

private:
	// Inherited from Actor
	virtual bool blocksMovement() const { return true; }
	virtual bool canMove() const { return true; }

	// New functions
	virtual bool doBeforeParalysis() = 0;
	virtual bool checkParalysis();
	virtual void doAfterParalysis() = 0;

	bool m_paralyzed;
	double m_travelDist;
};

// Abstract Base for Dumb Zombies and Smart Zombies.
class Zombie : public Mover {
public:
	Zombie(StudentWorld* world, double startX, double startY)
		: Mover(world, IID_ZOMBIE, startX, startY, 1), movePlan(0) {}
	virtual ~Zombie() {}
	// Technically unnecessary for now, but could be useful in 'future' scenario
		//int getMovementPlan() const { return movePlan; }
		//void setMovementPlan(int value) { movePlan = value; }
private:
	int movePlan;

	// Actor functions
	virtual bool canInfect() const { return true; }

	// Mover functions
	virtual bool doBeforeParalysis() { return false; }
	virtual void doAfterParalysis();

	// New functions
	virtual void setRandDir() = 0;
};

// Abstract Base for Player and Citizen classes.
class Human : public Mover {
public:
	// Constructor, Destructor
	Human(StudentWorld* world, int imageID, double startX, double startY, double travelDist)
		: Mover(world, imageID, startX, startY, travelDist), infected(false), infectionCount(0) {}
	virtual ~Human() {}

	// Getters
	int getInfectionCount() const { return infectionCount; }
	// Setters
	virtual void setInfectionStatus(bool status) { infected = status; }
	void setInfectionCount(int value) { infectionCount = value; }
private:
	bool infected;
	int infectionCount;

	// From Actor
	virtual bool canExit() const { return true; }
	// From Mover
	virtual bool doBeforeParalysis();
	// virtual bool doAfterParalysis() = 0;

	virtual bool diedFromInfection() = 0;
	virtual bool isInfected() const { return infected; }

};

class Player : public Human {
public:
	// Constructor, Destructor
	Player(StudentWorld* world, double startX, double startY);
	virtual ~Player();
	
	virtual bool checkParalysis() { return false; } // overwritten from mover

	// Getters
	int getLandmines() const;
	int getFlames() const;
	int getVaccines() const;
	// Setters (if player uses resource, pass in negative integer)
	void incLandmines(int howMuch);
	void incFlames(int howMuch);
	void incVaccines(int howMuch);
private:
	int numLandmines;
	int numFlames;
	int numVaccines;

	void useFlamethrower();

	virtual bool diedFromInfection();
	virtual void doAfterParalysis();
};

class Citizen : public Human {
public:
	Citizen(StudentWorld* world, double startX, double startY);
	virtual ~Citizen();
private:
	virtual bool diedFromInfection();
	virtual void doAfterParalysis();
};

class Dumb : public Zombie {
public:
	Dumb(StudentWorld* world, double xPos, double yPos)
		: Zombie(world, xPos, yPos) {}
	virtual ~Dumb();
private:
	virtual void setRandDir();
	virtual void setDead();
};
class Smart : public Zombie {
public:
	Smart(StudentWorld* world, double xPos, double yPos)
		: Zombie(world, xPos, yPos) {}
	virtual ~Smart();
private:
	virtual void setRandDir();
	virtual void setDead();
};

class Exit : public Activatable {
public:
	Exit(StudentWorld* world, double xPos, double yPos)
		: Activatable(world, IID_EXIT, xPos, yPos, false, right, 1) {}
	virtual ~Exit();
private:
	virtual bool doFirst() { return false; }
	virtual void doSomethingOverlapPlayer();
	virtual void doRest();
	virtual bool blocksFlame() const { return true; }
};

class Pit : public Activatable {
public:
	Pit(StudentWorld* world, double xPos, double yPos);
	virtual ~Pit();
private:
	virtual bool doFirst() { return false; } // does nothing first
	virtual void doSomethingOverlapPlayer();
	virtual void doRest();
};

class Projectile : public Activatable {
public:
	Projectile(StudentWorld* world, int imageID, double xPos, double yPos, 
		Direction dir = right, int lifespan = 2, int depth = 0);
	virtual ~Projectile();
private:
	int m_lifespan;
	virtual bool doFirst();
};
class Vomit : public Projectile {
public:
	Vomit(StudentWorld* world, double xPos, double yPos, Direction dir);
	virtual ~Vomit();
private:
	virtual void doSomethingOverlapPlayer();
	virtual void doRest();
};
class Flame : public Projectile {
public:
	Flame(StudentWorld* world, double xPos, double yPos, Direction dir);
	virtual ~Flame();
private:
	virtual void doSomethingOverlapPlayer();
	virtual void doRest();
};
class Landmine : public Activatable {
public:
	Landmine(StudentWorld* world, double xPos, double yPos);
	virtual ~Landmine();
private:
	int m_ticks;
	virtual void setDead();
	virtual bool doFirst();
	virtual void doSomethingOverlapPlayer();
	virtual void doRest();
};

// Abstract Base for all Goodies
class Goodie : public Activatable {
public:
	Goodie(StudentWorld* world, int imageID, double xPos, double yPos);
	virtual ~Goodie();
private:
	virtual void doSpecific() = 0;
	virtual bool doFirst() { return false; }
	virtual void doSomethingOverlapPlayer();
	virtual void doRest() { return; }
};

class Vaccine : public Goodie {
public:
	Vaccine(StudentWorld* world, double xPos, double yPos);
	virtual ~Vaccine();
private:
	virtual void doSpecific();
};

class GasCan : public Goodie {
public:
	GasCan(StudentWorld* world, double xPos, double yPos);
	virtual ~GasCan();
private:
	virtual void doSpecific();
};

class LandmineGoodie : public Goodie {
public:
	LandmineGoodie(StudentWorld* world, double xPos, double yPos);
	virtual ~LandmineGoodie();
private:
	virtual void doSpecific();
};
#endif // ACTOR_H_

#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
// GraphObject(int imageID, double startX, double startY, Direction dir = 0, int depth = 0, double size = 1.0)
class StudentWorld;
class Actor : public GraphObject {
public:
	Actor(StudentWorld* world, int imageID, double startX, double startY, Direction dir, int depth);
	virtual ~Actor() {}
	virtual void doSomething() = 0;
	virtual bool blocksMovement();
	virtual StudentWorld* getWorld() const { return m_world; }
private:
	StudentWorld* m_world;
};

class Player : public Actor {
public:
	Player(StudentWorld* world, double startX, double startY);
	virtual ~Player();
	virtual void doSomething();
	virtual bool blocksMovement();
};

class Wall : public Actor {
public:
	Wall(StudentWorld* world, double posX, double posY);
	virtual ~Wall();
	virtual void doSomething();
	virtual bool blocksMovement();
};
#endif // ACTOR_H_

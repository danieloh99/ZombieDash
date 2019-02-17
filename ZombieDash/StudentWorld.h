#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GraphObject.h" // for Direction
#include <string>
#include <vector>
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
	~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
	bool blockingActorExists(const Actor* actor); // this will be called for every moving Actor.
private:
	bool inHeightRange(double moverBottom, double moverTop, double blockerBottom, double blockerTop) const;
	bool inWidthRange(double moverLeft, double moverRight, double blockerLeft, double blockerRight) const;
	std::vector<Actor*> allActors; // The studentWorld should have a list of all the Actors.
};

#endif // STUDENTWORLD_H_

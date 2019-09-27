#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GraphObject.h" // for Direction
#include <string>
#include <list>
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class Player;
class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
	virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();

	int getNumCitizens() const { return numCitizens; }
	void incNumCitizens(int howMuch) { numCitizens += howMuch; }
	void setFinishedLevelTrue() { finishedLevel = true; }
	bool blockingActorExists(double moverX, double moverY, double travelDistance, Direction dir); // Determining blocking of movement

	// Consider changing to simply pass in position, rather than pointer to entire actor.
	void saveOverlapCitizens(const Actor* overlapee); // Kills all overlapping citizens.
	bool killOverlapMovers(const Actor* overlapee); // Kills all overlapping movers.
	bool killOverlapDamageables(const Actor* overlapee);
	void infectOverlapCitizens(const Actor* overlapee);

	bool overlapsWithPlayer(const Actor* overlapee); // Determines if actor overlaps with Player.
	void killPlayer();
	void infectPlayer();

	double distToPlayer(double xPos, double yPos);

	Direction getDirAwayClosestZombie(double xPos, double yPos, double travelDist);
	Direction getDirToClosestHuman(double xPos, double yPos);

	int getXDirToPlayer(double xPos);
	int getYDirToPlayer(double yPos);

	void givePlayerVacc(int howMuch);
	void givePlayerGasCan(int howMuch);
	void givePlayerLandmine(int howMuch);

	void addLandmine(double xPos, double yPos);
	void addPit(double xPos, double yPos);
	bool attemptAddFlame(double xPos, double yPos, Direction dir);
	bool attemptAddVomit(double xPos, double yPos, Direction dir);
	bool attemptAddVaccine(double xPos, double yPos);

	void addDumbZombie(double xPos, double yPos);
	void addSmartZombie(double xPos, double yPos);
private:
	Player* m_penelope;
	std::list<Actor*> allActors; // list of all the Actors (not including Penelope).

	int numCitizens;
	bool finishedLevel;

	// Determining direction
	int getXDirToActor(double xPos, const Actor* actor);
	int getYDirToActor(double yPos, const Actor* actor);
	int getXDirAwayActor(double xPos, const Actor* actor);
	int getYDirAwayActor(double yPos, const Actor* actor);
	double distToClosestCitizen(double xPos, double yPos);
	double distToClosestZombie(double xPos, double yPos);
	// Determining blocking of movement
	bool inHeightRange(double moverBottom, double moverTop, double blockerBottom, double blockerTop) const;
	bool inWidthRange(double moverLeft, double moverRight, double blockerLeft, double blockerRight) const;
	bool doesBlock(double blockerBot, double blockerTop, double blockerLeft,
		double blockerRight, double moverX, double moverY, double travelDistance, Direction dir) const;
	// Overlap
	bool overlapsFlameBlocker(double xPos, double yPos);
	bool doesOverlap(const Actor* overlapper, const double overlapeeX, const double overlapeeY);

	bool addedVomit(double xPos, double yPos, Direction dir);

	double computeDistance(double xDist, double yDist) { return (sqrt(pow(xDist, 2) + pow(yDist, 2))); }

};

#endif // STUDENTWORLD_H_

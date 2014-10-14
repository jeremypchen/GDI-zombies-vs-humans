#include "stdafx.h"
#include "ZombieWorld.h"
#include <sstream>

// Have to define static members
std::list<MachineState*> ZombieWorld::zombies;
std::list<MachineState*> ZombieWorld::humans;
static std::unordered_map<std::string, MachineState*> locationMap;
std::string ZombieWorld::zombieProgram;
std::string ZombieWorld::humanProgram;
int ZombieWorld::turns;

std::string createStringKey(int, int);

// Add a zombie. Push it onto list and update locationMap
void ZombieWorld::AddZombie(MachineState* zombie)
{
	zombies.push_back(zombie);
	UpdatePositions();
}

// Add a human. Push it onto list and update locationMap
void ZombieWorld::AddHuman(MachineState* human)
{
	humans.push_back(human);
	UpdatePositions();
}

// Given direction and location of attacker, attack victim
void ZombieWorld::AttackThingAt(MachineState::Facing direction, int x, int y, bool infected, bool ranged)
{
	// Depends on direction and if ranged attack.
	switch (direction)
	{
	case(MachineState::UP) :
		ranged ? y -= 2 : y--;
		break;
	case(MachineState::RIGHT) :
		ranged ? x += 2 : x++;
		break;
	case(MachineState::DOWN) :
		ranged ? y += 2 : y++;
		break;
	default:
	case(MachineState::LEFT) :
		ranged ? x -= 2 : x--;
		break;
	}

	// Create a string so we can use as a key for locationMap
	std::string key = createStringKey(x, y);

	// If there is something to attack 
	if (locationMap.find(key) != locationMap.end())
	{
		MachineState* victim = locationMap.at(key);
		// If attacker is zombie and victim is human
		if (infected && !victim->GetInfect())
		{
			victim->turnToZombie = TRUE;	 // So it can BindState and reset its state		
			AddZombie(victim);
			humans.remove(victim);
		} 
		// If attacker is human and victim is zombie
		else if (!infected && victim->GetInfect())
		{
			zombies.remove(victim);
		}
		// If attacker is human and victim is human
		else if (!infected && !victim->GetInfect())
		{
			humans.remove(victim);
		}
	}
	UpdatePositions();
}

// Update LocationMap
void ZombieWorld::UpdatePositions()
{
	// Clear it
	locationMap.clear();
	
	// For both zombies and humans, create keys based on their x,y coordiantes
	// so we can put them in the locationMap
	for (auto zombie : zombies)
	{
		std::string key = createStringKey(zombie->x, zombie->y);
		locationMap.emplace(key, zombie);
	}
	for (auto human : humans)
	{
		std::string key = createStringKey(human->x, human->y);
		locationMap.emplace(key, human);
	}
}

// Check if spot ahead is occupied, based on direction and location
bool ZombieWorld::IsOccupied(MachineState* state, int x, int y)
{
	std::string key;
	switch (state->m_Facing)
	{
	case(MachineState::UP) :
		y--;
		break;
	case(MachineState::RIGHT) :
		x++;
		break;
	case(MachineState::DOWN) :
		y++;
		break;
	default:
	case(MachineState::LEFT) :
		x--;
		break;
	}

	key = createStringKey(x, y);

	// If there is something there
	if (locationMap.find(key) != locationMap.end())
		return TRUE;
	return FALSE;
}

// Used for spawning zombies and humans and making sure that there isn't
// already a thing at the location given
bool ZombieWorld::IsOccupied(int x, int y)
{
	std::string key = createStringKey(x, y);
	if (locationMap.find(key) != locationMap.end())
		return TRUE;
	return FALSE;
}

// Return true if there is something there, false otherwise. Also update
// pointer m to the actual thing that is there
bool ZombieWorld::GetMachineAt(int x, int y, MachineState* m)
{
	// Not off the board
	if (x >= 10 && y >= 10)
	{
		// Convert the x,y values, which are mouse coordinates, 
		// into grid x,y coordinates
		x -= 10; y -= 10;
		x /= 30; y /= 30;
		std::string key = createStringKey(x, y);

		// Check to see if there is something there. If so, update m
		if (locationMap.find(key) != locationMap.end()){
			*m = *locationMap.at(key);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

// Check if there is a human ahead
bool ZombieWorld::IsHumanAhead(MachineState* state, int x, int y, int spotsAhead)
{
	std::string key;
	switch (state->m_Facing)
	{
	case(MachineState::UP) :
		y -= spotsAhead;
		break;
	case(MachineState::RIGHT) :
		x += spotsAhead;
		break;
	case(MachineState::DOWN) :
		y += spotsAhead;
		break;
	default:
	case(MachineState::LEFT) :
		x -= spotsAhead;
		break;
	}

	key = createStringKey(x, y);

	if (locationMap.find(key) != locationMap.end())
	{
		if (!locationMap.at(key)->GetInfect()) {
			return TRUE;
		}
	}
	return FALSE;
}

// Check if there is a zombie ahead
bool ZombieWorld::IsZombieAhead(MachineState* state, int x, int y, int spotsAhead)
{
	std::string key;
	switch (state->m_Facing)
	{
	case(MachineState::UP) :
		y -= spotsAhead;
		break;
	case(MachineState::RIGHT) :
		x += spotsAhead;
		break;
	case(MachineState::DOWN) :
		y += spotsAhead;
		break;
	default:
	case(MachineState::LEFT) :
		x -= spotsAhead;
		break;
	}

	key = createStringKey(x, y);

	if (locationMap.find(key) != locationMap.end())
	{
		if (locationMap.at(key)->GetInfect()) {
			return TRUE;
		}
	}
	return FALSE;
}

// Take turns and convert to a month.
// Return string as "Month (turns)"
std::string ZombieWorld::GetMonthAndTurn()
{
	std::stringstream monthAndTurn;
	std::string month;
	switch (turns%12){
	case 0: 
		month = "December";
		break;
	case 1:
		month = "January";
		break;
	case 2:
		month = "February";
		break;
	case 3:
		month = "March";
		break;
	case 4:
		month = "April";
		break;
	case 5:
		month = "May";
		break;
	case 6:
		month = "June";
		break;
	case 7:
		month = "July";
		break;
	case 8:
		month = "August";
		break;
	case 9:
		month = "September";
		break;
	case 10:
		month = "October";
		break;
	case 11:
		month = "November";
		break;
	}
	if (turns == 0) {
		month = "The Beginning";
	}
	monthAndTurn << month << " (" << turns << ")";

	return monthAndTurn.str();
}

// Function used to convert x,y coordinates into a string to be used as
// a key in locationMap
std::string createStringKey(int x, int y)
{
	std::stringstream coordinateKeyStream;
	coordinateKeyStream << "(" << x << "," << y << ")";

	return coordinateKeyStream.str();
}
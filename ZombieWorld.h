/* Singleton ZombieWorld */
#pragma once

#include "singleton.h"
#include "traits.h"
#include "machine.h"
#include <unordered_map>

struct ZombieTraits;
struct HumanTraits;

class ZombieWorld : public Singleton<ZombieWorld>
{
	DECLARE_SINGLETON(ZombieWorld);

	public:
		// Definition of functions and members in .cpp file
		static void AddZombie(MachineState* st);
		static void AddHuman(MachineState* st);
		static void AttackThingAt(MachineState::Facing, int x, int y, bool infected, bool ranged);
		static void UpdatePositions();
		static bool IsOccupied(MachineState*, int x, int y);
		static bool IsOccupied(int x, int y);
		static bool GetMachineAt(int x, int y, MachineState*);
		static bool IsHumanAhead(MachineState* st, int x, int y, int spotsAhead);
		static bool IsZombieAhead(MachineState* st, int x, int y, int spotsAhead);
		static std::string GetMonthAndTurn();

		static std::list<MachineState*> zombies;
		static std::list<MachineState*> humans;
		static std::string zombieProgram;
		static std::string humanProgram;
		static int turns;
};
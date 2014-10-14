#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "op.h"
#include "exceptions.h"

// Defines state data accessible by the machine and ops
struct MachineState
{
	// Friend Machine so BindState works
	template <typename MachineTraits>
	friend class Machine;
	
	enum Facing { UP, RIGHT, DOWN, LEFT};
	MachineState()
		: m_ProgramCounter(1)
		, m_ActionsTaken(0)
		, m_Facing(UP)
		, m_Test(false)
		, m_Memory(nullptr)
		, m_CurrentMem(0)
		, turnToZombie(false)
	{ }

	// Active line number in behavior program
	int m_ProgramCounter;
	// Number of actions taken this turn
	int m_ActionsTaken;
	// Current facing of this character
	Facing m_Facing;
	// Test flag for branches
	bool m_Test;
	// Current Memory slot
	int m_CurrentMem;
	// Location of in board
	int x, y; 
	// If human and attacked by zombie, convert to z!
	bool turnToZombie;

	int GetActionsPerTurn() const throw() { return m_ActionsPerTurn; }
	int GetMaxMemory() const throw() {return m_MaxMemory; }
	bool GetInfect() const throw() { return m_InfectOnAttack; }
	int GetMemory(int location)
	{
		try {
			if (GetMaxMemory() != 2 || (location != 0 && location != 1 )) {
				throw AccessViolation();
			} 
			else {
				return m_Memory[location];
			}
		}
		catch (std::exception& e){
			std::cout << e.what() << std::endl;
			return 0;
		}	
	}
	void SetMemory(int location, int value)
	{
		m_Memory[location] = value;
	}
private:
	// Data which is set by the traits
	int m_ActionsPerTurn;
	int m_MaxMemory;
	bool m_InfectOnAttack;
	int* m_Memory;
};

// Describes the machine which processes ops.
// Different policies dictate behavior possible for machine.
template <typename MachineTraits>
class Machine
{
public:
	// Load in all the ops for this machine from the specified file
	void LoadMachine(std::string& filename);

	// Given the state, binds the trait parameters to it
	void BindState(MachineState& state);

	// Take a turn using this logic for the passed in state
	void TakeTurn(MachineState& state);

private:
	// Push operation based on string operation and int parameter
	void GetAndPushOperation(std::string op, int param);

	std::vector<Op*> m_Ops;
};

template <typename MachineTraits>
void Machine<MachineTraits>::LoadMachine(std::string& filename)
{
	m_Ops.clear();

	std::ifstream fin(filename);
	try {
		if (fin.is_open())
		{
			char line[256];
			char * save_ptr;

			std::string operation;
			int parameter;

			while (fin.getline(line, 256))
			{
				// Separate into operation and parameter if exists
				save_ptr = strtok(line, ", \t");
				operation = save_ptr;
				save_ptr = strtok(NULL, "\n");
				if (save_ptr != NULL) {
					parameter = atoi(save_ptr);
				}
				else {
					parameter = 0;
				}

				GetAndPushOperation(operation, parameter);
			}
		} 
		else {
			throw InvalidFile();
		}
	}
	catch (std::exception& e){
		std::cout << e.what() << std::endl;
	}

}

template <typename MachineTraits>
void Machine<MachineTraits>::GetAndPushOperation(std::string op, int parameter)
{
	try {
		if (op == "attack")
		{
			m_Ops.push_back(new OpAttack());
		}
		else if (op == "ranged_attack")
		{
			m_Ops.push_back(new OpRangedAttack());
		}
		else if (op == "rotate")
		{
			m_Ops.push_back(new OpRotate(parameter));
		}
		else if (op == "forward")
		{
			m_Ops.push_back(new OpForward());
		}
		else if (op == "endturn")
		{
			m_Ops.push_back(new OpEndTurn());
		}
		else if (op == "test_human")
		{
			m_Ops.push_back(new OpTestHuman(parameter));
		}
		else if (op == "test_wall")
		{
			m_Ops.push_back(new OpTestWall());
		}
		else if (op == "test_zombie")
		{
			m_Ops.push_back(new OpTestZombie(parameter));
		}
		else if (op == "test_random")
		{
			m_Ops.push_back(new OpTestRandom());
		}
		else if (op == "test_passable")
		{
			m_Ops.push_back(new OpTestPassable());
		}
		else if (op == "je")
		{
			m_Ops.push_back(new OpTestFlagTrueJump(parameter));
		}
		else if (op == "jne")
		{
			m_Ops.push_back(new OpTestFlagFalseJump(parameter));
		}
		else if (op == "goto")
		{
			m_Ops.push_back(new OpGoto(parameter));
		}
		else if (op == "mem")
		{
			m_Ops.push_back(new OpMem(parameter));
		}
		else if (op == "set")
		{
			m_Ops.push_back(new OpSet(parameter));
		}
		else if (op == "inc")
		{
			m_Ops.push_back(new OpInc());
		}
		else if (op == "dec")
		{
			m_Ops.push_back(new OpDec());
		}
		else if (op == "test_mem")
		{
			m_Ops.push_back(new OpTestMem(parameter));
		}
		else if (op == "save_loc")
		{
			m_Ops.push_back(new OpSaveLoc());
		}
		else
		{
			throw InvalidOp();
		}
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}

template <typename MachineTraits>
void Machine<MachineTraits>::BindState(MachineState& state)
{
	state.m_ActionsPerTurn = MachineTraits::ACTIONS_PER_TURN;
	state.m_MaxMemory = MachineTraits::MEMORY_LOCATIONS;
	state.m_InfectOnAttack = MachineTraits::INFECT_ON_ATTACK;
	delete state.m_Memory;
	state.m_Memory = new int[state.m_MaxMemory];
	for (int i = 0; i < state.m_MaxMemory; i++)
	{
		state.m_Memory[i] = 0;
	}
}

template <typename MachineTraits>
void Machine<MachineTraits>::TakeTurn(MachineState& state)
{
	state.m_ActionsTaken = 0;
	try {
		while (state.m_ActionsTaken < MachineTraits::ACTIONS_PER_TURN)
		{
			if (state.m_ProgramCounter < 1 || state.m_ProgramCounter > (int)m_Ops.size()){
				throw InvalidProgramCounter();
			}
			m_Ops.at(state.m_ProgramCounter - 1)->Execute(state);
		}
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		state.m_ProgramCounter = 1;
	}
}
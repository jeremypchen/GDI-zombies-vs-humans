#include "stdafx.h"
#include "op.h"
#include "machine.h"
#include "ZombieWorld.h"
#include <iostream>

bool TestWall(MachineState&);
bool IsPassable(MachineState&);

// Output state information for debugging purposes
void Op::DebugOutput(MachineState& state)
{
	if (m_Param != NULL) {
		std::cout << state.m_ProgramCounter << ":" << m_OpName << std::endl;
	}
	else {
		std::cout << state.m_ProgramCounter << ":" << m_OpName << "," << m_Param << std::endl;
	}
	std::cout << "X: " << state.x << ", Y: " << state.y << ", Facing: " << state.m_Facing << std::endl;
}

// Test if there is a wall in front
void OpTestWall::Execute(MachineState& state)
{
	if (TestWall(state)){
		state.m_Test = TRUE;
	}
	else {
		state.m_Test = FALSE;
	}
	state.m_ProgramCounter++;
}

void OpTestFlagTrueJump::Execute(MachineState& state)
{
	try {
		if (m_Param < 1) {
			throw InvalidOp();
		}
		if (state.m_Test == TRUE) {
			state.m_ProgramCounter = m_Param;
		}
		else {
			state.m_ProgramCounter++;
		}
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		state.m_ProgramCounter++;
	}
}

void OpTestFlagFalseJump::Execute(MachineState& state)
{
	try {
		if (m_Param < 1) {
			throw InvalidOp();
		}
		if (state.m_Test == FALSE) {
			state.m_ProgramCounter = m_Param;
		}
		else {
			state.m_ProgramCounter++;
		}
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		state.m_ProgramCounter++;
	}
}

// Move forward if tile ahead is open
void OpForward::Execute(MachineState& state)
{
	if (IsPassable(state))
	{
		switch (state.m_Facing)
		{
		case(MachineState::UP) :
			state.y--;
			break;
		case(MachineState::RIGHT) :
			state.x++;
			break;
		case(MachineState::DOWN) :
			state.y++;
			break;
		default:
		case(MachineState::LEFT) :
			state.x--;
			break;
		}

		// Update location map
		ZombieWorld::UpdatePositions();
	}
	state.m_ProgramCounter++;
	state.m_ActionsTaken++;
}

// Test if human is ahead
void OpTestHuman::Execute(MachineState& state)
{
	if (ZombieWorld::IsHumanAhead(&state, state.x, state.y, m_Param)) {
		state.m_Test = TRUE;
	}
	else {
		state.m_Test = FALSE;
	}
	state.m_ProgramCounter++;
}

// Test if zombie is ahead
void OpTestZombie::Execute(MachineState& state)
{
	if (ZombieWorld::IsZombieAhead(&state, state.x, state.y, m_Param)) {
		state.m_Test = TRUE;
	}
	else {
		state.m_Test = FALSE;
	}

	state.m_ProgramCounter++;
}

// Test if tile ahead is passable
void OpTestPassable::Execute(MachineState& state)
{
	state.m_Test = IsPassable(state);
	state.m_ProgramCounter++;
}

// Set random m_Test
void OpTestRandom::Execute(MachineState& state)
{
	if ((rand() % 100) < 49)
	{
		state.m_Test = TRUE;
	}
	else
	{
		state.m_Test = FALSE;
	}
	state.m_ProgramCounter++;
}

// Rotate - direction depends on if m_Param is 1 or 2
void OpRotate::Execute(MachineState& state)
{
	switch (state.m_Facing)
	{
	case (MachineState::UP):
		if (m_Param == 0)
		{
			state.m_Facing = MachineState::RIGHT;
		}
		else
		{
			state.m_Facing = MachineState::LEFT;
		}
		break;
	case (MachineState::RIGHT):
		if (m_Param == 0)
		{
			state.m_Facing = MachineState::DOWN;
		}
		else
		{
			state.m_Facing = MachineState::UP;
		}
		break;
	case (MachineState::DOWN):
		if (m_Param == 0)
		{
			state.m_Facing = MachineState::LEFT;
		}
		else
		{
			state.m_Facing = MachineState::RIGHT;
		}
		break;
	default:
	case (MachineState::LEFT):
		if (m_Param == 0)
		{
			state.m_Facing = MachineState::UP;
		}
		else
		{
			state.m_Facing = MachineState::DOWN;
		}
		break;
	}
	state.m_ProgramCounter++;
	state.m_ActionsTaken++;
}

// Go to line represented by m_param
void OpGoto::Execute(MachineState& state)
{
	state.m_ProgramCounter = m_Param;
}

// Attack!!!
void OpAttack::Execute(MachineState& state)
{
	ZombieWorld::AttackThingAt(state.m_Facing, state.x, state.y, state.GetInfect(), false);

	state.m_ProgramCounter++;
	state.m_ActionsTaken++;
}

// Ranged attack!!
void OpRangedAttack::Execute(MachineState& state)
{
	ZombieWorld::AttackThingAt(state.m_Facing, state.x, state.y, state.GetInfect(), true);

	state.m_ProgramCounter++;
	state.m_ActionsTaken++;
}

void OpEndTurn::Execute(MachineState& state)
{
	state.m_ActionsTaken = state.GetActionsPerTurn();
}

// Set current memory slot to m_Param
void OpMem::Execute(MachineState& state)
{
	try {
		if (m_Param != 0 && m_Param != 1){
			throw InvalidOp();
		}
		if (state.GetInfect()){
			throw ZombieMemory();
		}
		state.m_CurrentMem = m_Param;
		state.m_ProgramCounter++;
	}
	catch (std::exception& e){
		std::cout << e.what() << std::endl;
		state.m_ProgramCounter++;
	}

}

// Set memory in current memory slot to m_Param value
void OpSet::Execute(MachineState& state)
{
	try {
		if (state.m_CurrentMem != 0 && state.m_CurrentMem != 1){
			throw NoCurrentMemory();
		}
		if (state.GetInfect()){
			throw ZombieMemory();
		}
		int slot = state.m_CurrentMem;
		state.SetMemory(slot, m_Param);

		state.m_ProgramCounter++;
	}
	catch (std::exception& e){
		std::cout << e.what() << std::endl;
		state.m_ProgramCounter++;
	}
}

// Increment value in current memory slot
void OpInc::Execute(MachineState& state)
{
	try {
		if (state.m_CurrentMem != 0 && state.m_CurrentMem != 1){
			throw NoCurrentMemory();
		}
		if (state.GetInfect()){
			throw ZombieMemory();
		}
		int slot = state.m_CurrentMem;
		int value = state.GetMemory(slot);
		state.SetMemory(slot, ++value);

		state.m_ProgramCounter++;
	}
	catch (std::exception& e){
		std::cout << e.what() << std::endl;
		state.m_ProgramCounter++;
	}
}

// Decrement value in current memory slot
void OpDec::Execute(MachineState& state)
{
	try {
		if (state.m_CurrentMem != 0 && state.m_CurrentMem != 1){
			throw NoCurrentMemory();
		}
		if (state.GetInfect()){
			throw ZombieMemory();
		}
		int slot = state.m_CurrentMem;
		int value = state.GetMemory(slot);
		state.SetMemory(slot, --value);
		
		state.m_ProgramCounter++;
	}
	catch (std::exception& e){
		std::cout << e.what() << std::endl;
		state.m_ProgramCounter++;
	}
}

// If value in current memory slot is equal to m_Param, set m_test to true
void OpTestMem::Execute(MachineState& state)
{
	try {
		if (state.m_CurrentMem != 0 && state.m_CurrentMem != 1){
			throw NoCurrentMemory();
		}
		if (state.GetInfect()){
			throw ZombieMemory();
		}
		int slot = state.m_CurrentMem;
		int value = state.GetMemory(slot);

		if (value == m_Param){
			state.m_Test = TRUE;
		}
		else {
			state.m_Test = FALSE;
		}
		state.m_ProgramCounter++;
	}
	catch (std::exception& e){
		std::cout << e.what() << std::endl;
		state.m_ProgramCounter++;
	}
}

// Save location of current coordinates into memory
void OpSaveLoc::Execute(MachineState& state)
{
	try {
		if (state.GetInfect()){
			throw ZombieMemory();
		}
		state.SetMemory(0, state.x);
		state.SetMemory(1, state.y);

		state.m_ProgramCounter++;
	}
	catch (std::exception& e){
		std::cout << e.what() << std::endl;
		state.m_ProgramCounter++;
	}
}

bool TestWall(MachineState& state)
{
	bool atWall = FALSE;
	// If facing wall, set m_Test to true
	// Figure out which way machine is facing
	// If facing up, look at (x, y-1)
	// If facing right, look at (x+1, y)
	// If facing down, look at (x, y+1)
	// If facing left, look at (x-1, y)
	switch (state.m_Facing)
	{
	case(MachineState::UP) :
		state.y == 0 ? atWall = TRUE : atWall = FALSE;
		break;
	case(MachineState::RIGHT) :
		state.x == 19 ? atWall = TRUE : atWall = FALSE;
		break;
	case(MachineState::DOWN) :
		state.y == 19 ? atWall = TRUE : atWall = FALSE;
		break;
	default:
	case(MachineState::LEFT) :
		state.x == 0 ? atWall = TRUE : atWall = FALSE;
		break;
	}

	return atWall;
}

bool IsPassable(MachineState& state)
{	
	return (!TestWall(state) 
		&& !ZombieWorld::IsOccupied(&state, state.x, state.y));
}

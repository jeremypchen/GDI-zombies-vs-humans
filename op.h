// Defines all the classes for different operations
// which can be performed.
#pragma once

struct MachineState;

// Abstract Base Class
class Op
{
public:
	Op(const char* OpName, int parameter)
		: m_OpName(OpName)
		, m_Param(parameter)
	{ }

	Op(const char* OpName)
		: m_OpName(OpName)
	{ }

	void DebugOutput(MachineState& state);

	virtual void Execute(MachineState& state) = 0;
protected:
	int m_Param;
	const char* m_OpName;
};

// Defines the rotate operation
// Param of 0 means rotate clockwise
// Param of 1 means rotate counter-clockwise
struct OpRotate : Op
{
	OpRotate(int parameter)
		: Op("OpRotate", parameter)
	{ }

	virtual void Execute(MachineState& state);
};

// Defines the goto operation
// Parameter determines the line number
struct OpGoto : Op
{
	OpGoto(int parameter)
		: Op("OpGoto", parameter)
	{ }

	virtual void Execute(MachineState& state);
};

struct OpAttack : Op
{
	OpAttack()
		: Op("OpAttack")
	{ }

	virtual void Execute(MachineState& state);
};

struct OpRangedAttack : Op
{
	OpRangedAttack()
	: Op("OpRangedAttack")
	{ }

	virtual void Execute(MachineState& state);
};

struct OpForward : Op
{
	OpForward()
	: Op("OpForward")
	{ }

	virtual void Execute(MachineState& state);
};


struct OpEndTurn : Op
{
	OpEndTurn()
	: Op("EndTurn")
	{ }

	virtual void Execute(MachineState& state);
};

struct OpTestHuman : Op
{
	OpTestHuman(int parameter)
	: Op("TestHuman", parameter)
	{ }

	virtual void Execute(MachineState& state);
};

struct OpTestWall : Op
{
	OpTestWall()
	: Op("OpTestWall")
	{ }

	virtual void Execute(MachineState& state);
};

struct OpTestZombie : Op
{
	OpTestZombie(int parameter)
	: Op("TestZombie", parameter)
	{ }

	virtual void Execute(MachineState& state);
};

struct OpTestRandom : Op
{
	OpTestRandom()
	: Op("OpTestRandom")
	{ }

	virtual void Execute(MachineState& state);
};

struct OpTestPassable : Op
{
	OpTestPassable()
	: Op("OpTestPassable")
	{ }

	virtual void Execute(MachineState& state);
};


struct OpTestFlagTrueJump : Op
{
	OpTestFlagTrueJump(int parameter)
		: Op("OpTestFlagTrueJump", parameter)
	{ }

	virtual void Execute(MachineState& state);
};

struct OpTestFlagFalseJump : Op
{
	OpTestFlagFalseJump(int parameter)
	: Op("OpTestFlagFalseJump", parameter)
	{ }

	virtual void Execute(MachineState& state);
};

struct OpMem : Op
{
	OpMem(int parameter)
	: Op("OpMem", parameter)
	{ }

	virtual void Execute(MachineState& state);
};

struct OpSet : Op
{
	OpSet(int parameter)
	: Op("OpSet", parameter)
	{ }

	virtual void Execute(MachineState& state);
};

struct OpInc : Op
{
	OpInc()
	: Op("OpInc")
	{ }

	virtual void Execute(MachineState& state);
};

struct OpDec : Op
{
	OpDec()
	: Op("OpDec")
	{ }

	virtual void Execute(MachineState& state);
};

struct OpTestMem : Op
{
	OpTestMem(int parameter)
	: Op("OpTestMem", parameter)
	{ }

	virtual void Execute(MachineState& state);
};

struct OpSaveLoc : Op
{
	OpSaveLoc()
	: Op("OpSaveLoc")
	{ }

	virtual void Execute(MachineState& state);
};



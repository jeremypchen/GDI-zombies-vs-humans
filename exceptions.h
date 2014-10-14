#pragma once
#include <exception>
class InvalidOp : public std::exception
{
	const char* what() const throw() { return "Invalid OP code or parameters."; }
};

class AccessViolation : public std::exception
{
	const char* what() const throw() { return "Machine tried to access outside available memory."; }
};

class InvalidFile : public std::exception{
	const char* what() const throw() { return "File is invalid."; }
};

class MachinesNotLoaded : public std::exception{
	const char* what() const throw() { return "Human and/or zombie machine not yet loaded."; }
};

class NoCurrentMemory : public std::exception{
	const char* what() const throw() { return "No current memory set."; };

};

class ZombieMemory : public std::exception{
	const char* what() const throw() { return "Zombies cannot use memory."; }
};

class InvalidProgramCounter : public std::exception{
	const char* what() const throw() { return "Program counter invalid access."; }
};
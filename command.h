#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <iostream>

const int MAX_ARGS=64;

class Command{
public:
	std::string cmd;
	char* args[MAX_ARGS];
};

#endif
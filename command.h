#ifndef COMMAND_H
#define COMMAND_H
#define MAX_ARGS 64

#include <string>
#include <iostream>

/*enum command_type{
	builtin,
	external
};
*/


class Command{
public:
	//command_type type;
	std::string cmd;
	char* args[MAX_ARGS];
};

#endif
#ifndef BUILTIN_COMMAND
#define BUILTIN_COMMAND

#include <string>
using namespace std;

/***********
 ** Change me. Replace array with hashtable.
 **********/
static string builtin_commands[] = {"cd", "pwd"};

bool isBuiltin(string cmd)
{
	int size = sizeof(builtin_commands) / sizeof(builtin_commands[0]);
	for(int i=0; i<size; ++i)
	{
		if(!cmd.compare(builtin_commands[i]))
		{
			return true;
		}
	}
	return false;
}


#endif

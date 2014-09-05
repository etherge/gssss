#ifndef BUILTIN_COMMAND
#define BUILTIN_COMMAND

#include <map>
#include <string>
#include <iostream>

#include <stdlib.h>
#include <unistd.h>

#include "command.h"

using namespace std;

extern Command current_command;

bool isBuiltin(string cmd);

int cd(char* args[MAX_ARGS]);

int initMap();

int exit_shell(char* args[MAX_ARGS]);

int exec_builtin(Command&);


#endif

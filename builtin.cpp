#include "builtin.h"

typedef int (*builtin_func)(char* [MAX_ARGS]);

static map<string, builtin_func> func_table;

map <string, builtin_func>::iterator table_iter;

int initMap()
{
	func_table["cd"] = cd;
	func_table["exit"] = exit_shell;
	func_table["echo"] = echo;
}

int cd(char* args[MAX_ARGS])
{
	int re = chdir(args[1]);
	if(re !=0 )
	{
		cout << "exec cd error!" << endl;
		return -1;
	}
	return 0;
}

int exit_shell(char* args[MAX_ARGS])
{
	exit(0);
}

int echo(char* args[MAX_ARGS])
{
	cout << args << endl;
	return 0;
}

bool isBuiltin(string cmd)
{
	table_iter = func_table.find(cmd);
	if(table_iter == func_table.end())
		return false;
	else
		return true;
}

//if this func is invoked, then cmd must be builtin func
int exec_builtin(Command& command)
{
	int re = (*(func_table[command.cmd]))(command.args);
	if(re != 0)
		return -1;
	return 0;
}
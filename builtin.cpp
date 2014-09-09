#include "builtin.h"

typedef int (*builtin_func)(char* [MAX_ARGS]);

static string builtin_commands[] = {"cd", "pwd"};
static map<string, builtin_func> func_table;

map <string, builtin_func>::iterator table_iter;

int initMap()
{
	cout << "init map." << endl;
	func_table["cd"] = cd;
	func_table["exit"] = exit_shell;

	

	//for ( table_iter = func_table.begin( ); table_iter != func_table.end( ); table_iter++ )
		//(*(table_iter->second))();
}

int cd(char* args[MAX_ARGS])
{
	cout << "exec cd!" << args[1]<<endl;
	int re = chdir(args[1]);
	cout << "re: " << re << endl;
}

int exit_shell(char* args[MAX_ARGS])
{
	exit(0);
}

int echo(char* args[MAX_ARGS])
{
	cout << args << endl;
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
	(*(func_table[command.cmd]))(command.args);
}
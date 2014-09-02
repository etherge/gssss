#include <string>
#include <unistd.h>
#include <iostream>

#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "builtin.h"
#include "util.h"
#include "command.h"


using namespace std;
int execute_external_command();
int execute_command();
int execute_builtin_command();
int parse_command();

static Command current_command;
static string argstring;
static string command_str;

static string delim = " |>";
static int dup_std_out = -1;

enum token_type{
	entity,
	sgreater,
	sless,
	pipeline
};

string last_entity;
token_type last_token_t;

int parse_command()
{
	int i = 0, start = 0, position, size;
	string substring;
	char c;

	bool in_direct = false,out_direct=false,double_in_direct=false,double_out_direct=false;
	cout << double_out_direct << endl;

	while((position = command_str.find_first_of(delim, start)) != string::npos)
	{
		c = command_str[position];
		substring = command_str.substr(start, position-start);
		start = position + 1;
		//cmddup = command_str.substr(position, cmddup.length()-position);

		switch(c){
			case ' ':	
				if(substring.length() == 0 || out_direct || double_out_direct)//skip space
				{
					continue;
				}			
				size = sizeof(char) * substring.length() + 1;
				current_command.args[i] = (char*)malloc(size);
				memcpy(current_command.args[i], substring.c_str(), size);
				++i;
				//cout << "pos: " << position <<" start: " << start <<" args: " << current_command.args[i-1] << endl;
				last_entity = substring;
				last_token_t = entity;
				break;
			case '|':
				last_token_t = pipeline;
				break;
			case '>':
				if(out_direct) //continuous '>'
				{
					cout << "here1" << endl;
					if(substring.length() == 0)// catch ">>"
					{
						if(double_out_direct)
						{
							cout << "syntax error near >." << endl;
							return -1;
						}
						double_out_direct = 1;
					}else//handle previous '>' , just create file, do not redirect
					{
						cout << "create file one."<<endl;
						dup_std_out = dup(1);
						int fd = open(last_entity.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0666);
						dup2(fd, 1);
						out_direct = 0;
					}
				}else
				{
					cout << "here2"<<endl;
					out_direct = 1;
				}
				last_token_t = sgreater;
				break;
			case '<':
				last_token_t = sless;
				break;
			default:
				break;
		}
		

	}

	for(int j=0; j<i; ++j){
		cout << current_command.args[j] <<"  ";
	}
	cout << "i: " << i <<endl;

	position =command_str.length();
	if(start !=0 && start < position){		
		substring = command_str.substr(start, position-start);
		if(last_token_t == entity)
		{	
			size = sizeof(char) * substring.length() + 1;
			current_command.args[i++] = (char*)malloc(size);
			memcpy(current_command.args[i++], substring.c_str(), size);
			
		}else if(last_token_t == sgreater)
		{
			cout << "create file."<<endl;
			dup_std_out = dup(1);
			int fd = open(substring.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0666);
			dup2(fd, 1);
			out_direct = 0;
		}
	}
	current_command.args[i] = NULL;

	for(int j=0; j<i; ++j){
		cout << current_command.args[j] <<"  ";
	}
	cout << endl;
	current_command.cmd = string(current_command.args[0]);
	//current_command.pArgs = (char* [0])&args[0];
    
	return 0;
}

int execute_command()
{
	if(isBuiltin(current_command.cmd))
	{
		execute_builtin_command();
	}else{
		execute_external_command();
	}
}

int execute_external_command()
{
	pid_t pid;
	int status;
	if((pid = fork()) < 0)
	{
		cout << "fork error!";
	}else if(pid == 0)
	{ // in child
		execvp(current_command.cmd.c_str(), current_command.args);
	}
	// in parent
	if((pid = waitpid(pid, &status, NULL)) < 0)
		cout << "error when waitpid.";

	//restore stdout_fd
	cout << "dup_std_out: " << dup_std_out << endl;
	if(dup_std_out != -1){
		dup2(dup_std_out, 1);
	}
	return 0;
}

int execute_builtin_command()
{
	//execlp(current_command.c_str(), current_command.c_str(), 0);
}

int main(){
	string command;
	while(1){
		cout << "[foo@gsy_shell]$";
		getline(cin, command_str);

		parse_command();

		execute_command();
	}


	return 0;
}


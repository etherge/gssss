#include "shell.h"

using namespace std;

Command command_list[MAX_PIPE];
int command_count = 0;

static string command_str_list[MAX_PIPE];

static string argstring;
static string command_str="";

static string delim = " >";


/**
 * erase space in command_str. including continues space, heading space, tailing space
 * such space can not be erased, like token separator(only one space left).
 * special case: > > ≠ >>, > &1 ≠ >&1
 **/
int erase_space(string& command_str)
{
	string::iterator it;
	bool pre_symbol = true;
	char pre=' ', next=0;
	for(it = command_str.begin(); it != command_str.end(); )
	{
		next = *(it+1);
		//cout << (int)next  << endl; 
		if(pre == '>' || pre == '<')
			pre_symbol = true;
		else
			pre_symbol = false;

		if(*it == ' ' && (pre == ' ' || (pre_symbol && next != '>' && next != '&')))
		{
			it = command_str.erase(it);//pre not change 
		}else
		{
			pre = *it;
			it++;			
		}		
	}
	if(*(--it)==' ')//handle last space
		command_str.erase(it);
	return 0;
}

//no extra space in command_str except token separator
int handle_redirect(string& command_str)
{
	string redirect_symbol = "><";
	int position;
	char symbol;
	string substring(command_str);
	string filename;
	int append = 0;
	int start, length;
	//string new_command_str(command_str);

	while((position=substring.find_first_of(redirect_symbol,0)) != string::npos)
	{
		append = 0;
		symbol = substring[position];
		char nextchar = substring[position+1];
		
		if(position == 0)
			start = 0;
		else
			start = position-1;

		if(nextchar == 0)//syntax error. no redirct target.
		{
			cout << "syntax error!"<<endl;
			return -1;
		}else if(nextchar == '>')//get >>
		{
			append = 1;
			position++;
		}

		int nextspace= substring.find_first_of(' ', position+1);
		if(nextspace == string::npos)//no more symbol
		{
			if(position == substring.length()-1){// no token anymore, it's a syntax error.
				cout << "syntax error.\n";
				return -1;
			}else//get last token as redirect target
			{
				filename = substring.substr(position+1, substring.length()-position-1);
			}
			
		}else //exist space
		{
			filename = substring.substr(position+1, nextspace-position-1);
		}

		if(symbol == '>')
		{
			if(position == 0 || substring[position-1]==' ' ||  substring[position-1]=='>')
				handle_out_redirect_to_file(1, filename, append);
			else
			{
				handle_out_redirect_to_file(substring[position-1]-'0', filename, append);
			}
		}
		if(symbol == '<')
		{
			if(position == 0 || substring[position-1]==' ')
				handle_in_redirect_from_file(0, filename);
			else
			{
				handle_in_redirect_from_file(substring[position-1]-'0', filename);
			}
		}

		cout << filename << "|" <<filename.length() << endl;
		length = filename.length()+2;
		cout << "length: "<<length << endl;
		if(append)
		{
			length++;
			cout << "++lenth: " <<length << endl;
		}
		substring = substring.substr(position + filename.length()+1, substring.length() - position - filename.length());
		command_str = command_str.erase(start, length);
	}
}

//simple function to validate file name. NOT exactly as linux filename rules.
int validate_filename(string filename)
{
	char firstchar = filename[0];
	if(firstchar==0 || firstchar=='>' || firstchar=='<')
		return -1;
	return 0;
}

//validate default STDOUT and STDERR
int validate_std_out(int fd)
{
	if(fd==1 || fd==2)
		return 0;
	return -1;
}

//need to add exception handle
int handle_out_redirect_to_file(int from, string tofile, int append)
{
	if(validate_std_out(from)!=0 || validate_filename(tofile) != 0)
	{
		cout << "syntax error!"<<endl;
		return -1;
	}



	//cout << "out redirect: "<<from <<" " << tofile <<endl;
	int fd;
	if(tofile[0] == '&' && validate_std_out(tofile[1]-'0') == 0)
	{
		fd = tofile[1] = '0';
	}else
	{
		if(!append)
			fd = open(tofile.c_str(), O_CREAT | O_TRUNC | O_RDWR, 0666);
		else
			fd = open(tofile.c_str(), O_CREAT | O_APPEND | O_RDWR , 0666);
	}
	dup2(fd, from);
	close(fd);
	return 0;
}

//need to add exception handle
int handle_in_redirect_from_file(int from, string fromfile)
{
	if(from != 0 || validate_filename(fromfile) != 0)
	{
		cout << "syntax error!"<<endl;
		return -1;
	}
	//cout << "read from file." << endl;
	int fd = open(fromfile.c_str(), O_RDONLY);
	dup2(fd, from);
	close(fd);
	return 0;
}


int execute_command_list()
{
	int infd = dup(0);
	int outfd = dup(1);
	execute_command_with_index(0);
	
	int re1 = dup2(infd, 0);
	int re2 = dup2(outfd, 1);

	close(infd);
	close(outfd);
}

int execute_command_with_index(int command_index)
{
	if(command_index >= command_count)
	{
		return 0;
	}
	parse_single_command(command_str_list[command_index], command_list[command_index]);
	if(command_index == command_count-1)
	{
		//cout << "execute here!"<<endl;
		execute_command(command_list[command_index]);
	}else
	{
		int _fds[2], pid;
		pipe(_fds);
		if((pid = fork()) < 0)
		{
			cout << " fork error!" << endl;
			return -1;
		}else if(pid == 0)// in child
		{
			close(_fds[1]);
			dup2(_fds[0],0);
			close(_fds[0]);
			execute_command_with_index(command_index+1);
		}else// in parent
		{
			close(_fds[0]);
			dup2(_fds[1],1);
			close(_fds[1]);
			execute_command(command_list[command_index]);
			close(1);
			waitpid(pid, 0, 0);
		}
	}
}
 

int execute_command(Command& command)
{
	if(isBuiltin(command.cmd))
	{
		execute_builtin_command(command);
	}else{
		execute_external_command(command);
	}
}

int execute_external_command(Command& command)
{
	pid_t pid;
	int status;
	if((pid = fork()) < 0)
	{
		cout << "fork error!";
	}else if(pid == 0)
	{ // in child
		execvp(command.cmd.c_str(), command.args);
	}
	// in parent
	if((pid = waitpid(pid, &status, 0)) < 0)
		cout << "error when waitpid.";
	return 0;
}

int execute_builtin_command(Command& command)
{
	exec_builtin(command);
}

int parse_to_commands()
{
	char pipe_symbol = '|';
	int position, start=0, i=0;
	string substring(command_str);
	while((position=substring.find_first_of(pipe_symbol,0)) != string::npos)
	{
		command_str_list[i++] = substring.substr(0, position);
		start = position + 1;
		substring = substring.substr(start, substring.length()-start);
	}

	command_str_list[i] = substring;
	command_count = i+1;

	//cout << "Commands: " <<endl;
	//for(int k=0; k<command_count; ++k)
	//{
		//cout  << k << ": " <<command_str_list[k] << endl;
	//}
	//cout <<endl;

	//for(int j=0; j<command_count; ++j)
	//{
		//parse_single_command(command_str_list[j], command_list[j]);
	//}
}


int parse_single_command(string single_command_str, Command& single_command)
{
	//Command single_command;
	erase_space(single_command_str);
	//cout << "after erase: " << single_command_str << endl;

	handle_redirect(single_command_str);
		//return -1;
	//cout << "after handle redirect: " << single_command_str <<" length: " << single_command_str.length()<<endl;

	int i = 0, start = 0, position, size;
	string substring;

	while((position = single_command_str.find_first_of(delim, start)) != string::npos)
	{
		if(position == start) continue;

		substring = single_command_str.substr(start, position-start);
		start = position + 1;

		size = sizeof(char) * substring.length() + 1;
		single_command.args[i] = (char*)malloc(size);
		memcpy(single_command.args[i], substring.c_str(), size);
		++i;
	}

	position =single_command_str.length();
	if(start == 0){//single cmd
		size = sizeof(char) * single_command_str.length() + 1;
		single_command.args[i] = (char*)malloc(size);
		memcpy(single_command.args[i++], single_command_str.c_str(), size);
	}
	if(start !=0 && start < position){		
		substring = single_command_str.substr(start, position-start);	
		size = sizeof(char) * substring.length() + 1;
		single_command.args[i] = (char*)malloc(size);
		memcpy(single_command.args[i++], substring.c_str(), size);			
	}
	single_command.args[i] = NULL;

	for(int j=0; j<i; ++j){
		cout << single_command.args[j] <<"  ";
	}
	cout << endl;
	single_command.cmd = string(single_command.args[0]);

	return 0;
}


int main(){
	initMap();

	string command;
	char endch;
	while(1){
		command_str="";
		cout << "[foo@gsy_shell]$";
		getline(cin, command);

		
		while((endch = command[command.length()-1]) == '\\'){
			command[command.length()-1] = ' ';
			command_str.append(command);
			cout << ">";
			getline(cin, command);
		}
		command_str.append(command);

		parse_to_commands();

		execute_command_list();
	}


	return 0;
}


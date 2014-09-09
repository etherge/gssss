#ifndef SHELL_H
#define SHELL_H

#include <string>
#include <iostream>

#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "builtin.h"
#include "command.h"

const int MAX_PIPE=8;

int execute_external_command(Command&);
int execute_command(Command&);
int execute_builtin_command(Command&);
int parse_single_command(string single_command_str, Command& single_command);
int parse_to_commands();
int execute_command_with_index(int command_index);
int execute_command_list();


int handle_redirect(string&);
int handle_out_redirect_to_file(int from, string tofile, int);
int handle_in_redirect_from_file(int from, string fromfile);
int validate_filename(string);
int validate_std_out(int);

#endif
// Sarah Nicholson
// ----------------------------------------------
// These are the only libraries that can be 
// used. Under no circumstances can additional 
// libraries be included
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include "shell.h"

// --------------------------------------------
// Currently only two builtin commands for this
// assignment exist, however in future, more could 
// be added to the builtin command array.
// --------------------------------------------
const char* valid_builtin_commands[] = {"cd", "exit", NULL};


void parse( char* line, command_t* p_cmd ) {

	// ----------------------------------------
	// TODO: you fully implement this function

	char sp = ' ';	
	int i = 0;	

	while (line[i] == sp)
		i++;
	
	//if the user didn't enter anything 
	if (line[i] == NULL)
	{
		p_cmd -> argv = NULL;
		p_cmd -> path = NULL;
		p_cmd -> argc = 0;
	}
	
	//if the user did enter something 
	else
	{
		int cnt = 0;
		char* token = strtok(line, " ");
		char cmd[sizeof(token)];
		strcpy(cmd, token);
		p_cmd -> argv = malloc(64 * sizeof(char*));
		
		//populate arguments by getting tokens
		while(token != NULL)
		{
			p_cmd -> argv[cnt] = token;
			cnt++;
			token = strtok(NULL, " ");
		}
	
		//last element is supposed to be null
		p_cmd -> argv[cnt + 1] = NULL;

		int b = -1;
		int j;

		//is the command the user entered in valid_builtin_commands[]
		while(j < 2 && b == -1)
		{
			if(strcmp(cmd, valid_builtin_commands[j]) == 0)
			{
				b = 0;
			}
			
			j++;
		}

		//user entered in a valid command
		if(b == 0)
		{
			p_cmd -> path = cmd;
			p_cmd -> argc = cnt;
		}

		//user entered in an invalid command
		else if(find_fullpath(cmd, p_cmd) == 0)
		{
			p_cmd -> argv = NULL;
			p_cmd -> path = cmd;
			p_cmd -> argc = -1;
		}

		else
		{
			p_cmd -> argc = cnt;
			find_fullpath(cmd, p_cmd);
		}
	}

} // end parse function


int execute( command_t* p_cmd ) {

	
	// ----------------------------------------
	// TODO: you fully implement this function

	// -------------------------------------------
	// Added a default return statement
	// however this needs to be changed 
	// to reflect the correct return 
	// value described in the header file
	
	int done;
	
	if (p_cmd -> argc > 0)
	{
		if(fork() == 0)
		{
			//child process
			execv(p_cmd -> path, p_cmd -> argv);
			perror("Execute terminated with an error condition!\n");
			exit(1);
		}
		//parent process which waits for child to terminate
		wait(NULL);
		done = 0;
	}

	if (p_cmd -> argc == -1)
		printf("%s isn't a valid command\n", p_cmd -> path);


	return done;


} // end execute function


int find_fullpath( char* command_name, command_t* p_cmd ) {


	char path_env_variable[300]; 
	
	// ----------------------------------------
	// TODO: you fully implement this function


	// -------------------------------------------
	// Added a default return statement
	// however this needs to be changed 
	// to reflect the correct return 
	// value described in the header file

	strcpy(path_env_variable, getenv("PATH"));
	struct stat buffer;
	char* pathT = strtok(path_env_variable, ":");
	char file_or_dir[70];
	int b = 0;

	while (pathT != NULL && b == 0)
	{
		strcpy(file_or_dir, pathT);
		strcat(file_or_dir, "/");
		strcat(file_or_dir, command_name);
		int doesExist = stat(file_or_dir, &buffer);

		//file or directory exists
		if(doesExist == 0)
		{
			p_cmd -> path = strdup(file_or_dir);
			b = 1;
		}
		
		pathT = strtok(NULL, ":");
	}
	return b;

} // end find_fullpath function


int is_builtin( command_t* p_cmd ) {

	int cnt = 0;

	while ( valid_builtin_commands[cnt] != NULL ) {

		if ( equals( p_cmd->path, valid_builtin_commands[cnt] ) ) {

			return TRUE;

		}

		cnt++;

	}

	return FALSE;

} // end is_builtin function


int do_builtin( command_t* p_cmd ) {

	// only builtin command is cd

	if ( DEBUG ) printf("[builtin] (%s,%d)\n", p_cmd->path, p_cmd->argc);

	struct stat buff;
	int status = ERROR;

	if ( p_cmd->argc == 1 ) {

		// -----------------------
		// cd with no arg
		// -----------------------
		// change working directory to that
		// specified in HOME environmental 
		// variable

		status = chdir( getenv("HOME") );

	} else if ( ( stat( p_cmd->argv[1], &buff ) == 0 && ( S_IFDIR & buff.st_mode ) ) ) {


		// -----------------------
		// cd with one arg 
		// -----------------------
		// only perform this operation if the requested
		// folder exists

		status = chdir( p_cmd->argv[1] );

	} 

	return status;

} // end do_builtin function



void cleanup( command_t* p_cmd ) {

	int i=0;
	
	while ( p_cmd->argv[i] != NULL ) {
		free( p_cmd->argv[i] );
		i++;
	}

	free( p_cmd->argv );
	free( p_cmd->path );	

} // end cleanup function


int equals( char* str1, const char* str2 ) {

	// First check length

	int len[] = {0,0};

	char* b_str1 = str1;
	const char* b_str2 = str2;

	while( (*str1) != '\0' ) { 
		len[0]++;
		str1++;
	}

	while( (*str2) != '\0' ) {
		len[1]++;
		str2++;
	}

	if ( len[0] != len[1] ) {

		return FALSE;

	} else {

		while ( (*b_str1) != '\0' ) {

			// don't care about case (you did not have to perform
			// this operation in your solution

			if ( tolower( (*b_str1)) != tolower((*b_str2)) ) {

				return FALSE;

			}

			b_str1++;
			b_str2++;

		}

	} 

	return TRUE;


} // end compare function definition


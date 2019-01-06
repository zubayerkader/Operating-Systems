// Shell starter file
// You may make any changes to any part of this file.

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#define COMMAND_LENGTH 1024
#define NUM_TOKENS (COMMAND_LENGTH / 2 + 1)
#define HISTORY_DEPTH 10
char history[HISTORY_DEPTH][COMMAND_LENGTH];
int command_count = 0, i = 0;


void insertHistory (char *buff)
{
    
	command_count++;
	char command[COMMAND_LENGTH];
	strcpy (command, buff);
	int buff_length = strlen(command);

	for (int j=0; j<buff_length; j++)
		history[i][j] = command[j];
	history[i][buff_length] = '\0';

	i = (i+1)%10;
}

void displayHistory ()
{
	int start, end_loop;
	if (command_count >= 11)
	{
		end_loop = 10;
    	start = command_count-10+1;

    	int k = i;

    	char start_string[5];

    	for (int j=0; j<end_loop; j++)
    	{
    		sprintf (start_string, "%d", start);
    		write(STDOUT_FILENO, start_string, strlen(start_string));
    		write(STDOUT_FILENO, "\t", strlen("\t"));
    		write(STDOUT_FILENO, history[k], strlen(history[k]));
    		//printf("history[%d]: %s", k, history[k]);
    		write(STDOUT_FILENO, "\n", strlen("\n"));
    		start++;
    		k = (k+1)%10;
    	}
	}
    else 
    {
    	end_loop = i;
    	start = 1;

    	char start_string[5];

    	for (int j=0; j<end_loop; j++)
    	{
    		sprintf (start_string, "%d", start);
    		write(STDOUT_FILENO, start_string, strlen(start_string));
    		write(STDOUT_FILENO, "\t", strlen("\t"));
    		write(STDOUT_FILENO, history[j], strlen(history[j]));
    		write(STDOUT_FILENO, "\n", strlen("\n"));
    		start++;
    		
   		}
    }
}

void handle_SIGINT()
{
	write(STDOUT_FILENO, "\n", strlen("\n"));
	displayHistory();
}

/**
 * Command Input and Processing
 */

/*
 * Tokenize the string in 'buff' into 'tokens'.
 * buff: Character array containing string to tokenize.
 *       Will be modified: all whitespace replaced with '\0'
 * tokens: array of pointers of size at least COMMAND_LENGTH/2 + 1.
 *       Will be modified so tokens[i] points to the i'th token
 *       in the string buff. All returned tokens will be non-empty.
 *       NOTE: pointers in tokens[] will all point into buff!
 *       Ends with a null pointer.
 * returns: number of tokens.
 */
int tokenize_command(char *buff, char *tokens[])
{
	int token_count = 0;
	_Bool in_token = false;
	int num_chars = strnlen(buff, COMMAND_LENGTH);
	for (int i = 0; i < num_chars; i++) {
		switch (buff[i]) {
		// Handle token delimiters (ends):
		case ' ':
		case '\t':
		case '\n':
			buff[i] = '\0';
			in_token = false;
			break;

		// Handle other characters (may be start)
		default:
			if (!in_token) {
				tokens[token_count] = &buff[i];
				token_count++;
				in_token = true;
			}
		}
	}
	tokens[token_count] = NULL;
	return token_count;
}

/**
 * Read a command from the keyboard into the buffer 'buff' and tokenize it
 * such that 'tokens[i]' points into 'buff' to the i'th token in the command.
 * buff: Buffer allocated by the calling code. Must be at least
 *       COMMAND_LENGTH bytes long.
 * tokens[]: Array of character pointers which point into 'buff'. Must be at
 *       least NUM_TOKENS long. Will strip out up to one final '&' token.
 *       tokens will be NULL terminated (a NULL pointer indicates end of tokens).
 * in_background: pointer to a boolean variable. Set to true if user entered
 *       an & as their last token; otherwise set to false.
 */
void read_command(char *buff, char *tokens[], _Bool *in_background)
{
	*in_background = false;

	// Read input

	int length = read(STDIN_FILENO, buff, COMMAND_LENGTH-1);
	if (length == -1)
	{
		buff[0] = '\0';
		return;
	}

	if ( (length < 0) && (errno !=EINTR) )
	{
    	perror("Unable to read command. Terminating.\n");
    	exit(-1);  /* terminate with error */
	}

	// Null terminate and strip \n.
	buff[length] = '\0';
	if (buff[strlen(buff) - 1] == '\n') 
	{
		buff[strlen(buff) - 1] = '\0';
	}
    
    if ( (buff[0] == '!' && (buff[1] >= 'a' && buff[1] <= 'z')) || (buff[0] == '!' && i == 0) )
    {   
        write(STDOUT_FILENO, "./shell: event not found\n", strlen("./shell: event not found.\n"));
    }
    else if (buff[0] == '!' && buff[1] == '!' && i != 0)
    {
        write(STDOUT_FILENO, history[i-1], strlen(history[i-1]));
        strcpy (buff, history[i-1]);
        insertHistory(buff);
    }
    else if (buff[0] == '!' && (buff[1] >= '1' && buff[1] <= '9') && i != 0)
    {
    	int start = 0;
    	if (command_count>10)
    		start = command_count-10+1;

       	int n = atoi (buff+1);
    	
    	if (command_count<=10 && (n>=1 && n<=command_count))
    	{
    		write(STDOUT_FILENO, history[n-1], strlen(history[n-1]));
        	strcpy (buff, history[n-1]);
        	insertHistory(buff);
    	}
    	else if (command_count>10 && (n>=start && n<=command_count))
    	{
    		int x = 10-(command_count-n+1);

    		write(STDOUT_FILENO, history[x], strlen(history[x]));
        	strcpy (buff, history[x]);
        	insertHistory(buff);
    	}
    	else
    		 write(STDOUT_FILENO, "./shell: event not found\n", strlen("./shell: event not found.\n"));
    }
    else if (buff[0] != '\0' )
		insertHistory(buff);

	// Tokenize (saving original command string)
	int token_count = tokenize_command(buff, tokens);
	if (token_count == 0) {
		return;
	}

	// Extract if running in background:
	if (token_count > 0 && strcmp(tokens[token_count - 1], "&") == 0) {
		*in_background = true;
		tokens[token_count - 1] = 0;
	}
}

/**
 * Main and Execute Commands
 */
int main(int argc, char* argv[])
{
	char input_buffer[COMMAND_LENGTH];
	char *tokens[NUM_TOKENS];

	/* set up the signal handler */
	struct sigaction handler;
	handler.sa_handler = handle_SIGINT;
	handler.sa_flags = 0;
	sigemptyset(&handler.sa_mask);
	sigaction(SIGINT, &handler, NULL);

	while (true) 
	{

        char path[COMMAND_LENGTH];
        getcwd(path, COMMAND_LENGTH);
        write(STDOUT_FILENO, path, strlen(path));
        
        
		// Get command
		// Use write because we need to use read() to work with
		// signals, and read() is incompatible with printf().
		write(STDOUT_FILENO, "$ ", strlen("$ "));
		_Bool in_background = false;
		read_command(input_buffer, tokens, &in_background);

		// DEBUG: Dump out arguments:
		if (input_buffer[0] != '\0')
		{
			for (int i = 0; tokens[i] != NULL; i++) 
			{
				write(STDOUT_FILENO, "   Token: ", strlen("   Token: "));
				write(STDOUT_FILENO, tokens[i], strlen(tokens[i]));
				write(STDOUT_FILENO, "\n", strlen("\n"));
			}
		}
              
		if (in_background) 
		{
			write(STDOUT_FILENO, "Run in background.\n", strlen("Run in background.\n"));
		}
        
        if (tokens[0] == NULL || input_buffer[0] == '\0')
            continue;

		if(strcmp(tokens[0], "exit") == 0 || strcmp(tokens[0], "pwd") == 0 || strcmp(tokens[0], "cd") == 0 || strcmp(tokens[0], "type") == 0 || strcmp(tokens[0], "history") == 0)
		{
        	if (strcmp(tokens[0], "exit") == 0)
            	return 0;
        	else if (strcmp(tokens[0], "pwd") == 0)
        	{
            	getcwd(path, COMMAND_LENGTH);
            	write(STDOUT_FILENO, path, strlen(path));
            	write(STDOUT_FILENO, "\n", strlen("\n"));
            	continue;
        	}
        	else if (strcmp(tokens[0], "cd") == 0)
        	{
            	if (chdir(tokens[1]) == -1)
                	write(STDOUT_FILENO, "./shell: cd: h: No such file or directory\n", strlen("./shell: cd: h: No such file or directory\n"));
            	continue;
        	}
        	else if (strcmp(tokens[0], "type") == 0)
        	{
            	if (strcmp(tokens[1], "exit") == 0 || strcmp(tokens[1], "pwd") == 0 || strcmp(tokens[1], "cd") == 0 || strcmp(tokens[1], "type") == 0)
            	{
                	write(STDOUT_FILENO, tokens[1], strlen(tokens[1]));
                	write(STDOUT_FILENO, " is a shell300 builtin\n", strlen(" is a shell300 builtin\n"));
            	}
            	else
            	{
                	write(STDOUT_FILENO, tokens[1], strlen(tokens[1]));
                	write(STDOUT_FILENO, " is external to shell300\n", strlen(" is external to shell300\n"));
            	}
            	continue;
        	}
        	else if (strcmp(tokens[0], "history") == 0)
        	{
            	displayHistory();
            	continue;
        	}
    	}
    	else 
    	{      
			int status = 0;
        	pid_t child_pid = fork();
        	if (child_pid < 0)
        	{
            	write(STDOUT_FILENO, "Forking failed. Exiting... ", strlen("Forking failed. Exiting... "));
        	}
        	else if (child_pid == 0)
        	{
            	status = execvp(tokens[0], tokens);
            	if (status == -1 ) 
            	{
            		write(STDOUT_FILENO, "Command not found.\n", strlen("Command not found.\n"));
            	}
            	exit(0);
        	}
      
        	if (in_background == false)
        	{
            	waitpid(child_pid, &status, 0);
        		//wait(&child_pid);
        	}	
		}

		// Cleanup any previously exited background child processes
        // (The zombies)
       	while (waitpid(-1, NULL, WNOHANG) > 0); 

  
	}
	return 0;
}


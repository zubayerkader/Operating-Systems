// Brian's Shell
// Programming project 2 solution

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define COMMAND_LENGTH 1024
#define NUM_TOKENS (COMMAND_LENGTH / 2 + 1)
#define TOKEN_LENGTH (COMMAND_LENGTH)


/**
 * Write Routines
 * (All safe to use with signals)
 */
#define WRITE_BUFFER_SIZE 1024
char write_buff[WRITE_BUFFER_SIZE];
ssize_t write_string(char *str)
{
	return write(STDOUT_FILENO, str, strlen(str));
}
ssize_t writeInt(int i)
{
	snprintf(write_buff, WRITE_BUFFER_SIZE, "%d", i);
	return write_string(write_buff);
}

void write_current_dir()
{
	char* ret = getcwd(write_buff, WRITE_BUFFER_SIZE);
	if (ret == NULL) {
		write_string("Unable to get current directory.");
	}
	write_string(write_buff);
}
void write_prompt()
{
	write_current_dir();
	write_string("> ");
}



/**
 * History
 */
#define HISTORY_DEPTH 10
static char history[HISTORY_DEPTH][COMMAND_LENGTH];
static int history_next_index = 0;
static int history_num_commands_stored = 0;
void history_store_command(char *new_command)
{
	memcpy(history[history_next_index], new_command, COMMAND_LENGTH);
	history_next_index = (history_next_index + 1) % HISTORY_DEPTH;
	history_num_commands_stored++;
}
void history_write_commands()
{
	int num_to_show = history_num_commands_stored;
	if (history_num_commands_stored > HISTORY_DEPTH) {
		num_to_show = HISTORY_DEPTH;
	}
	for (int i = num_to_show - 1; i >= 0; i--) {
		int index = (HISTORY_DEPTH + history_next_index - 1 - i) % HISTORY_DEPTH;
		int cmd_num = history_num_commands_stored - i;
		writeInt(cmd_num);
		write_string("\t");
		write_string(history[index]);
		write_string("\n");
	}
}
char* history_get_command(int commandNumber)
{
	if (commandNumber < 1 || commandNumber > history_num_commands_stored) {
		return NULL;
	}

	_Bool history_full = history_num_commands_stored > HISTORY_DEPTH;
	_Bool too_old = commandNumber <= history_num_commands_stored - HISTORY_DEPTH;
	if (history_full && too_old) {
		return NULL;
	}

	int index = (commandNumber - 1) % HISTORY_DEPTH;
	return history[index];
}



/**
 * Signal Handler
 */
void handle_SIGINT()
{
	write_string("\n");
	history_write_commands();
	write_prompt();
}
void register_sigint_handler()
{
	struct sigaction handler;
	handler.sa_handler = handle_SIGINT;
	handler.sa_flags = 0;
	sigemptyset(&handler.sa_mask);
	sigaction(SIGINT, &handler, NULL);
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
		// Handle token delimeters (ends):
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
 *       a & as their last token; otherwise set to false.
 */
void read_command(char *buff, char *tokens[], _Bool *in_background)
{
	*in_background = false;

	// Read input
	// Ctrl-c will cause read to fail, but we'll loop to get more input.
	char command[COMMAND_LENGTH];
	int length = -1;
	while (length < 0) {
		length = read(STDIN_FILENO, command, COMMAND_LENGTH-1);

		// If read failure was not caused by ctrl-c, then exit
		if (length < 0 && errno != EINTR) {
			perror("Unable to read command. Terminating.\n");
			exit(-1);
		}
	}

	// Null terminate and strip \n.
	command[length] = '\0';
	if (command[length - 1] == '\n') {
		command[length - 1] = '\0';
	}

	// Tokenize (saving original string)
	memcpy(buff, command, COMMAND_LENGTH);
	int token_count = tokenize_command(buff, tokens);
	if (token_count == 0) {
		return;
	}

	// Handle any history commands:
	if (tokens[0][0] == '!') {
		int hist_number = 0;
		if (tokens[0][1] == '!') {
			hist_number = history_num_commands_stored;
 		} else {
 			hist_number = atoi(&tokens[0][1]);
 		}

		char *command_from_history = history_get_command(hist_number);
		if (command_from_history != NULL) {
			memcpy(buff, command_from_history, COMMAND_LENGTH);
			memcpy(command, command_from_history, COMMAND_LENGTH);
			write_string(buff);
			write_string("\n");
			token_count = tokenize_command(buff, tokens);
		} else {
			write_string("SHELL: Unknown history command.\n");
			tokens[0] = 0;
			token_count = 0;
		}
	}

	// Record command in history
	if (token_count > 0) {
		history_store_command(command);
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
_Bool handle_internal_commands(char *tokens[NUM_TOKENS])
{
	if (strcmp(tokens[0], "history") == 0) {
		history_write_commands();
	}

	else if (strcmp(tokens[0], "exit") == 0) {
		exit(0);
	}

	else if (strcmp(tokens[0], "cd") == 0) {
		int result = chdir(tokens[1]);
		if (result != 0) {
			write_string("Invalid directory.\n");
		}
	}

	else if (strcmp(tokens[0], "pwd") == 0) {
		write_current_dir();
		write_string("\n");
	}
	else {
		return false;
	}
	return true;
}

int main(int argc, char* argv[])
{
	register_sigint_handler();

	char buff[COMMAND_LENGTH];
	char *tokens[NUM_TOKENS];
	while (true) {
		// Cleanup any previously exited processes (zombies)
		while (waitpid(-1, NULL, WNOHANG) > 0) 
			; // do nothing.

		// Get command
		write_prompt();
		_Bool in_background = false;
		read_command(buff, tokens, &in_background);

		// Handle no command
		if (tokens[0] == 0) {
			continue;
		}

		// Do internal commands:
		_Bool was_internal = handle_internal_commands(tokens);
		if (!was_internal) {
			// Make child
			pid_t pid = fork();
			if (pid < 0) {
				perror("Unable to launch child process. Aborting.\n");
				exit(-1);
			}

			// Child: run command
			if (pid == 0) {
				execvp(tokens[0], tokens);

				// Got here? Then exec didn't work
				write_string(tokens[0]);
				write_string(": Unknown command.\n");
				exit(-1);
			}

			// Parent: Wait?
			if (!in_background) {
				pid_t pid_waited = waitpid(pid, NULL, 0);
				if (pid_waited < 0) {
					perror("Error waiting for child.\n");
					exit(-1);
				}
			} 
		}
	}
	return 0;
}

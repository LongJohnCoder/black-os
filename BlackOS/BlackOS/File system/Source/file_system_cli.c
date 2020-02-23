#include "file_system_cli.h"
#include "board_serial.h"
#include "file_system.h"

#include "check.h"

#include <string.h>

#define MAX_ARGUEMTNS 6
#define LENGTH_ARGUMENT 50

static char command_line_argument[MAX_ARGUEMTNS][LENGTH_ARGUMENT];

void cli_input_decode(char* data)
{	
	uint8_t argument_index = 0;
	uint8_t char_index = 0;
	
	while (*data != '\0')
	{
		// Check for overflow
		if ((char_index >= LENGTH_ARGUMENT)  || (argument_index >= MAX_ARGUEMTNS))
		{
			return;
		}
		
		// If we receive a space
		if (*data == ' ')
		{
			if (char_index < LENGTH_ARGUMENT - 1)
			{
				command_line_argument[argument_index][char_index] = '\0';
				argument_index++;
				char_index = 0;
				data++;
			}
			else
			{
				return;
			}
		}
		
		command_line_argument[argument_index][char_index++] = *data;
		
		data++;
	}
	if (char_index < LENGTH_ARGUMENT - 1)
	{
		command_line_argument[argument_index][char_index] = '\0';
		char_index = 0;
	}
	
	if (!strncmp(command_line_argument[0], "ls", 2))
	{
		file_system_command_ls();
	}
	else if (!strncmp(command_line_argument[0], "cd", 2))
	{
		file_system_command_cd(command_line_argument[1]);
	}
	else if (!strncmp(command_line_argument[0], "cat", 3))
	{
		file_system_command_cat(command_line_argument[1]);
	}
	else if (!strncmp(command_line_argument[0], "run", 3))
	{
		file_system_command_run(command_line_argument[1]);
	}
	
	file_system_print_directory();
}
#include "file_system.h"
#include "board_serial.h"

#include "dynamic_memory.h"
#include "kernel.h"

#include <string.h>

#define FILE_SYSTEM_MAX_PATH_LENGTH		1000
#define FILE_SYSTEM_MAX_LIST_LENGTH		30
#define FILE_SYSTEM_BUFFER_SIZE			128


char file_system_path[FILE_SYSTEM_MAX_PATH_LENGTH];
char file_system_tmp_path[FILE_SYSTEM_MAX_PATH_LENGTH];

char file_system_buffer[FILE_SYSTEM_BUFFER_SIZE];

directory_t directory;
file_info_t file_info;

void file_system_config(void)
{
	strcpy(file_system_path, "/");
}

file_result_t file_system_command_ls(void)
{
	file_result_t result;
	
	uint16_t file_count = 0;
	
	result = f_opendir(&directory, file_system_path);
	
	// Return if not successful
	if (result != FR_OK)
	{
		return result;
	}
	board_serial_print("\n");
	
	do
	{
		result = f_readdir(&directory, &file_info);
		
		if (result != FR_OK)
		{
			return result;
		}
		
		board_serial_print("%d Bytes\t%d.%d.%d\t%d:%d\t", file_info.fsize, (file_info.fdate & 0b11111), (file_info.fdate >> 4) & 0b1111, 1980 + ((file_info.fdate >> 9) & 0b1111111), (file_info.ftime >> 11) & 0b11111, (file_info.ftime >> 5) & 0b111111);
		
		if (file_info.fattrib == AM_DIR)
		{
			board_serial_print("<DIR>");
		}
		else
		{
			
		}
		board_serial_print("\t");
		//board_serial_print(file_info.fname);
		
		
		char* tmp = (char *)file_info.fname;
		
		uint8_t filetype = 0;
		while(*tmp != '\0')
		{
			if (*tmp != '.')
			{
				if (!filetype)
				{
					board_serial_write(*tmp);
				}
				else
				{
					board_serial_write((*tmp) - 32);
				}
			}
			else
			{
				board_serial_print("\t");
				filetype = 1;
			}
			tmp++;
		}
		if (filetype)
		{
			board_serial_print(" File");
		}
		board_serial_print("\n");
		
		if (file_count > FILE_SYSTEM_MAX_LIST_LENGTH)
		{
			return FR_OK;
		}
		
		file_count++;
		
		
	} while ((file_info.fname[0]) && (file_count < 100));
	
	board_serial_print("\n");
	
	return FR_OK;
}

file_result_t file_system_command_cd(char* arg)
{
	if (strcmp(arg, ""))
	{
		strcpy(file_system_tmp_path, file_system_path);
		
		// If the argument is a full path
		if (arg[0] == '/')
		{
			if (strlen(arg) + 1 > sizeof(file_system_path))
			{
				return FR_OK;
			}
			else
			{
				strncpy(file_system_tmp_path, arg, sizeof(file_system_tmp_path));
			}
		}
		else if (!strcmp(arg, ".."))
		{
			uint16_t back_counter = strlen(file_system_tmp_path) - 1;
			
			while ((file_system_tmp_path[back_counter] != '/') && (back_counter > 1))
			{
				back_counter--;
			}
			file_system_tmp_path[back_counter] = 0;
		}
		else
		{
			if (strlen(file_system_tmp_path) + strlen(arg) + 2 > sizeof(file_system_path))
			{
				return FR_OK;
			}
			else
			{
				if (strcmp(file_system_tmp_path, "/"))
				{
					strcat(file_system_tmp_path, "/");
				}
				
				strcat(file_system_tmp_path, arg);
			}
		}
		
		file_result_t res = f_opendir(&directory, file_system_tmp_path);
		
		if (res == FR_OK)
		{
			strncpy(file_system_path, file_system_tmp_path, sizeof(file_system_path));
		}

		return FR_OK;
	}
	return FR_OK;
}

file_result_t file_system_command_cat(char* arg)
{
	file_result_t res;
	file_t file;
	uint32_t bytes_read = 0;
	
	if (strlen(file_system_path) + strlen(arg) + 2 > sizeof(file_system_tmp_path))
	{
		
	}

	strcpy(file_system_tmp_path, file_system_path);

	if (strcmp(file_system_tmp_path, "/"))
	{
		strcat(file_system_tmp_path, "/");
	}

	strcat(file_system_tmp_path, arg);

	res = f_open(&file, file_system_tmp_path, FA_READ);
	if (res != FR_OK)
	{
		return res;
	}
	
	do
	{
		res = f_read(&file, file_system_buffer, FILE_SYSTEM_BUFFER_SIZE, &bytes_read);
		if (res != FR_OK)
		{
			return res;
		}
		
		for (uint16_t i = 0; i < bytes_read; i++)
		{
			board_serial_write(file_system_buffer[i]);
		}
		
		
	} while (bytes_read == FILE_SYSTEM_BUFFER_SIZE);
	
	return FR_OK;
}

file_result_t file_system_command_run(char* arg)
{
	file_result_t res;
	file_t file;
	uint32_t bytes_read = 0;
	
	
	if (strlen(file_system_path) + strlen(arg) + 2 > sizeof(file_system_tmp_path))
	{
		
	}

	strcpy(file_system_tmp_path, file_system_path);

	if (strcmp(file_system_tmp_path, "/"))
	{
		strcat(file_system_tmp_path, "/");
	}

	strcat(file_system_tmp_path, arg);

	res = f_open(&file, file_system_tmp_path, FA_READ);
	if (res != FR_OK)
	{
		return res;
	}
	
	// Before we get the data we have to allocate space for it
	uint8_t* application = (uint8_t *)dynamic_memory_new(SRAM, 2000);
	uint8_t* application_iterator = application;
	do
	{
		res = f_read(&file, file_system_buffer, FILE_SYSTEM_BUFFER_SIZE, &bytes_read);
		if (res != FR_OK)
		{
			return res;
		}
		
		for (uint16_t i = 0; i < bytes_read; i++)
		{	
			*application_iterator++ = file_system_buffer[i];
		}
		
		
	} while (bytes_read == FILE_SYSTEM_BUFFER_SIZE);
	
	board_serial_print_address("Application address: ", (uint32_t)application);
	
	uint32_t* iter = (uint32_t *)application;
	
	uint8_t* application_main = application - 1 + *iter++;
	
	application_main = (uint8_t*)((uint32_t)application_main | 0x01);
	
	uint8_t* end_text = application + *iter++;
	uint8_t* start_relocate = application + *iter++;
	uint8_t* end_relocate = application + *iter++;
	uint8_t* start_zero = application + *iter++;
	uint8_t* end_zero = application + *iter++;
	
	
	
	board_serial_print_address("Application main address: ", (uint32_t)application_main);
	board_serial_print_address("End text: ", (uint32_t)end_text);
	board_serial_print_address("Start relocate: ", (uint32_t)start_relocate);
	board_serial_print_address("End relocate: ", (uint32_t)end_relocate);
	board_serial_print_address("Start zero: ", (uint32_t)start_zero);
	board_serial_print_address("End zero: ", (uint32_t)end_zero);
	
	// Here we initialize the relocation segment
	/*uint8_t* src = end_text;
	uint8_t* dest = start_relocate;
	
	if (src != dest)
	{
		while (dest < end_relocate)
		{
			*dest++ = *src++;
		}
	}
	
	// Initialize the zero segment
	for (dest = start_zero; dest < end_zero; dest++)
	{
		*dest = 0;
	}*/

	// Now the data is placed at the right place so no we can start the application
	kernel_add_thread("Print", (thread_function_pointer)(application_main), NULL, THREAD_LEVEL_3, 200);
	
	return FR_OK;
}

void file_system_print_directory(void)
{
	board_serial_print("Cortex:");
	board_serial_print(file_system_path);
	char* tmp = file_system_path;
	while (*tmp != '\0')
	{
		if (*tmp == '\n')
		{
			board_serial_print("ERROOROROROROROROR\n");
		}
		tmp++;
	}
	board_serial_print(" >");
}
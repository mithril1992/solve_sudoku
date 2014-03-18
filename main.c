#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "solver.h"

const size_t BUFFER_SIZE = 512;

int main(int argc, char* argv[])
{
	char *token, buffer[BUFFER_SIZE];
	uint16_t grid[81];
	size_t size;
	if(argc != 2)
	{
		printf("usage: %s <source file name>\n", argv[0]);
		exit(1);
	}

	FILE* source_file = fopen(argv[1], "r");
	if(source_file == NULL)
	{
		perror(argv[1]);
		exit(1);
	}

	size = fread(buffer, sizeof(char), BUFFER_SIZE, source_file);

	token = strtok(buffer, ",\n");
	if(token == NULL)
	{
		fprintf(stderr, "%s: Source File Format is not CSV for \"SUDOKU\".\n", argv[1]);
		exit(1);
	}
	int token_value = atoi(token);
	grid[0] = set_flag(0, token_value);

	for(int i = 1; i < 81; i++)
	{
		token = strtok(NULL, ",\n");
		if(token == NULL)
		{
			fprintf(stderr, "%s: Source File Format is not CSV for \"SUDOKU\".\n", argv[1]);
			exit(1);
		}

		int token_value = atoi(token);
		grid[i] = set_flag(0, token_value);
	}

	printf("problem\n");

	view_grid(grid);

	init_grid(grid);

	solve_grid(grid);

	printf("\nresult:\n");
	view_grid(grid);

	return 0;
}
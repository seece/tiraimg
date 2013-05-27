#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "tiraimg.h"
#include "compress.h"

enum {ACTION_COMPRESS, ACTION_DECOMPRESS} action = 
	ACTION_COMPRESS;

int quality = 90;
bool verbose = false;
char* input_path = NULL;
char* output_path = NULL;


/**
 * @brief Prints program usage instructions.
 */
void print_usage(void) 
{
	printf("Usage: tiraimg [-Vvd] input output\n");
}


/**
 * @brief Prints program version.
 */
void print_version(void) 
{
	printf("tiraimg version %s\n", tiraimg_version_string);
}


/**
 * @brief Parses the program arguments and sets the global configuration
 * variables accordingly.
 *
 * @param argc program argument count
 * @param argv program argument string array
 */
void handle_arguments(int argc, char** argv) 
{
	int c;

	while((c=getopt(argc, argv, "cd")) != -1) {
		switch (c) {
			case 'v':
				verbose = true;
				break;
			case 'V':
				print_version();
				exit(0);
				break;
			case 'd':
				action = ACTION_DECOMPRESS;
				break;
			default:
				printf("unknown char: %c\n", c);
				break;
		}

	}	

	int argument_count = argc-optind;

	if (argument_count < 2) {
		print_usage();
		exit(0);
	}

	input_path = argv[optind];
	output_path = argv[optind + 1];
}

int main(int argc, char** argv) 
{
	handle_arguments(argc, argv);

	printf("input: %s output: %s\n", input_path, output_path);

	if (action == ACTION_COMPRESS) {
		printf("compress\n");
	}

	if (action == ACTION_DECOMPRESS) {
		printf("decompress\n");
	}

	exit(0);
}

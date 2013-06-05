#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>
#include "tiraimg.h"
#include "compress.h"

/**
 * @brief Program processing mode.
 */
enum {ACTION_COMPRESS, ACTION_DECOMPRESS} action = 
	ACTION_COMPRESS;

int quality = 80;
bool verbose = false;
char* input_path = NULL;
char* output_path = NULL;


/**
 * @brief Prints program usage instructions.
 */
void print_usage(void) 
{
	printf("Usage: tiraimg [-Vvd] [-q QUALITY] input output\n");
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

	while((c=getopt(argc, argv, "cdq:")) != -1) {
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
			case 'q':
				quality = atoi(optarg);
				break;
			default:
				printf("unknown char: %c\n", c);
				break;
		}

	}	

	int argument_count = argc-optind;

	if (argument_count < 2) {
		print_usage();
		exit(EXIT_FAILURE);
	}

	input_path = argv[optind];
	output_path = argv[optind + 1];

	if (quality > 100)
		quality = 100;
}

/**
 * @brief Writes a byte array to the disk.
 *
 * @param path target file path
 * @param data pointer to the data to be written
 * @param length length of the data to be written
 *
 * @return how many bytes were actually written to the file
 */
unsigned long write_file(char* path, unsigned char* data, unsigned long length)
{
	unsigned long bytes_written = 0;
	FILE* fp;	
	fp = fopen(path, "w");

	if (fp == NULL)
		return 0;

	bytes_written = fwrite(data, 1, length, fp);

	fclose(fp);

	return bytes_written;
}

unsigned char* read_file(char* path, unsigned long* length) 
{
	FILE* fp = fopen(path, "rb");

	if (fp == NULL) {
		return NULL;
	}

	fseek(fp, 0, SEEK_END);
	*length = ftell(fp);
	rewind(fp);

	unsigned char* data = malloc(*length);
	assert(fread(data, 1, *length, fp) == *length);
	fclose(fp);

	return data;
}

int main(int argc, char** argv) 
{
	handle_arguments(argc, argv);

	printf("input: %s output: %s\n", input_path, output_path);

	if (action == ACTION_COMPRESS) {
		struct Image* imagep = image_load(input_path);
		printf("Picture dimensions: %dx%d\n", imagep->width, imagep->height);
		unsigned long length = 0;
		unsigned long result = 0;
		unsigned char* data = compress_image_full(imagep, quality, &length);

		printf("Saving %lu bytes to %s...", length, output_path);

		result = write_file(output_path, data, length);

		if (result == length) {
			printf("OK!\n");
		} else {
			printf("ERROR: Only wrote %lu bytes!\n", result);
		}

		image_del(imagep);
	}

	if (action == ACTION_DECOMPRESS) {
		printf("decompress\n");
		unsigned long data_len;
		unsigned char* data = read_file(input_path, &data_len);

		if (!data) {
			printf("ERROR: Can't load file %s!\n", input_path);
			exit(EXIT_FAILURE);
		}

		struct Image* imagep = decompress_image_full(data, data_len);

		if (!imagep) {
			printf("ERROR: Image decompression failure.\n");
			exit(EXIT_FAILURE);
		}

		image_save(output_path, imagep);
	}

	exit(EXIT_SUCCESS);
}

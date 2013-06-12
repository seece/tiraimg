#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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

int32_t quality = 60;
uint32_t flags = 0;
bool verbose = false;
char* input_path = NULL;
char* output_path = NULL;

/**
 * @brief Prints program version.
 */
void print_version(void) 
{
	printf("tiraimg version %s\n", tiraimg_version_string);
}

/**
 * @brief Prints program usage instructions.
 */
void print_usage(void) 
{
	printf("Usage: tiraimg [-Vivdy] [-q QUALITY] input output\n");
}


/**
 * @brief Prints detailed program usage instructions.
 */
void print_help(void) 
{
	print_version();
	print_usage();
	printf(
		" -V\tshow version\n"
		" -v\tverbose mode\n"
		" -d\tdecompress, default mode is compression\n"
		" -h\tthis help\n"
		" \nCompression mode options:\n" 
		" -q\tcompression quality, range: [1, 100], default: 60\n"
		" \nDecompression mode (-d) options:\n" 
		" -y\tdo not convert output to RGB colorspace\n"
		" -i\tdo not perform inverse DCT\n"
		" -t\tprint the huffman tree\n"
		);
}

/**
 * @brief Parses the program arguments and sets the global configuration
 * variables accordingly.
 *
 * @param argc program argument count
 * @param argv program argument string array
 */
void handle_arguments(int32_t argc, char** argv) 
{
	int c;

		while((c=getopt(argc, argv, "Vvtcdhyiq:")) != -1) {
		switch (c) {
			case 'v':
				verbose = true;
				global_message_level = TIMG_MSG_VERBOSE;
				break;
			case 't':
				verbose = true;
				global_message_level = TIMG_MSG_HUFFMAN;
				break;
			case 'V':
				print_version();
				exit(0);
				break;
			case 'h':
				print_help();
				exit(0);
				break;
			case 'd':
				action = ACTION_DECOMPRESS;
				break;
			case 'y':
				flags |= COMPRESS_KEEP_YCBCR;
				break;
			case 'i':
				flags |= COMPRESS_NO_IDCT;
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
uint64_t write_file(char* path, uint8_t* data, uint64_t length)
{
	uint64_t bytes_written = 0;
	FILE* fp;	
	fp = fopen(path, "w");

	if (fp == NULL)
		return 0;

	bytes_written = fwrite(data, 1, length, fp);

	fclose(fp);

	return bytes_written;
}


/**
 * @brief Reads a file from the disk to a char array.
 *
 * @param path file path
 * @param length file length output variable
 *
 * @return pointer to the allocated data array
 */
uint8_t* read_file(char* path, uint64_t* length) 
{
	FILE* fp = fopen(path, "rb");

	if (fp == NULL) {
		return NULL;
	}

	fseek(fp, 0, SEEK_END);
	*length = ftell(fp);
	rewind(fp);

	uint8_t* data = malloc(*length);
	fread(data, 1, *length, fp);
	fclose(fp);

	return data;
}

int main(int32_t argc, char** argv) 
{
	handle_arguments(argc, argv);

	printf("input: %s output: %s\n", input_path, output_path);

	if (action == ACTION_COMPRESS) {
		struct Image* imagep = image_load(input_path);
		printf("Picture dimensions: %dx%d\n", imagep->width, imagep->height);
		uint64_t length = 0;
		uint64_t result = 0;
		uint8_t* data = compress_image_full(imagep, quality, (uint64_t*) &length);

		printf("Saving %lu bytes to %s...", length, output_path);

		result = write_file(output_path, data, length);

		if (result == length) {
			printf("OK!\n");

		} else {
			printf("ERROR: Only wrote %lu bytes!\n", result);
		}

		if (length > 0)
			free(data);

		image_del(imagep);
	}

	if (action == ACTION_DECOMPRESS) {
		uint64_t data_len;
		uint8_t* data = read_file(input_path, &data_len);

		if (!data) {
			printf("ERROR: Can't load file %s!\n", input_path);
			exit(EXIT_FAILURE);
		}

		if (verbose) {
			printf("Read %u bytes\n", data_len);
			printf("Decompressing...\n");
		}

		struct Image* imagep = decompress_image_full(data, data_len, flags);

		if (!imagep) {
			printf("ERROR: Image decompression failure.\n");
			exit(EXIT_FAILURE);
		}

		image_save(output_path, imagep);
		image_del(imagep);
		free(data);

		if (verbose)
			printf("Image saved to %s\n", output_path);
	}

	exit(EXIT_SUCCESS);
}

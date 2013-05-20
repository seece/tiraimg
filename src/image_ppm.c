#include <pam.h>
#include <stdbool.h>
#include "image_ppm.h"

/**
 * Initializes libnetpbm image loading library.
 */
bool init_ppm_image_loader(void)
{
	pm_init("tiraimg", 0);

	return false;
}

bool load_ppm_image(
	char * path, 
	int32_t* width, 
	int32_t height, 
	void* data)
{


	return false;
}


#include <stdlib.h>
#include "image.h"

void del_image(struct Image* imagep)
{
	if (imagep->data != NULL) {
		free(imagep->data);	
	}
}

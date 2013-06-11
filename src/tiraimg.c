#include <stdint.h>
#include "tiraimg.h"

enum msg_level global_message_level = TIMG_MSG_NONE;

/** In theory, the image will compress better if we save
 * the color channels in lower quality. This hurts Huffman
 * coding efficiency though. */
const double TIMG_COLOR_QUALITY_MULT = 1.0;

/** Version string used in text output */
const char tiraimg_version_string[] = "0.1";

/** tiraimg file format magic string */
const char tiraimg_magic[] = "CATS";

/** The zero byte used in block zero run length coding */
const uint8_t TIMG_ZERO_BYTE = 0x80;

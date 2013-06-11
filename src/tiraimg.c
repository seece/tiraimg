#include <stdint.h>
#include "tiraimg.h"

enum msg_level global_message_level = TIMG_MSG_NONE;

const char tiraimg_version_string[] = "0.1";
const char tiraimg_magic[] = "CATS";
const uint8_t TIMG_ZERO_BYTE = 0x80;

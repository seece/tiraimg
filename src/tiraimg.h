#ifndef TIRAIMG_H_
#define TIRAIMG_H_
#include <stdint.h>

enum msg_level {TIMG_MSG_NONE, TIMG_MSG_VERBOSE};
extern enum msg_level global_message_level;

extern const double TIMG_COLOR_QUALITY_MULT;
extern const char tiraimg_version_string[];
extern const char tiraimg_magic[];
extern const uint8_t TIMG_ZERO_BYTE;
#endif


#include <stdint.h>

/**
 * @brief Computes an absolute value of an integer.
 *
 * @param i Value whose absolute value will be calculated.
 *
 * @return The absolute value.
 */
int32_t abs(int32_t i)
{      
	return (i < 0 ? -i : i);
}

// psxutil.h

#ifndef _PSXUTIL_H
#define _PSXUTIL_H

/**
 * Given a button bitmask, returns the name for the buttons
 * that the bitmask reports activated.
 * If more than one button is found in the bitmask, it is specified by the symbol &
 * followed by the other button name and so on...
 *
 * @param button Button bitmask as returned by PSX_ReadPad()
 * @param out Pointer to a string, to which the button names will be output
 * @param out_len The maximum length of the string
 * @return The pointer to passed as the "out" parameter
 */

char *PSX_GetButtonName(unsigned short button, char *out, unsigned int out_len);

#endif

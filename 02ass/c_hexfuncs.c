// C implementation of hexdump functions

#include <unistd.h>  // this is the only system header file you may include!
#include "hexfuncs.h"

// TODO: add function implementations here

// Read up to 16 bytes from standard input into data_buf.
// Returns the number of characters read.
unsigned hex_read(char data_buf[]) {
    // TODO
    // 0 is stdin. or do STDIN_FILENO
    unsigned char_read = 0;
    while (read(0, data_buf, 1) == 1 && char_read <= 16) {
        char_read++;
    }
    return char_read;
}

// Write given nul-terminated string to standard output.
void hex_write_string(const char s[]) {
    // TODO
    while (*s != '\0') { //? check end of string
        write(1, s, 1); // write 1 char to stdout
    } 
    
}

// Format an unsigned value as an offset string consisting of exactly 8
// hex digits. The formatted offset is stored in sbuf, which must
// have enough room for a string of length 8.
void hex_format_offset(unsigned offset, char sbuf[]) {
    // TODO: implement
    // cant literate thru offset??? and just put into sbuf
    long remainder;
    long quotient = offset;
    int j = 8;
    while (quotient != 0 && j >= 0) {
        remainder = quotient % 16;
        if (remainder < 10) {
            sbuf[j--] = 48 + remainder;
        } else {
            sbuf[j--] = 55 + remainder;
        }
        quotient = quotient / 16;
    }

}

// Format a byte value (in range 0-255) as string consisting of two hex digits. 
// The string is stored in sbuf.
void hex_format_byte_as_hex(unsigned char byteval, char sbuf[]) {
    // TODO
    int asc = byteval; // ascii number in base what?
    // convert that to a string??
    //sbuf[] = //where
}

// Convert a byte value (in the range 0-255) to a printable character
// value.  If byteval is already a printable character, it is returned
// unmodified.  If byteval is not a printable character, then the
// ASCII code for '.' should be returned.
char hex_to_printable(unsigned char byteval) { // ? base?
    // TODO
    int asc = byteval;
    if (asc > 32 && asc < 127) {
        return byteval;
    } else {
        asc = '.';
        return asc;
    }
}
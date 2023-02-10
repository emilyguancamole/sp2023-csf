// C implementation of hexdump functions

#include <unistd.h>  // this is the only system header file you may include!
#include "hexfuncs.h"

// TODO: add function implementations here

// Read up to 16 bytes from standard input into data_buf.
// Returns the number of characters read.
unsigned hex_read(char data_buf[]) {
    // 0 is stdin
    int char_read = 0;
    char_read = read(0, data_buf, 16);
    
    return char_read;
}

// Write given nul-terminated string to standard output.
void hex_write_string(const char s[]) {
    while (*s != '\0') {
        write(1, s, 1); 
    } 
    
}

// Format an unsigned value as an offset string consisting of exactly 8
// hex digits. The formatted offset is stored in sbuf, which must
// have enough room for a string of length 8.
void hex_format_offset(unsigned offset, char sbuf[]) {
    // initialize 8 spaces
    for (int i = 0; i < 8; i++) {
        sbuf[i] = 0;
    }

    sbuf[8] = '\0';
    int i = 7;
    while (i >= 0) {
        int n = offset & 0b1111; // 4 bits per digit
        
        if (n < 10) {
            n += 48; // numbers
        } else {
            n += 87; // letters, starting from a
        }
        sbuf[i] = n; 
        offset = offset >> 4; // shift by 4 bits to get next digit
        i--;
    }
}

// Format a byte value (in range 0-255) as string consisting of two hex digits. 
// The string is stored in sbuf.
void hex_format_byte_as_hex(unsigned char byteval, char sbuf[]) {
    sbuf[2] = '\0'; // null terminate the string
    int i = 1;
    while (i >= 0) {
        int n = byteval & 0b1111; // 4 bits per digit
        if (n < 10) {
            n += 48; // numbers
        } else {
            n += 87; // letters, starting from a
        }
        sbuf[i] = n; 
        byteval = byteval >> 4; // shift by 4 bits to get next digit
        i--;
    }
    
}

// Convert a byte value (in the range 0-255) to a printable character
// value.  If byteval is already a printable character, it is returned
// unmodified.  If byteval is not a printable character, then the
// ASCII code for '.' should be returned.
char hex_to_printable(unsigned char byteval) { // ? base?
    // TODO
    if (byteval > 32 && byteval < 127) {
        return byteval;
    } else {
        return '.';
    }
}
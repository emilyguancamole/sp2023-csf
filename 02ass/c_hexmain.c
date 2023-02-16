// C implementation of hexdump main function

#include "hexfuncs.h"  // this is the only header file which may be included!

int main(void) {
  // TODO: implement the main function
  char sbuf[17] = {0};
  char offset[9] = {0};   
  char hexbuf[3] = {0};
  int n = hex_read(sbuf);
  unsigned position = 0;

  while (n != 0) {
    hex_format_offset(position, offset);
    hex_write_string(offset); // write format offset
    hex_write_string(": ");

    // print hex pair for each byte
    // looping thru num bytes read and calling formate byte as hex, hex write string(sbuf)
    for (int i = 0; i < n; i++) {
      hex_format_byte_as_hex(sbuf[i], hexbuf);
      hex_write_string(hexbuf);
      hex_write_string(" ");
      
    }

    // fill empty space if not 16 hex digits were passed in
    for (int i = n; i < 16; i++) {
      hex_write_string("   ");
    }

    hex_write_string(" "); // space before string representation
    // print 16 hex to printables
    char printedWords[17] = {0};
    for (int i = 0; i < n; i++) {
      printedWords[i] = hex_to_printable(sbuf[i]);
    }
    
    hex_write_string(printedWords);
    // new line
    hex_write_string("\n");

    n = hex_read(sbuf);
    position += 16;
  }
}

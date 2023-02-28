// Unit tests for hex functions
// These tests should work for both your C implementations and your
// assembly language implementations

#include <stdio.h>
#include <stdlib.h>
#include "tctest.h"
#include "hexfuncs.h"

// test fixture object
typedef struct {
  char test_data_1[16];
  char test_data_2[35];
  char test_data_3[8];
} TestObjs;

// setup function (to create the test fixture)
TestObjs *setup(void) {
  TestObjs *objs = malloc(sizeof(TestObjs));
  strcpy(objs->test_data_1, "Hello, world!\n");
  return objs;
}

// cleanup function (to destroy the test fixture)
void cleanup(TestObjs *objs) {
  free(objs);
}

// Prototypes for test functions
void testFormatOffset(TestObjs *objs);
void testFormatByteAsHex(TestObjs *objs);
void testFormatByteAsHexOutOfRange(TestObjs *objs);
void testHexToPrintable(TestObjs *objs);
void testFormatOffsetHighLow(TestObjs *objs);
void testFormatOffsetTooLongOrShort(TestObjs *objs);
void testHexToPrintableWithinRange(TestObjs *objs);
void testHexToPrintableWeirdChar(TestObjs *objs);
void testHexToPrintableLongArray(TestObjs *objs);

int main(int argc, char **argv) {
  if (argc > 1) {
    tctest_testname_to_execute = argv[1];
  }

  TEST_INIT();

  TEST(testFormatOffset);
  TEST(testFormatByteAsHex);
  TEST(testHexToPrintable);
  
  // OUR UNIT TESTS
  TEST(testFormatByteAsHexOutOfRange);
  TEST(testFormatOffsetHighLow);
  TEST(testFormatOffsetTooLongOrShort);
  TEST(testHexToPrintableWithinRange);
  TEST(testHexToPrintableWeirdChar);
  TEST(testHexToPrintableLongArray);

  TEST_FINI();

  return 0;
}

void testFormatOffset(TestObjs *objs) {
  (void) objs; // suppress warning about unused parameter
  char buf[16];
  hex_format_offset(0x00000001u, buf); 
  ASSERT(0 == strcmp(buf, "00000001"));

  hex_format_offset(0xabcd1234u, buf);
  ASSERT(0 == strcmp(buf, "abcd1234"));
}

void testFormatOffsetHighLow(TestObjs *objs) {
  (void) objs;
  char buf[16];
  hex_format_offset(0x00000000u, buf); 
  ASSERT(0 == strcmp(buf, "00000000")); // test minimum offset

  hex_format_offset(0x99999999u, buf);
  ASSERT(0 == strcmp(buf, "99999999")); // test maximum offset
}

void testFormatOffsetTooLongOrShort(TestObjs *objs) {
  (void) objs;
  char buf[16];
  
  hex_format_offset(0x012345678u, buf); 
  ASSERT(0 == strcmp(buf, "12345678")); // test offset of 9 values

  hex_format_offset(0x0123u, buf); 
  ASSERT(0 == strcmp(buf, "00000123")); // test offset of 4 values
}

void testFormatByteAsHex(TestObjs *objs) { // mix of characters within and out of the range
  char buf[16];
  
  hex_format_byte_as_hex(objs->test_data_1[0], buf);
  ASSERT(0 == strcmp(buf, "48"));

  hex_format_byte_as_hex(objs->test_data_1[6], buf); // test for space
  ASSERT(0 == strcmp(buf, "20"));

  hex_format_byte_as_hex(objs->test_data_1[5], buf); // test for comma (punctuation)
  ASSERT(0 == strcmp(buf, "2c"));
}

void testFormatByteAsHexOutOfRange(TestObjs *objs) {
  char buf[16];
  strcpy(objs->test_data_1, "^..# &!*) }| \n");
  
  hex_format_byte_as_hex(objs->test_data_1[1], buf); // test for period
  ASSERT(0 == strcmp(buf, "2e"));

  hex_format_byte_as_hex(0xC, buf); // tests for values out of bounds (below 32 and 127)
  ASSERT(0 == strcmp(buf, "0c"));

  hex_format_byte_as_hex(0x1A, buf);
  ASSERT(0 == strcmp(buf, "1a"));

  hex_format_byte_as_hex(0x7F, buf);
  ASSERT(0 == strcmp(buf, "7f"));
}

void testHexToPrintable(TestObjs *objs) {
  ASSERT('H' == hex_to_printable(objs->test_data_1[0]));
  ASSERT('.' == hex_to_printable(objs->test_data_1[13]));
}

void testHexToPrintableWithinRange(TestObjs *objs) {
  ASSERT('l' == hex_to_printable(objs->test_data_1[2])); // test for random char within range of printable
  ASSERT(',' == hex_to_printable(objs->test_data_1[5]));
  ASSERT(' ' == hex_to_printable(objs->test_data_1[6]));
  ASSERT('!' == hex_to_printable(objs->test_data_1[12]));
}

void testHexToPrintableWeirdChar(TestObjs *objs) {
  strcpy(objs->test_data_1, "^@$# &!*) }| \n");

  ASSERT('^' == hex_to_printable(objs->test_data_1[0])); // test for punctuations
  ASSERT('|' == hex_to_printable(objs->test_data_1[11]));
  ASSERT(' ' == hex_to_printable(objs->test_data_1[12])); // test for space
  ASSERT('.' == hex_to_printable(objs->test_data_1[13])); // test for new line '\n'

  ASSERT('.' == hex_to_printable(0x0)); // tests for ascii value out of bounds
  ASSERT('.' == hex_to_printable(0xA)); 
  ASSERT('.' == hex_to_printable(0x7F)); 
}

void testHexToPrintableLongArray(TestObjs *objs) {
  strcpy(objs->test_data_2, "We love CSF sooooooo MUCH! LOL ><\n");

  ASSERT('W' == hex_to_printable(objs->test_data_2[0])); // test for array longer than 16
  ASSERT('o' == hex_to_printable(objs->test_data_2[18]));
  ASSERT('U' == hex_to_printable(objs->test_data_2[22]));
  ASSERT('>' == hex_to_printable(objs->test_data_2[31]));
}
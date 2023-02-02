#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uint256.h"

// Create a UInt256 value from a single uint64_t value.
// Only the least-significant 64 bits are initialized directly,
// all other bits are set to 0.
UInt256 uint256_create_from_u64(uint64_t val) {
  UInt256 result;
  result.data[0] = val;
  result.data[1] = 0;
  result.data[2] = 0;
  result.data[3] = 0;
  return result;
}

// Create a UInt256 value from an array of 4 uint64_t values.
// The element at index 0 is the least significant, and the element
// at index 3 is the most significant.
UInt256 uint256_create(const uint64_t data[4]) {
  UInt256 result;
  result.data[0] = data[0];
  result.data[1] = data[1];
  result.data[2] = data[2];
  result.data[3] = data[3];
  return result;
}

// Create a UInt256 value from a string of hexadecimal digits.
UInt256 uint256_create_from_hex(const char *hex) {
  UInt256 result;
  char* end;

  char temp[64]; // 64 characters always
  if (strlen(hex) < 64) {
    size_t zeros = (strlen(hex) > 64) ? 0 : 64 - strlen(hex); // number of zeros to pad if hex is less than 64
    memset(temp, '0', zeros); // place '0' into buffer, 'zeros' times
    strcpy(temp + zeros, hex); // temp + zeros + hex = 64; copy over hex to correct location in temp
  }

  // chunk into 4 parts, put into buffer
  for (int i = 3; i >=0 ; i--) {
    char buffer[17]; // array!!
    strncpy(buffer, temp + i * 16, 16); // copy over temp, starting from right-most chunk
    buffer[16] = '\0'; // add null term
    
    result.data[3-i] = strtoul(buffer, &end, 16); // convert the chunk
    
  }
  
  return result;
}

// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char *uint256_format_as_hex(UInt256 val) {
  int sig = 3; // first significant value
  for (int i = 3; i >= 0; i--) {
    if (val.data[i] == 0UL) {
      sig--;
    } else {
      break;
    }
  }

  char *hex;

  if (sig == -1) {
    hex = malloc(2); // only stores '0' + '\n'
    strcpy(hex, "0"); // setting memory to store the 0 string with strcpy
    return hex;
  }

  
  hex = malloc(16 * sig + 1); // max amount of memory possible
  char *temp = hex; // points along the chunk of memory that hex points to
  
  int x = sprintf(temp, "%lx", val.data[sig]); // number of bytes at most significant place
  temp += x;

  for (int i = sig-1; i >= 0; i--) {
    sprintf(temp, "%016lx", val.data[i]);
    temp += 16;
  }

  return hex;
}

// Get 64 bits of data from a UInt256 value.
// Index 0 is the least significant 64 bits, index 3 is the most
// significant 64 bits.
uint64_t uint256_get_bits(UInt256 val, unsigned index) {
  uint64_t bits;
  bits = val.data[index];
  return bits;
}

// Compute the sum of two UInt256 values.
UInt256 uint256_add(UInt256 left, UInt256 right) {
  UInt256 sum;
  int carry = 0; // use a carry that's either 1 or 0
  for (int i = 0; i < 4; i++) {
    uint64_t leftval = left.data[i];
    uint64_t rightval = right.data[i];
    
    sum.data[i] = leftval + rightval + carry;
    if (leftval + rightval < left.data[i] || leftval + rightval < right.data[i]) { // check overflow - check both left and right
      carry = 1;
    } else if (sum.data[i] < left.data[i] || sum.data[i] < right.data[i]) {
      carry = 1;
    } else  {
      carry = 0;
    }

  }
  return sum;
}

// Compute the difference of two UInt256 values.
UInt256 uint256_sub(UInt256 left, UInt256 right) {
  UInt256 result;

  for (int i = 0; i < 4; i++) {
    // iterate through bits in the column, invert
    right.data[i] = (~right.data[i]);
  }
  right = uint256_add(right, uint256_create_from_u64(1));

  result = uint256_add(left, right);
  return result;
}

// Compute the product of two UInt256 values.
UInt256 uint256_mul(UInt256 left, UInt256 right) {
  UInt256 product; // initialize to 000000

  // TODO: implement
  // int totShifts = 0;
  // for (int i = 0; i < 4; i++) {
  //   uint64_t val = left.data[i]; // current block

    // // figure out total number of shifts
    // for (int j = 0; j < 64; j++) { 
    //   if (uint256_bit_is_set(val, j)) {
    //     totShifts += j;
    //     // add product + uint256_leftshift(right, j);
    //   }
    // }
  // }


  for (int i = 0; i < 256; i++) {
    if (uint256_bit_is_set(left, i)) {
      uint256_leftshift(right, i);
    }
    // call shift function: uint256_leftshift(right, i);
    // add to result
  }

  return product;
}


int uint256_bit_is_set(UInt256 val, unsigned index) {
  int idx = index / 64; // index of val (the block)
  
  // val.data[idx]; // the number in the block
  uint64_t n = index % (64 * idx); // index within the block
  uint64_t mask = 1UL << n; //2^(n+1) - 2^n; // only a 1 at position n

  if (val.data[idx] & mask) {
    return 0; // true, there's a 1 at position n
  }
  return -1; // false
}

UInt256 uint256_leftshift(UInt256 val, unsigned shift) {
  uint64_t mask = 0; // mask to create buffer
  for (int i = 0; i < shift; i++) {
    mask |= (1UL << i); // shift in 'shift' number of 1s
  }
  mask = mask << (64 - shift); // move the 1-bits to the left side

  uint64_t buf1 = val.data[0] & mask; // first chunk overflow
  buf1 = buf1 >> (64 - shift); // shift buf1 all the way right

  val.data[0] = val.data[0] << shift; // shift the actual chunk0

  // shift chunk 2 to make space
  uint64_t buf2 = val.data[1] & mask; 
  buf2 = buf2 >> (64 - shift); // shift buf2 back
  val.data[1] = val.data[1] << shift;

  // insert buf1 to chunk 2
  val.data[1] = val.data[1] | buf1;
  // done with buf1

  buf1 = val.data[2] & mask; // 3rd chunk overflow
  buf1 = buf1 >> (64 - shift); // shift buf1 back
  val.data[2] = val.data[2] << shift;

  // insert buf2 to chunk 3
  val.data[2] = val.data[2] | buf2;

  // insert buf1 into chunk 4
  val.data[3] = val.data[3] | buf1;
  buf2 = val.data[3] & mask; // 4th chunk overflow; discarded
  val.data[3] = val.data[3] << shift;

}
/*
 * Author: Zhu, Yinghui
 * Description: input 2089 bytes to BCH encoder, output 23bytes REDUNDENT, 
 *      The least 4bits of the last REDUNDENT is dummy which is always 0.

 *  BCH spec:
 *      correct 12bits: t = 12.
 *      GF Field is GF(2^15), So:  m = 15.
 *      The Number of REDUNDENT bits is : t * m = 12*15 = 180bits = 22.5 Bytes.
 *      max number of message bits = 2^m-m*t = 32588 bits
 *      Actual number of message bits = 2089*8 = 16712 bits < 32588 bits, 
 *                     in implementation, the (32588-16712)bits are all 0.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 180bit(parity bits) + 4bit(padding bits for byte align, all 0) = 184bits bits
const unsigned char poly[23] = {0xf3, 0xcc, 0xca, 0x1b, 0x16, 0xb1, 
				0xf8, 0xb1, 0xc4, 0xa9, 0x37, 0x16,
				0x85, 0xb3, 0x5d, 0x1f, 0xe, 0x16,
				0x8d, 0x85, 0x92, 0xb1, 0xb0 }; // last 4bits must be 0


void shift_left(unsigned char *msg) { // msg left shift 1bit
  unsigned char cin; // right byte msb, which is feed into current byte when left shift in
  cin = 0;
  for(int i=2088; i>=0; i--) {// 先shift msg[2088], 然后msg[2087], ... 最后msg[0]
    int msb;
    msb = (msg[i] & 0x80) >> 7;
    msg[i] <<=1;
    msg[i] += cin; // the msb of the byte right of current byte shift into lsb
    cin = msb;
  }
}

void poly_xor(unsigned char *msg) { // just xor POLY (180+4)bits
  for(int i=0; i<23; i++) {
    msg[i] ^= poly[i];
  }
}

void bch_enc(const unsigned char *m, // input message bytes
	     unsigned char redundent[23]) { // output 23 Bytes redundent
  unsigned char msb;
  unsigned char msg[2089];
  if(! memcpy(msg, m, 2089) ) {  // keep input message untouched
    printf("error copy from message memory\n");
    exit(1);
  }
  for(int i =0; i<2089*8; i++) {
    msb = (msg[0] & 0x80) >> 7;
    shift_left(msg);
    if(msb)  poly_xor(msg);
  }
  if(! memcpy(redundent, msg, 23) ) {
    printf("error copy to redundent memory\n");
    exit(1);
  }
}


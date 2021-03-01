/*
 * Author: Zhu, Yinghui
 * Description: input 2089 bytes to BCH encoder, output 23bytes REDUNDENT, 
 *      The least 4bits of the last REDUNDENT is dummy which is always 0.
 * This file is used to test BCH encoder, read message file, output redundent 23Bytes.
 *  BCH spec:
 *      correct 12bits: t = 12.
 *      GF Field is GF(2^15), So:  m = 15.
 *      The Number of REDUNDENT bits is : t * m = 12*15 = 180bits = 22.5 Bytes.
 *      max number of message bits = 2^m-m*t = 32588 bits
 *      Actual number of message bits = 2089*8 = 16712 bits < 32588 bits, 
 *                     in implementation, the (32588-16712)bits are all 0.
 */


#include <stdio.h>
#include "bch.h"

void get_msg_from_file(FILE *f, unsigned char *msg) {
  for(int i=2088; i>=0; i--) {
    fscanf(f, "%hhx ", &msg[i]); /* unsigned char input using hex format; 1st line number in 
				    the last enter into encoder */
  }
}

int main() {
  unsigned char msg[2089];
  unsigned char rdt[23];
  char file_name[] = "message.txt";
  FILE *fp = fopen(file_name, "r");
  get_msg_from_file(fp, msg);
  fclose(fp);
  printf("msg[2088]=%x, msg[0]=%x\n", msg[2088], msg[0]);

  bch_enc(msg, rdt);
  
  printf("redundent = \n");
  for(int i=0; i<23; i++) {
    printf("%2x ", rdt[i]);
  }
  printf("\n");
  // for initial version of message.txt, the expected redundent output is :
  //       2b 41 ab 90 98 fd 31 8f 68 bd 3b  8 cc 8e c8 b5 bb e5 d6 d3 53 f2 f0 
}

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
#include <stdlib.h>
#include <string.h>
#include "bch.h"
/*
  m为信息byte,  r为冗余校验byte, 排列如下:
  m[0], m[1], ..., m[2089], r[0], r[1], ..., r[22]
*/
void get_msg_from_file(FILE *f, unsigned char *msg) {
  for(int i=0; i<2089; i++) {
    fscanf(f, "%hhx ", &msg[i]); /* unsigned char input using hex format */
  }
}

void error_inject(unsigned char r[2112]){
  int num, byte_location, bit_location;
  num = rand() % 13; // 0~12 error bits
  printf("\t---- Total %d Error bits injection\n", num);
  for(int i=0; i<num; i++) {
    byte_location = rand() % 2112;
    bit_location = rand() % 8;
    r[byte_location] ^= 1<<bit_location;
    printf("\t\t=>Error Injdect: (%d, %d)\n", byte_location, bit_location);
  }
}

int main() {
  FILE *fp1;
  unsigned char msg[2089];
  unsigned char rdt[23];
  unsigned char rev[2112], dec_dat[2089], errval[2089]; // 2089 + 23
  
  char file_name[] = "message.txt";
  FILE *fp = fopen(file_name, "r");
  get_msg_from_file(fp, msg);
  fclose(fp);

  bch_enc(msg, rdt);
  printf("redundent = \n");
  for(int i=0; i<23; i++) {
    printf("%2x ", rdt[i]);
  }
  printf("\n");
  // for initial version of message.txt, the expected redundent output is :
  //       77 42 dd e7 6e a9 36 a3 5d 11 85 ee c8 28 e4 17 13 57 d3 43 d2 f1 30

  if( !memcpy(rev, msg, 2089) || !memcpy(&rev[2089], rdt, 23) ) {
    printf("Error Copy message and redundent to rev buffer.\n");
  }

  fp1=fopen("rev_dat.dat","w");
  for(int i=0; i<2112; i++)
    fprintf(fp1, "%02x\n", rev[i]);
  fclose(fp1);
  error_inject(rev);

  bch_dec(rev, errval);
  
  for(int i=0; i<2089; i++){
    dec_dat[i] = rev[i] ^ errval[i];
    if(errval[i] != 0)
      printf("error byte:%d;  rev dat:%x;  dec dat:%x\n",i+1,rev[i],dec_dat[i]);
  }

}

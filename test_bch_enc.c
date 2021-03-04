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
#include <time.h>
#include "bch.h"
//#define _DEBUG_VERBOSE_

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
  num = rand() % 14; // 0~12 error bits可纠错, 当num==13的时候不可纠错
  printf("\t---- Total %d Error bits injection\n", num);
  for(int i=0; i<num; i++) {
    byte_location = rand() % 2112;
    bit_location = rand() % 8;
    r[byte_location] ^= 1<<bit_location;
    printf("\t\t=>Error Injdect: (%d, %d)\n", byte_location, bit_location);
  }
}

int main() {
  unsigned char msg[2089];
  unsigned char rdt[23];
  unsigned char rev[2112], dec_dat[2112], errval[2112]; // 2089 + 23
  int num_ebits;
  srand(time(NULL));
  
  char file_name[] = "message.txt";
  FILE *fp = fopen(file_name, "r");
  get_msg_from_file(fp, msg);
  fclose(fp);

  bch_enc(msg, rdt);

#ifdef  _DEBUG_VERBOSE_
  printf("redundent = \n");
  for(int i=0; i<23; i++) {
    printf("%2x ", rdt[i]);
  }
  printf("\n");
#endif

  if( !memcpy(rev, msg, 2089) || !memcpy(&rev[2089], rdt, 23) ) {
    printf("Error Copy message and redundent to rev buffer.\n");
  }

#ifdef  _DEBUG_VERBOSE_
  FILE *fp1;
  fp1=fopen("rev_dat.dat","w");
  for(int i=0; i<2112; i++)
    fprintf(fp1, "%02x\n", rev[i]);
  fclose(fp1);
#endif

  error_inject(rev);

  num_ebits = bch_dec(rev, errval);
  if(num_ebits == -1) {
    printf("\n\t---- Uncorrected Error for BCH, maybe number of error bits is greater than 12\n");
    exit(-1);
  } else {
    printf("\n\t---- Successfully Decoded: totally <<  %d  >> Error bits corrected\n", num_ebits);
  }
  for(int i=0; i<2112; i++){
    dec_dat[i] = rev[i] ^ errval[i];
    if(errval[i] != 0)
      printf("error byte:%d;  rev dat:%x;  dec dat:%x\n",i, rev[i], dec_dat[i]);
  }
  return 0;
}

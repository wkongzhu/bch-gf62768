/*
 * Author: Zhu, Yinghui
 * Description: input 2089 bytes to BCH encoder, output 23bytes REDUNDENT, 
 *      The least 4bits of the last REDUNDENT is dummy which is always 0.

 *  BCH spec:
 *      correct 12bits: t = 12.
 *      GF Field is GF(2^15), So:  m = 15.
 *      The Number of REDUNDENT bits is : t * m = 12*15 = 180bits = 22.5 Bytes. should padding 4bits 0 to byte align
 *      max number of message bits = 2^m-m*t = 32588 bits
 *      Actual number of message bits = 2089*8 = 16712 bits < 32588 bits, 
 *                     in implementation, the (32588-16712)bits are all 0.
 */

#ifndef _RAYMX_BCH_H_
#define _RAYMX_BCH_H_

void bch_enc(const unsigned char *m, // input message bytes
	     unsigned char redundent[23]) ;  // output 23 Bytes redundent


#endif

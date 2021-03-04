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

void bch_enc(const unsigned char m[2089], // input message bytes
	     unsigned char redundent[23]) ;  // output 23 Bytes redundent

int  bch_dec(const unsigned char d[2112], unsigned char errval[2112]);
/*
  bch_dec返回值说明:
  -1   : 无法纠错, 错误bit数量大于12个.
  0    : 没有任何错误bit出现
  1~12 : 有出现1~12个错误bit, 但是可以被纠错, 错误值放在了errval数组里, 
         只需要将输入值和errval对应的byte进行xor, 就可以恢复原始正确数值
	 即:   correted_data[i] = d[i] ^ errval[i];
 */

#endif

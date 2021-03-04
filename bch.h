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
 *
 *  中文说明:
 *  BCH编解码子程序:
 *      输入2089 bytes的message数组, 经过bch_enc()编码,得到23 bytes的校验冗余, 将此23个bytes拼接在2089数组的后面,
 *      构成一个2012 bytes的cordword数组. 此cordword存放到flash里面或者通过channel发送到接收方.
 *      从flash里面读出cordword之后,有可能其中有数据损坏:
 *           (*)  当cordword没有任何错误的时候, bch_decc()返回0.
 *           (*)  当错误bit总数小于等于12 bits的时候, 经过BCH解码电路, bch_dec()返回错误bit的数量,
 *                同时, 在errval数组中记录具体错误的数值, 然后将此值和输入codeword异或, 就可以计算
 *                出原始的cordword值. 
 *           (*)  当错误bits总数大于12 bits的时候,此BCH解码电路无法纠错. bch_dec()函数返回-1.
 */

#ifndef _RAYMX_BCH_H_
#define _RAYMX_BCH_H_

void bch_enc(const unsigned char m[2089], // input message bytes
	     unsigned char redundent[23]) ;  // output 23 Bytes redundent

int  bch_dec(const unsigned char d[2112], unsigned char errval[2112]);
/*
  输入codeword数组 2089+23=2012Bytes
  输出errval值数组, 值为0表示对应的codeword byte没有错误, 值不等于0则表示对应byte的错误值

  bch_dec返回值说明:
  -1   : 无法纠错, 错误bit数量大于12个.
  0    : 没有任何错误bit出现
  1~12 : 有出现1~12个错误bit, 但是可以被纠错, 错误值放在了errval数组里, 
         只需要将输入值和errval对应的byte进行xor, 就可以恢复原始正确数值
	 即:   correted_data[i] = d[i] ^ errval[i];
 */

#endif

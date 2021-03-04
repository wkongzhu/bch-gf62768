/*
 * Author: Zhu, Yinghui
 * Description: input 2089 bytes to BCH encoder, output 23bytes REDUNDENT, 
 *      The least 4bits of the last REDUNDENT is dummy which is always 0.
 * Codeword的格式如下:
          m[0], m[1], ..., m[2088], r[0], r[1], ..., r[22]
	  其中 r[22]的最低4bit一定为0,是多余的padding
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

typedef unsigned char   uchar;
typedef unsigned short   ushort;

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

void bch_enc(const unsigned char m[2089], // input message bytes
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

//////////////////////////////////////////////////////////////// Decoder
/*
 * GF(2^15):  x^15 + x + 1, 表示为0x03, 最高项不用表示
 *
 * 
 */
#include "syndrome.c"

unsigned short gf15mul(unsigned short x, unsigned short y)
{
  unsigned short z;
  unsigned short POLY = 3;
  unsigned short p[15];
  int i,j,k;
  for(i = 0; i < 15; i++){
    if (y & (1<<i) ) {
        p[i] = x;
        for(j=0; j<i; j++) {
          p[i] = (p[i]<<1) ^ (p[i] & (1<<14) ? POLY : 0);
	}
    } else {
      p[i] = 0;
    }
  }
  z = 0;
  for(k=0; k<15; k+=1)
    z = z^ p[k];
  z = z & 0x7FFF; 
  return(z);
}

int ibm(const unsigned short s[24], unsigned short v[13])
{
  //v = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  unsigned short kappa[13] =   {1,0,0,0,0,0,0,0,0,0,0,0,0};
  unsigned short delta = 1;
  unsigned short d = s[0];
  unsigned short v_tmp[13];
  int deg_v = 0;
  unsigned short d_x_kappa[13];
  unsigned short delta_x_v[13];
  unsigned short dtmp[24];
  unsigned short dtmp_n[24];
  unsigned short kappa_tmp[13];
  unsigned short d_tmp;
  for(int i=1; i<13; i++) v[i] = 0;
  v[0] = 1;
  for(int i=0; i<3; i++)
    dtmp[i] = s[2-i];
  for(int i=3; i<24; i++)
    dtmp[i] = s[26-i];

  for(int k=0; k<12; k++){
    for(int i=0; i<13; i++){
      d_x_kappa[i] = gf15mul(d,kappa[i]);
      delta_x_v[i] = gf15mul(delta,v[i]);
    }
    v_tmp[0] = delta_x_v[0];
    for(int i=1; i<13; i++){
      v_tmp[i] = delta_x_v[i]^d_x_kappa[i-1];
    }

    if(d == 0 || (deg_v > k)){
      delta = delta;
      kappa_tmp[0] = 0;
      kappa_tmp[1] = 0;
      for(int i=2; i<13; i=i+1)
        kappa_tmp[i] = kappa[i-2];
    }
    else {
      delta = d;
      kappa_tmp[0] = 0;
      for(int i=1; i<13; i=i+1)
        kappa_tmp[i] = v[i-1];
    }

    for(int i=2; i<24; i=i+1)
      dtmp_n[i] = dtmp[i-2];
    for(int i=0; i<2; i=i+1)
      dtmp_n[i] = dtmp[22+i];

    d_tmp = 0;
    for(int i=0; i<13; i++){
      d_tmp = d_tmp^gf15mul(dtmp[i],v_tmp[i]);
    }

    for(int i=0; i<13; i++){
      kappa[i] = kappa_tmp[i];
      v[i] = v_tmp[i];
    }  
    
    for(int i=0; i<24; i++)
      dtmp[i] = dtmp_n[i];

    d = d_tmp;
    for(int i=0; i<13; i++){
      if(v[i] != 0)
        deg_v = i;
    }
  }
  return deg_v;
}


int ChienSearch(unsigned short v[],int vlen,unsigned char errval[],int elen)
{
  int total_sbits = 0;
  unsigned short A[13],B[13],p[13],vmul[13];
  unsigned short sum;
  unsigned char t=0x01;
  
  unsigned short alpha[13]={0x0,   0x2,   0x4,   0x8,   0x10,  0x20, 0x40, 0x80,
			    0x100, 0x200, 0x400, 0x800, 0x1000};
  unsigned short alpha15876[13]={0x0,    0x75c6, 0x2e72, 0x3228, 0xcfc, 0x2af6, 0x1a58, 0x47d0,
				 0x54b0, 0x2715, 0x4c8c, 0x7088, 0x16d8};
  for(int n=0; n<2089; n++)  errval[n]=0;
  
  for(int i=0; i<elen; i++){
    for(int j=0; j<8;j++){
      sum =v[0];
      for(int k=1; k<vlen; k++){
        A[k] = (i==0 && j==0) ? v[k] : vmul[k];
        B[k] = (i==0 && j==0) ? alpha15876[k] : alpha[k];
        p[k] = gf15mul(A[k],B[k]);
        sum = sum ^ p[k]; 
      }
      if(sum == 0)   {
	total_sbits++;
	errval[i] = errval[i] | (t<<(7-j));
      }
      for(int k=1; k<13; k++)  vmul[k] = p[k];
    }
  }
  return total_sbits;
}

/*
  bch_dec:
  返回值: 0~12表示0~12个bit错误, 可以纠正
  返回值: -1表示不可纠正
 */
int bch_dec(const uchar dat[2112], uchar errval[2112]){
  ushort key[13],  s[24];
  int total_sbits; // search bits
  int deg_v; // error location的次数
  int error_flag = 0;
  syndrome(dat, 2112, s, 24);
  for(int i=0; i<24; i++) {
    if(s[i] != 0) error_flag = 1;
  }
  if(error_flag == 0) return 0;

  deg_v = ibm(s, key);

  total_sbits = ChienSearch(key, 13, errval, 2112);

  if(deg_v == total_sbits)
    return deg_v;
  else
    return -1;
}

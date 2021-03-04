
static ushort shift8(ushort lsfr, ushort g, uchar b)
{
  uchar i;
  ushort r=lsfr;

  for(i=0x80; i!=0; i>>=1)
  {
    if((r&0x4000) != 0){
      r<<=1;
      r=r^g;
    }
    else{
      r<<=1;
    }
    if((b&i) != 0)
      r=r^g;
  }
  r=r&0x7FFF;
  return(r);
}

static short shift5(ushort lsfr,ushort g,uchar b)
{
  uchar i;
  ushort r=lsfr;

  for(i=0x80; i>=0x08; i>>=1)
  {
    if((r&0x4000) != 0){
      r<<=1;
      r=r^g;
    }
    else{
      r<<=1;
    }
    if((b&i) != 0)
      r=r^g;
  }
  r=r&0x7FFF;
  return(r);
}


static ushort lsfr(const uchar din[],int len,ushort poly)
{
  ushort dec_lsfr=0;
  ushort tmp=0;
  ushort tmp0=0;
  ushort tmp1=0;
  ushort tmp_d;
  
  for(int i=0; i<len; i++){
    tmp_d=(ushort)din[i];
    if(i < (len-3))
      dec_lsfr = shift8(dec_lsfr,poly,din[i]);
    else if(i == (len-3)){
      tmp = shift5(dec_lsfr,poly,din[i]);
      dec_lsfr=(tmp&0x0FFF)|((tmp&0x7000)^((tmp_d<<12)&0x7FFF));
    }
    else if(i == (len-2)){
      tmp0 = (dec_lsfr&0x0FF0)^(tmp_d<<4);
    }
    else{
      tmp1 = (dec_lsfr&0x000F)^(tmp_d>>4);
      dec_lsfr =(dec_lsfr&0x7000) | tmp0 | tmp1; 
    }
  }
  return(dec_lsfr);
}

void byte2bit(ushort bits[],int len,ushort t){
  ushort j=0x0001;
  ushort p = t;
  for(int i=0; i<len; i++){
    bits[i] = j&p;
    p = p>>1;
  }
}

ushort bit2byte(ushort a[],int len){
  ushort b = 0;
  ushort tmp = 0x0001;
  for(int i=0; i<len; i++){
    if(a[i])
      b = b | tmp;
    tmp = tmp<<1;
  }
  return(b); 
}

void syndrome(const uchar din[], int len, ushort s[24], int slen) {
  //定义最小多项式
  ushort poly[24]={0x3,   0x3,    0x423,  0x3,    0x100B, 0x423,  0x2AAB, 0x3,
		   0x437, 0x100B, 0x8C7,  0x423,  0xCF,   0x2AAB, 0x7FFD, 0x3,
		   0x729, 0x437,  0x103D, 0x100B, 0x2FC1, 0x8C7,  0x431,  0x423};
  ushort t[24];
  ushort r[15];
  ushort stmp[15];
  for(int i=0; i<slen; i++)    s[i]=0;

  for(int i=0; i<slen; i++)
    t[i] = lsfr(din, len, poly[i]);

  s[0]=t[0];

  byte2bit(r, 15, t[1]);
  stmp[0] = r[0];
  stmp[1] = r[8];
  stmp[2] = r[1]^r[8];
  stmp[3] = r[9];
  stmp[4] = r[2]^r[9];
  stmp[5] = r[10];
  stmp[6] = r[3]^r[10];
  stmp[7] = r[11];
  stmp[8] = r[4]^r[11];
  stmp[9] = r[12];
  stmp[10] = r[5]^r[12];
  stmp[11] = r[13];
  stmp[12] = r[6]^r[13];
  stmp[13] = r[14];
  stmp[14] = r[7]^r[14];
  s[1]=bit2byte(stmp,15);

  byte2bit(r,15,t[2]);
  stmp[0] = r[0]^r[5]^r[10];
  stmp[1] = r[5];
  stmp[2] = r[10];
  stmp[3] = r[1]^r[6]^r[11];
  stmp[4] = r[6];
  stmp[5] = r[11];
  stmp[6] = r[2]^r[7]^r[12];
  stmp[7] = r[7];
  stmp[8] = r[12];
  stmp[9] = r[3]^r[8]^r[13];
  stmp[10] = r[8];
  stmp[11] = r[13];
  stmp[12] = r[4]^r[9]^r[14];
  stmp[13] = r[9];
  stmp[14] = r[14];
  s[2]=bit2byte(stmp,15);

  byte2bit(r,15,t[3]);
  stmp[0] = r[0];
  stmp[1] = r[4]^r[11];
  stmp[2] = r[4]^r[8]^r[11];
  stmp[3] = r[12];
  stmp[4] = r[1]^r[8]^r[12];
  stmp[5] = r[5]^r[12];
  stmp[6] = r[5]^r[9]^r[12];
  stmp[7] = r[13];
  stmp[8] = r[2]^r[9]^r[13];
  stmp[9] = r[6]^r[13];
  stmp[10] = r[6]^r[10]^r[13];
  stmp[11] = r[14];
  stmp[12] = r[3]^r[10]^r[14];
  stmp[13] = r[7]^r[14];
  stmp[14] = r[7]^r[11]^r[14];
  s[3]=bit2byte(stmp,15);

  byte2bit(r,15,t[4]);
  stmp[0] = r[0]^r[3]^r[6]^r[9]^r[12];
  stmp[1] = r[3]^r[9];
  stmp[2] = r[6]^r[9];
  stmp[3] = r[9];
  stmp[4] = r[12];
  stmp[5] = r[1]^r[4]^r[7]^r[10]^r[13];
  stmp[6] = r[4]^r[10];
  stmp[7] = r[7]^r[10];
  stmp[8] = r[10];
  stmp[9] = r[13];
  stmp[10] = r[2]^r[5]^r[8]^r[11]^r[14];
  stmp[11] = r[5]^r[11];
  stmp[12] = r[8]^r[11];
  stmp[13] = r[11];
  stmp[14] = r[14];
  s[4]=bit2byte(stmp,15);

  byte2bit(r,15,t[5]);
  stmp[0] = r[0]^r[5]^r[10];
  stmp[1] = r[12];
  stmp[2] = r[5]^r[12];
  stmp[3] = r[3]^r[8]^r[13];
  stmp[4] = r[3]^r[8]^r[10]^r[13];
  stmp[5] = r[8];
  stmp[6] = r[1]^r[6]^r[8]^r[11];
  stmp[7] = r[13];
  stmp[8] = r[6]^r[13];
  stmp[9] = r[4]^r[9]^r[14];
  stmp[10] = r[4]^r[9]^r[11]^r[14];
  stmp[11] = r[9];
  stmp[12] = r[2]^r[7]^r[9]^r[12];
  stmp[13] = r[14];
  stmp[14] = r[7]^r[14];
  s[5]=bit2byte(stmp,15);

  byte2bit(r,15,t[6]);
  stmp[0] = r[0];
  stmp[1] = r[13];
  stmp[2] = r[11];
  stmp[3] = r[9]^r[11]^r[13];
  stmp[4] = r[7];
  stmp[5] = r[5]^r[7]^r[13];
  stmp[6] = r[3]^r[7]^r[11];
  stmp[7] = r[1]^r[3]^r[5]^r[7]^r[9]^r[11]^r[13];
  stmp[8] = r[14];
  stmp[9] = r[12];
  stmp[10] = r[10]^r[12]^r[14];
  stmp[11] = r[8];
  stmp[12] = r[6]^r[8]^r[14];
  stmp[13] = r[4]^r[8]^r[12];
  stmp[14] = r[2]^r[4]^r[6]^r[8]^r[10]^r[12]^r[14];
  s[6]=bit2byte(stmp,15);

  byte2bit(r,15,t[7]);
  stmp[0] = r[0];
  stmp[1] = r[2]^r[9]^r[13];
  stmp[2] = r[2]^r[4]^r[9]^r[11]^r[13];
  stmp[3] = r[6]^r[13];
  stmp[4] = r[4]^r[6]^r[8]^r[11]^r[13];
  stmp[5] = r[6]^r[10]^r[13];
  stmp[6] = r[6]^r[10]^r[12]^r[13];
  stmp[7] = r[14];
  stmp[8] = r[1]^r[8]^r[12]^r[14];
  stmp[9] = r[3]^r[10]^r[14];
  stmp[10] = r[3]^r[5]^r[10]^r[12]^r[14];
  stmp[11] = r[7]^r[14];
  stmp[12] = r[5]^r[7]^r[9]^r[12]^r[14];
  stmp[13] = r[7]^r[11]^r[14];
  stmp[14] = r[7]^r[11]^r[13]^r[14];
  s[7]=bit2byte(stmp,15);

  byte2bit(r,15,t[8]);
  stmp[0] = r[0]^r[5]^r[10]^r[11]^r[13];
  stmp[1] = r[5]^r[8]^r[11];
  stmp[2] = r[5]^r[8]^r[10];
  stmp[3] = r[2]^r[5]^r[7]^r[12];
  stmp[4] = r[2]^r[10]^r[12];
  stmp[5] = r[12]^r[13];
  stmp[6] = r[4]^r[9]^r[10]^r[12]^r[14];
  stmp[7] = r[7]^r[9]^r[12];
  stmp[8] = r[4]^r[12];
  stmp[9] = r[1]^r[6]^r[11]^r[12];
  stmp[10] = r[6]^r[9]^r[12];
  stmp[11] = r[6]^r[9]^r[11];
  stmp[12] = r[3]^r[6]^r[8]^r[13];
  stmp[13] = r[3]^r[11]^r[13];
  stmp[14] = r[13]^r[14];
  s[8]=bit2byte(stmp,15);

  byte2bit(r,15,t[9]);
  stmp[0] = r[0]^r[3]^r[6]^r[9]^r[12];
  stmp[1] = r[10];
  stmp[2] = r[3]^r[9]^r[10];
  stmp[3] = r[13];
  stmp[4] = r[6]^r[9]^r[13];
  stmp[5] = r[2]^r[5]^r[8]^r[11]^r[14];
  stmp[6] = r[2]^r[5]^r[8]^r[9]^r[11]^r[14];
  stmp[7] = r[5]^r[11];
  stmp[8] = r[5]^r[11]^r[12];
  stmp[9] = r[8]^r[11];
  stmp[10] = r[1]^r[4]^r[7]^r[8]^r[10]^r[11]^r[13];
  stmp[11] = r[11];
  stmp[12] = r[4]^r[10]^r[11];
  stmp[13] = r[14];
  stmp[14] = r[7]^r[10]^r[14];
  s[9]=bit2byte(stmp,15);

  byte2bit(r,15,t[10]);
  stmp[0] = r[0]^r[9];
  stmp[1] = r[4]^r[9]^r[11]^r[13];
  stmp[2] = r[4]^r[7]^r[8];
  stmp[3] = r[3]^r[7]^r[13];
  stmp[4] = r[8]^r[14];
  stmp[5] = r[3]^r[10]^r[12];
  stmp[6] = r[6]^r[7]^r[10]^r[12]^r[14];
  stmp[7] = r[2]^r[7]^r[10];
  stmp[8] = r[2]^r[10]^r[13];
  stmp[9] = r[9]^r[10]^r[11]^r[13];
  stmp[10] = r[5]^r[6]^r[10];
  stmp[11] = r[1]^r[5]^r[9]^r[10];
  stmp[12] = r[5]^r[10]^r[12]^r[14];
  stmp[13] = r[5]^r[8]^r[9];
  stmp[14] = r[4]^r[8]^r[14];
  s[10]=bit2byte(stmp,15);

  byte2bit(r,15,t[11]);
  stmp[0] = r[0]^r[5]^r[10];
  stmp[1] = r[6]^r[13];
  stmp[2] = r[6]^r[12]^r[13];
  stmp[3] = r[4]^r[9]^r[14];
  stmp[4] = r[4]^r[5]^r[9]^r[12]^r[14];
  stmp[5] = r[4]^r[9]^r[11]^r[14];
  stmp[6] = r[3]^r[4]^r[8]^r[9]^r[11]^r[13]^r[14];
  stmp[7] = r[9];
  stmp[8] = r[3]^r[8]^r[9]^r[10]^r[13];
  stmp[9] = r[2]^r[7]^r[9]^r[12];
  stmp[10] = r[2]^r[7]^r[8]^r[9]^r[12];
  stmp[11] = r[14];
  stmp[12] = r[1]^r[6]^r[8]^r[11]^r[14];
  stmp[13] = r[7]^r[14];
  stmp[14] = r[7]^r[13]^r[14];
  s[11]=bit2byte(stmp,15);

  byte2bit(r,15,t[12]);
  stmp[0] = r[0]^r[9]^r[13];
  stmp[1] = r[7]^r[8]^r[11]^r[12]^r[13];
  stmp[2] = r[8]^r[12]^r[14];
  stmp[3] = r[6]^r[7]^r[8]^r[10]^r[11];
  stmp[4] = r[6]^r[8]^r[10]^r[13];
  stmp[5] = r[5]^r[7]^r[8]^r[9]^r[13];
  stmp[6] = r[8]^r[12]^r[13]^r[14];
  stmp[7] = r[4]^r[6]^r[7]^r[13];
  stmp[8] = r[4]^r[6]^r[11]^r[12];
  stmp[9] = r[3]^r[4]^r[5]^r[11];
  stmp[10] = r[4]^r[10]^r[14];
  stmp[11] = r[2]^r[3];
  stmp[12] = r[2]^r[9]^r[13];
  stmp[13] = r[1]^r[9]^r[13];
  stmp[14] = r[8]^r[9]^r[12]^r[13]^r[14];
  s[12]=bit2byte(stmp,15);

  byte2bit(r,15,t[13]);
  stmp[0] = r[0];
  stmp[1] = r[14];
  stmp[2] = r[13]^r[14];
  stmp[3] = r[12];
  stmp[4] = r[11]^r[12];
  stmp[5] = r[10]^r[12]^r[14];
  stmp[6] = r[9]^r[10]^r[11]^r[12]^r[13]^r[14];
  stmp[7] = r[8];
  stmp[8] = r[7]^r[8];
  stmp[9] = r[6]^r[8]^r[14];
  stmp[10] = r[5]^r[6]^r[7]^r[8]^r[13]^r[14];
  stmp[11] = r[4]^r[8]^r[12];
  stmp[12] = r[3]^r[4]^r[7]^r[8]^r[11]^r[12];
  stmp[13] = r[2]^r[4]^r[6]^r[8]^r[10]^r[12]^r[14];
  stmp[14] = r[1]^r[2]^r[3]^r[4]^r[5]^r[6]^r[7]^r[8]^r[9]^r[10]^r[11]^r[12]^r[13]^r[14];
  s[13]=bit2byte(stmp,15);

  byte2bit(r,15,t[14]);
  stmp[0] = r[0]^r[1]^r[2]^r[3]^r[4]^r[5]^r[6]^r[7]^r[8]^r[9]^r[10]^r[11]^r[12]^r[13]^r[14];
  stmp[1] = r[1]^r[3]^r[5]^r[7]^r[9]^r[11]^r[13];
  stmp[2] = r[2]^r[3]^r[6]^r[7]^r[10]^r[11]^r[14];
  stmp[3] = r[3]^r[7]^r[11];
  stmp[4] = r[4]^r[5]^r[6]^r[7]^r[12]^r[13]^r[14];
  stmp[5] = r[5]^r[7]^r[13];
  stmp[6] = r[6]^r[7]^r[14];
  stmp[7] = r[7];
  stmp[8] = r[8]^r[9]^r[10]^r[11]^r[12]^r[13]^r[14];
  stmp[9] = r[9]^r[11]^r[13];
  stmp[10] = r[10]^r[11]^r[14];
  stmp[11] = r[11];
  stmp[12] = r[12]^r[13]^r[14];
  stmp[13] = r[13];
  stmp[14] = r[14];
  s[14]=bit2byte(stmp,15);

  byte2bit(r,15,t[15]);
  stmp[0] = r[0];
  stmp[1] = r[1]^r[8]^r[12]^r[14];
  stmp[2] = r[1]^r[2]^r[8]^r[9]^r[12]^r[13]^r[14];
  stmp[3] = r[3]^r[10]^r[14];
  stmp[4] = r[2]^r[3]^r[4]^r[9]^r[10]^r[11]^r[13]^r[14];
  stmp[5] = r[3]^r[5]^r[10]^r[12]^r[14];
  stmp[6] = r[3]^r[5]^r[6]^r[10]^r[12]^r[13]^r[14];
  stmp[7] = r[7]^r[14];
  stmp[8] = r[4]^r[6]^r[7]^r[8]^r[11]^r[13]^r[14];
  stmp[9] = r[5]^r[7]^r[9]^r[12]^r[14];
  stmp[10] = r[5]^r[6]^r[7]^r[9]^r[10]^r[12]^r[13]^r[14];
  stmp[11] = r[7]^r[11]^r[14];
  stmp[12] = r[6]^r[7]^r[10]^r[11]^r[12]^r[13]^r[14];
  stmp[13] = r[7]^r[11]^r[13]^r[14];
  stmp[14] = r[7]^r[11]^r[13];
  s[15]=bit2byte(stmp,15);

  byte2bit(r,15,t[16]);
  stmp[0] = r[0]^r[5]^r[7]^r[10]^r[11]^r[13]^r[14];
  stmp[1] = r[5]^r[6]^r[8]^r[11]^r[13];
  stmp[2] = r[1]^r[6]^r[8]^r[10]^r[12]^r[13];
  stmp[3] = r[1]^r[6]^r[9]^r[13];
  stmp[4] = r[2]^r[6]^r[11]^r[12]^r[13];
  stmp[5] = r[9]^r[10]^r[11]^r[13];
  stmp[6] = r[2]^r[3]^r[10]^r[12]^r[13];
  stmp[7] = r[3]^r[7]^r[10]^r[11]^r[13];
  stmp[8] = r[3]^r[4]^r[10]^r[12]^r[13];
  stmp[9] = r[3]^r[8]^r[12]^r[13];
  stmp[10] = r[5]^r[8]^r[12]^r[13];
  stmp[11] = r[5]^r[9]^r[11];
  stmp[12] = r[4]^r[6];
  stmp[13] = r[9]^r[10]^r[12]^r[13]^r[14];
  stmp[14] = r[5]^r[6]^r[7]^r[10]^r[12];
  s[16]=bit2byte(stmp,15);

  byte2bit(r,15,t[17]);
  stmp[0] = r[0]^r[5]^r[10]^r[11]^r[13];
  stmp[1] = r[4]^r[12];
  stmp[2] = r[4]^r[5]^r[8]^r[11]^r[12];
  stmp[3] = r[1]^r[6]^r[11]^r[12];
  stmp[4] = r[1]^r[5]^r[6]^r[8]^r[10]^r[11]^r[12];
  stmp[5] = r[6]^r[9]^r[12];
  stmp[6] = r[2]^r[5]^r[6]^r[7]^r[9];
  stmp[7] = r[6]^r[9]^r[11];
  stmp[8] = r[2]^r[6]^r[9]^r[10]^r[11]^r[12];
  stmp[9] = r[3]^r[6]^r[8]^r[13];
  stmp[10] = r[3]^r[6]^r[8]^r[12];
  stmp[11] = r[3]^r[11]^r[13];
  stmp[12] = r[3]^r[4]^r[9]^r[10]^r[11]^r[12]^r[13]^r[14];
  stmp[13] = r[13]^r[14];
  stmp[14] = r[7]^r[9]^r[12]^r[13]^r[14];
  s[17]=bit2byte(stmp,15);

  byte2bit(r,15,t[18]);
  stmp[0] = r[0]^r[3]^r[6]^r[9]^r[11]^r[12];
  stmp[1] = r[3]^r[4]^r[11];
  stmp[2] = r[4]^r[6]^r[8];
  stmp[3] = r[9]^r[10]^r[11]^r[12];
  stmp[4] = r[1]^r[8]^r[10];
  stmp[5] = r[1]^r[4]^r[5]^r[11]^r[12];
  stmp[6] = r[4]^r[7]^r[9]^r[12];
  stmp[7] = r[5]^r[7]^r[9]^r[10]^r[11]^r[12]^r[13];
  stmp[8] = r[2]^r[9]^r[10]^r[12]^r[13];
  stmp[9] = r[5]^r[6]^r[9]^r[11]^r[12]^r[13];
  stmp[10] = r[2]^r[6]^r[8]^r[10]^r[12];
  stmp[11] = r[5]^r[6]^r[11]^r[12]^r[14];
  stmp[12] = r[3]^r[6]^r[8]^r[12];
  stmp[13] = r[3]^r[6]^r[7]^r[11]^r[12];
  stmp[14] = r[3]^r[6]^r[9]^r[10]^r[11]^r[12]^r[14];
  s[18]=bit2byte(stmp,15);

  byte2bit(r,15,t[19]);
  stmp[0] = r[0]^r[3]^r[6]^r[9]^r[12];
  stmp[1] = r[5]^r[11]^r[12];
  stmp[2] = r[5]^r[10]^r[11]^r[12];
  stmp[3] = r[8]^r[11];
  stmp[4] = r[3]^r[8]^r[9]^r[10]^r[11];
  stmp[5] = r[1]^r[4]^r[7]^r[8]^r[10]^r[11]^r[13];
  stmp[6] = r[1]^r[4]^r[7]^r[8]^r[10]^r[11];
  stmp[7] = r[11];
  stmp[8] = r[6]^r[9]^r[11]^r[13];
  stmp[9] = r[4]^r[10]^r[11];
  stmp[10] = r[2]^r[4]^r[5]^r[8]^r[10]^r[14];
  stmp[11] = r[14];
  stmp[12] = r[2]^r[5]^r[8]^r[9]^r[11];
  stmp[13] = r[7]^r[10]^r[14];
  stmp[14] = r[5]^r[7]^r[10]^r[11]^r[14];
  s[19]=bit2byte(stmp,15);

  byte2bit(r,15,t[20]);
  stmp[0] = r[0]^r[5]^r[10]^r[11];
  stmp[1] = r[5];
  stmp[2] = r[5]^r[9]^r[10];
  stmp[3] = r[3]^r[5]^r[8]^r[9]^r[13];
  stmp[4] = r[5]^r[8]^r[10]^r[13];
  stmp[5] = r[5]^r[7]^r[8];
  stmp[6] = r[1]^r[5]^r[6]^r[8]^r[9]^r[10]^r[11]^r[13];
  stmp[7] = r[1]^r[3]^r[5]^r[7]^r[9]^r[13];
  stmp[8] = r[10]^r[11];
  stmp[9] = r[4]^r[9]^r[11]^r[14];
  stmp[10] = r[4]^r[10]^r[11];
  stmp[11] = r[8]^r[11]^r[14];
  stmp[12] = r[2]^r[7]^r[8]^r[10]^r[11]^r[12]^r[14];
  stmp[13] = r[4]^r[7]^r[8]^r[9]^r[11]^r[12];
  stmp[14] = r[2]^r[4]^r[6]^r[8]^r[10]^r[11]^r[12]^r[14];
  s[20]=bit2byte(stmp,15);

  byte2bit(r,15,t[21]);
  stmp[0] = r[0]^r[9];
  stmp[1] = r[2]^r[10]^r[13];
  stmp[2] = r[2]^r[4]^r[9]^r[10]^r[11];
  stmp[3] = r[9]^r[10]^r[11]^r[13];
  stmp[4] = r[4]^r[7]^r[8]^r[9]^r[10]^r[11]^r[13];
  stmp[5] = r[5]^r[6]^r[10];
  stmp[6] = r[3]^r[5]^r[6]^r[7]^r[10]^r[13];
  stmp[7] = r[1]^r[5]^r[9]^r[10];
  stmp[8] = r[1]^r[5]^r[8]^r[9]^r[10]^r[14];
  stmp[9] = r[5]^r[10]^r[12]^r[14];
  stmp[10] = r[3]^r[5]^r[14];
  stmp[11] = r[5]^r[8]^r[9];
  stmp[12] = r[5]^r[6]^r[7]^r[8]^r[9]^r[10]^r[12]^r[14];
  stmp[13] = r[4]^r[8]^r[14];
  stmp[14] = r[2]^r[4]^r[7]^r[8]^r[10]^r[14];
  s[21]=bit2byte(stmp,15);

  byte2bit(r,15,t[22]);
  stmp[0] = r[0]^r[5]^r[10]^r[11];
  stmp[1] = r[2]^r[8]^r[9]^r[11]^r[13];
  stmp[2] = r[2]^r[4]^r[8];
  stmp[3] = r[2]^r[5]^r[6]^r[9]^r[13];
  stmp[4] = r[2]^r[4]^r[6]^r[7]^r[8]^r[13];
  stmp[5] = r[7]^r[9]^r[10]^r[13];
  stmp[6] = r[4]^r[7]^r[12];
  stmp[7] = r[7]^r[9]^r[10]^r[12]^r[14];
  stmp[8] = r[1]^r[4]^r[8]^r[10];
  stmp[9] = r[1]^r[3]^r[9]^r[10]^r[12];
  stmp[10] = r[5]^r[10]^r[12]^r[14];
  stmp[11] = r[5]^r[6]^r[7]^r[9]^r[10]^r[14];
  stmp[12] = r[5]^r[6]^r[8]^r[9]^r[10];
  stmp[13] = r[3]^r[5]^r[7]^r[9]^r[10]^r[11];
  stmp[14] = r[5]^r[10]^r[11]^r[13]^r[14];
  s[22]=bit2byte(stmp,15);

  byte2bit(r, 15, t[23]);
  stmp[0] = r[0]^r[5]^r[10];
  stmp[1] = r[3]^r[8]^r[9]^r[10]^r[13];
  stmp[2] = r[3]^r[6]^r[8]^r[9]^r[10];
  stmp[3] = r[2]^r[7]^r[9]^r[12];
  stmp[4] = r[2]^r[6]^r[7]^r[9]^r[13];
  stmp[5] = r[2]^r[7]^r[8]^r[9]^r[12];
  stmp[6] = r[2]^r[4]^r[7]^r[8]^r[12]^r[14];
  stmp[7] = r[14];
  stmp[8] = r[4]^r[5]^r[9]^r[12];
  stmp[9] = r[1]^r[6]^r[8]^r[11]^r[14];
  stmp[10] = r[1]^r[4]^r[6]^r[8]^r[9];
  stmp[11] = r[7]^r[14];
  stmp[12] = r[3]^r[4]^r[7]^r[8]^r[9]^r[11]^r[13];
  stmp[13] = r[7]^r[13]^r[14];
  stmp[14] = r[7]^r[9]^r[13]^r[14];
  s[23] = bit2byte(stmp, 15);
}


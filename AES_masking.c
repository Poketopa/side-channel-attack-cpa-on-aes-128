#include<assert.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "aes.h"
#include <time.h>

#define MUL2(a) (a<<1)^(a&0x80?0x1b:0)//2와 mixcolumn(==xtime(t)->여기서는 t를 a로 씀)-x * 다항식:차수 1씩 증가 이후 기약다항식으로 감산. 
//8차항이 존재할 경우 8차항을 기약다항식으로 감산한 0x1b와 xor.
#define MUL3(a) MUL2(a)^a//mul3==(x+1)a==xa+a=mul2+a
#define MUL4(a) MUL2((MUL2(a)))//x^2*a=x(xa)=x*mul2=mul2(mul2(a))            매크로->코드복사   :괄호 2개 사용 필수(우선순위 주의)
#define MUL8(a) MUL2((MUL2((MUL2(a)))))//x^3*a=x(x(xa))=mul2((mul2((mul2(a)))))
#define MULB(a) (MUL8(a))^(MUL2(a))^(a)//복 (x^3+x+1)a 
#define MUL9(a) (MUL8(a))^(a)//복호화시 필요(inverse mixcolumn) (x^3+1)a
#define MULD(a) (MUL8(a))^(MUL4(a))^(a)//복   
#define MULE(a) (MUL8(a))^(MUL4(a))^(MUL2(a))//복 (x^3+x^2+x)a

u8 MSbox[256] = { 0, };

u32 u4byte_in(u8* x) {//8비트 네개 32비트 하나로 묶기
   return(x[0] << 24) | (x[1] << 16) | (x[2] << 8) | x[3];//x[0]x[1]x[2]x[3]
}

void u4byte_out(u8* x, u32 y) {//32비트 8비트 네개로 쪼개기
   x[0] = (y >> 24) & 0xff;//32비트에서 8비트만 남기기
   x[1] = (y >> 16) & 0xff;
   x[2] = (y >> 8) & 0xff;
   x[3] = y & 0xff;

}
void AddRoundKey(u8 S[16], u8 RK[16])//라운드키와 이전 라운드의state를 연산
{
   S[0] ^= RK[0];   S[1] ^= RK[1];   S[2] ^= RK[2];   S[3] ^= RK[3];
   S[4] ^= RK[4];   S[5] ^= RK[5];   S[6] ^= RK[6];   S[7] ^= RK[7];
   S[8] ^= RK[8];   S[9] ^= RK[9];   S[10] ^= RK[10]; S[11] ^= RK[11];
   S[12] ^= RK[12]; S[13] ^= RK[13]; S[14] ^= RK[14]; S[15] ^= RK[15];
};
void SubBytes(u8 S[16]) //sbox값 이용해 치환연산
{
   S[0] = Sbox[S[0]]; S[1] = Sbox[S[1]]; S[2] = Sbox[S[2]]; S[3] = Sbox[S[3]];
   S[4] = Sbox[S[4]]; S[5] = Sbox[S[5]]; S[6] = Sbox[S[6]]; S[7] = Sbox[S[7]];
   S[8] = Sbox[S[8]]; S[9] = Sbox[S[9]]; S[10] = Sbox[S[10]]; S[11] = Sbox[S[11]];
   S[12] = Sbox[S[12]]; S[13] = Sbox[S[13]]; S[14] = Sbox[S[14]]; S[15] = Sbox[S[15]];


};
void MSubBytes(u8 S[16]) //sbox값 이용해 치환연산
{
   S[0] = MSbox[S[0]]; S[1] = MSbox[S[1]]; S[2] = MSbox[S[2]]; S[3] = MSbox[S[3]];
   S[4] = MSbox[S[4]]; S[5] = MSbox[S[5]]; S[6] = MSbox[S[6]]; S[7] = MSbox[S[7]];
   S[8] = MSbox[S[8]]; S[9] = MSbox[S[9]]; S[10] = MSbox[S[10]]; S[11] = MSbox[S[11]];
   S[12] = MSbox[S[12]]; S[13] = MSbox[S[13]]; S[14] = MSbox[S[14]]; S[15] = MSbox[S[15]];


};
void ShiftRows(u8 S[16])
{//state의 열 밀기
   u8 temp;
   temp = S[1]; S[1] = S[5]; S[5] = S[9]; S[9] = S[13]; S[13] = temp;
   temp = S[2]; S[2] = S[10]; S[10] = temp; temp = S[6]; S[6] = S[14]; S[14] = temp;
   temp = S[15]; S[15] = S[11]; S[11] = S[7]; S[7] = S[3]; S[3] = temp;

}
void MixColumn(u8 S[16]) //mixcolumn 계산해 인자배열 대입
{
   u8 temp[16];
   int i;
   for (i = 0; i < 16; i += 4) {
      temp[i] = MUL2(S[i]) ^ MUL3(S[i + 1]) ^ S[i + 2] ^ S[i + 3];//mixcolumn에서 s'0,0계산
      temp[i + 1] = S[i] ^ MUL2(S[i + 1]) ^ MUL3(S[i + 2]) ^ S[i + 3];
      temp[i + 2] = S[i] ^ S[i + 1] ^ MUL2(S[i + 2]) ^ MUL3(S[i + 3]);
      temp[i + 3] = MUL3(S[i]) ^ S[i + 1] ^ S[i + 2] ^ MUL2(S[i + 3]);

   }
   S[0] = temp[0];   S[1] = temp[1];   S[2] = temp[2];   S[3] = temp[3];
   S[4] = temp[4];   S[5] = temp[5];   S[6] = temp[6];   S[7] = temp[7];
   S[8] = temp[8];   S[9] = temp[9];   S[10] = temp[10];   S[11] = temp[11];
   S[12] = temp[12];   S[13] = temp[13];   S[14] = temp[14];   S[15] = temp[15];




};
void RoundkeyGeneration128(u8 MK[], u8 RK[]);

void AES_KeySchedule(u8 MK[], u8 RK[], int keysize)
{
   if (keysize == 128)RoundkeyGeneration128(MK, RK);
   //if (keysize == 192)RoundkeyGeneration192(MK, RK);
   //if (keysize == 256)RoundkeyGeneration256(MK, RK);

}

; void RoundkeyGeneration128_masking(u8 MK[], u8 RK[], u8 m[]);


void AES_KeySchedule_masking(u8 MK[], u8 RK[], int keysize,u8 m[])
{
   if (keysize == 128)RoundkeyGeneration128_masking(MK, RK,m);
   //if (keysize == 192)RoundkeyGeneration192(MK, RK);
   //if (keysize == 256)RoundkeyGeneration256(MK, RK);

}

void AES_KeyWordToByte(u32 W[], u8 RK[]) {
   int i;
   for (i = 0; i < 44; i++) {
      u4byte_out(RK + 4 * i, W[i]);//RK[4i]||RK[4i+1]||RK[4i+2]||RK[4i+3]<--W[i]
   }
}

u32 RCons[10] = { 0x01000000,0x02000000,0x04000000,0x08000000,0x10000000,0x20000000,0x40000000,0x80000000,0x1b000000,0x36000000 };
//기약다항식으로 감산함(0x1b0000000부터)
#define RotWord(x) ((x << 8) | (x >> 24))//1 2 3 4 를 2 3 4 0,0 0 0 1로 만들어 두개 or 해서 2341로 만듦


#define SubWord(x) ((u32)Sbox[(u8)(x >> 24)] << 24) |  ((u32)Sbox[(u8)(x >> 16) & 0xff] << 16) | ((u32)Sbox[(u8)(x >> 8) & 0xff] << 8) | (u32)Sbox[(u8)(x & 0xff)]



void RoundkeyGeneration128(u8 MK[], u8 RK[])
{
   u32 W[44];//4byte*11
   int i;
   u32 T;
   W[0] = u4byte_in(MK);//W[0]=MK[0] || MK[1] || MK[2] || MK[3]
   W[1] = u4byte_in(MK + 4);
   W[2] = u4byte_in(MK + 8);
   W[3] = u4byte_in(MK + 12);


   for (i = 0; i < 10; i++)
   {
      //T = G_func(W[4 * i + 3]);->G함수는 처음 워드(w0,w4..)에 대해서만 실행한다. 이하 세 줄은 gfunc
      T = W[4 * i + 3];//w3,7,11..등에 대해서gfunc 계산함 
      T = RotWord(T);//한 번 왼쪽으로 shiftrow
      T = SubWord(T);//4바이트의 sbox 치환
      T ^= RCons[i];//rcons와 xor연산
      //여기까지 gfunc

      W[4 * i + 4] = W[4 * i] ^ T;
      W[4 * i + 5] = W[4 * i + 1] ^ W[4 * i + 4];
      W[4 * i + 6] = W[4 * i + 2] ^ W[4 * i + 5];
      W[4 * i + 7] = W[4 * i + 3] ^ W[4 * i + 6];
   }
   AES_KeyWordToByte(W, RK);

}


void RoundkeyGeneration128_masking(u8 MK[], u8 RK[],u8 m[])
{
   u32 W[44];//4byte*11
   int i;
   u32 T;
   W[0] = u4byte_in(MK);//W[0]=MK[0] || MK[1] || MK[2] || MK[3]
   W[1] = u4byte_in(MK + 4);
   W[2] = u4byte_in(MK + 8);
   W[3] = u4byte_in(MK + 12);


   for (i = 0; i < 10; i++)
   {
      //T = G_func(W[4 * i + 3]);->G함수는 처음 워드(w0,w4..)에 대해서만 실행한다. 이하 세 줄은 gfunc
      T = W[4 * i + 3];//w3,7,11..등에 대해서gfunc 계산함 
      T = RotWord(T);//한 번 왼쪽으로 shiftrow
      T = SubWord(T);//4바이트의 sbox 치환
      T ^= RCons[i];//rcons와 xor연산
      //여기까지 gfunc

      W[4 * i + 4] = W[4 * i] ^ T;
      W[4 * i + 5] = W[4 * i + 1] ^ W[4 * i + 4];
      W[4 * i + 6] = W[4 * i + 2] ^ W[4 * i + 5];
      W[4 * i + 7] = W[4 * i + 3] ^ W[4 * i + 6];
   }
   AES_KeyWordToByte(W, RK);

   for (i = 0; i < 44; i++)//11개의 라운드키
   {
      RK[4 * i] ^= m[0] ^ m[6];//i번째 라운드키의 첫 바이트 마스킹은 m과 m1'으로 마스킹한다
      RK[4 * i + 1] ^= m[0] ^ m[7];//이하 같은 방식으로 다른 값으로 마스킹(addroundkey과정에서의 masking)
      RK[4 * i + 2] ^= m[0] ^ m[8];
      RK[4 * i + 3] ^= m[0] ^ m[9];

   }

}


void AES_ENC(u8 PT[16], u8 RK[], u8 CT[16], int keysize) {
   int Nr = keysize / 32 + 6;//keysize입력으로 라운드 수 계산.128bit 키의 경우 10라운드
   int i;
   u8 temp[16];//작업공간
   for (i = 0; i < 16; i++)temp[i] = PT[i];//대칭키는 연산이 빠르므로 크기가 크지 않는 한 for문보다 풀어쓰는것이 효율이 뛰어남

   AddRoundKey(temp, RK);//temp의 16바이트와 RK의 첫 16바이트를 xor하여 temp에 결과를 담는 함수
   for (i = 0; i < Nr - 1; i++)
   {
      SubBytes(temp);
      ShiftRows(temp);
      MixColumn(temp);
      AddRoundKey(temp, RK + 16 * (i + 1));

   }
   SubBytes(temp);
   ShiftRows(temp);
   AddRoundKey(temp, RK + 16 * (i + 1));

   for (i = 0; i < 16; i++)CT[i] = temp[i];//결과 암호문에 대입(풀어쓰는게 효율좋음)


}



void AES_ENC_masking(u8 PT[16], u8 RK[], u8 CT[16], int keysize,u8 m[10]) {
   int Nr = keysize / 32 + 6;//keysize입력으로 라운드 수 계산.128bit 키의 경우 10라운드
   int i,j;
   u8 temp[16];//작업공간
   for (i = 0; i < 16; i++)temp[i] = PT[i];//대칭키는 연산이 빠르므로 크기가 크지 않는 한 for문보다 풀어쓰는것이 효율이 뛰어남

   for(i=0;i<16;i+=4)//첫 라운드에서 평문을 X^3으로 마스킹해야 0라운드 keyaddition 시 1라운드 입력이 x로 마스킹 된 채 입력됨
    {
      temp[i] ^= m[6];
      temp[i+1] ^= m[7];
      temp[i+2] ^= m[8];
      temp[i+3] ^= m[9];

   }

   AddRoundKey(temp, RK);//temp의 16바이트와 RK의 첫 16바이트를 xor하여 temp에 결과를 담는 함수
   for (i = 0; i < Nr - 1; i++)
   {
      MSubBytes(temp);
      ShiftRows(temp);
      for (j = 0; j < 16; j += 4)//shiftrow 이후 마스킹 단계
      {
         temp[j] ^= m[2]^m[1];
         temp[j+1] ^= m[3] ^ m[1];
         temp[j+2] ^= m[4] ^ m[1];
         temp[j+3] ^= m[5] ^ m[1];

      }
      MixColumn(temp);
      AddRoundKey(temp, RK + 16 * (i + 1));

   }
   MSubBytes(temp);
   ShiftRows(temp);
   AddRoundKey(temp, RK + 16 * (i + 1));

   for (j = 0; j < 16; j += 4)//마지막에 마스킹 한 후 암호문에서 마스킹 제거=>C xor X xor X^3 xor X^1 형태로 마스킹되어있음 
   {
      temp[j] ^= m[0] ^ m[1]^m[6];
      temp[j + 1] ^= m[0] ^ m[1]^m[7];
      temp[j + 2] ^= m[0] ^ m[1]^m[8];
      temp[j + 3] ^= m[0] ^ m[1]^m[9];

   }


   for (i = 0; i < 16; i++)CT[i] = temp[i];//결과 암호문에 대입(풀어쓰는게 효율좋음)


}


int main(int argc, char* argv[]) {
   
   u8 PT[16] = { 0xDF,0x6C ,0x5D ,0x18 ,0xF9 ,0x6E ,0x72 ,0xE3 ,0xBF,0x57 ,0xCE ,0x57 ,0x6C ,0xD9 ,0x9D ,0x6D
   };
   u8 MK[16] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0 };
   //u8 MK[16] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };    
   u8 CT[16] = { 0x00, };
   u32 W[60] = { 0x00, };
   u8 RK[240] = { 0x00, };//15라운드까지 쓸 경우에는 240바이트 필요하므로(16*15).11라운드에서는 176까지만(16*11)
   u8 temp;
   int keysize = 128;
   u8 m[10];//m,m',m1,m2,m3,m4,m1',m2',m3',m4'


   AES_KeySchedule(MK, RK, keysize);//1r:RK 0-15,2r:RK 16-31
   AES_ENC(PT, RK, CT, keysize);
   int i;
   printf("AES:\n");
   for (i = 0; i < 16; i++)printf("%02x ", CT[i]);
   printf("\n\n");

   
   //난수생성:m,m',m1,m2,m3,m4
   //m1',m2',m3',m4'는 m1~m4의 mixcoumn
   srand(time(NULL));
   m[0] = rand();//m
   m[1] = rand();//m'
   m[2] = rand();//m1
   m[3] = rand();//m2
   m[4] = rand();//m3
   m[5] = rand();//m4

   m[6]= MUL2(m[2]) ^ MUL3(m[3]) ^ m[4] ^ m[5];
   m[7]= m[2] ^ MUL2(m[3]) ^ MUL3(m[4]) ^ m[5];
   m[8]=m[2] ^ m[3] ^ MUL2(m[4]) ^ MUL3(m[5]);
   m[9]= MUL3(m[2]) ^ m[3] ^ m[4] ^ MUL2(m[5]);


   AES_KeySchedule_masking(MK, RK, keysize,m);//1r:RK 0-15,2r:RK 16-31

   for (i = 0; i < 256; i++)//msbox 생성
   {
      MSbox[(u8)i ^ m[0]] = Sbox[(u8)i] ^ m[1];//위와 같은 식이다: MSbox(x xor m)==Sbox(x) xor m'
   }
   AES_ENC_masking(PT, RK, CT, keysize,m);

   printf("AES:\n");
   for (i = 0; i < 16; i++)printf("%02x ", CT[i]);
   printf("\n\n");
   printf("%d", MSbox[1]);
}
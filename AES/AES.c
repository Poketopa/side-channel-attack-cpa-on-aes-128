#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "aes.h"

// 2배를 곱함
// a를 leftShift한 뒤 첫번째 비트가 1이면 0x1b과 xor연산
// 0x1b과 xor연산은 Galois 필드에서 다항식을 사용하여 나머지를 구하는 역할을 함
// 0x80 = 1000 0000 이므로 앞자리가 1인지 확인
#define MUL2(a) (a<<1)^(a&0x80?0x1b:0)
// 3배를 곱함 (2배 곱에 a 더하기) / Galois필드 GF(2^8)에서 덧셈은 xor이다
#define MUL3(a) (MUL2(a))^(a)
#define MUL4(a) MUL2((MUL2(a)))
#define MUL8(a) MUL2((MUL2((MUL2(a)))))
#define MUL9(a) (MUL8(a))^(a)
#define MULB(a) (MUL8(a))^(MUL2(a))^(a)
#define MULD(a) (MUL8(a))^(MUL4(a))^(a)
#define MULE(a) (MUL8(a))^(MUL4(a))^(MUL2(a))

u8 MUL(u8 a, u8 b){
    u8 r =0;    
    u8 tmp = b;
    u32 i;
    // Galois 필드에서 덧셈은 xor, 곱셈은 쉬프트 + xor
    // a의 비트수 만큼 연산한다 1이면 xor연산 0이면 넘어간다
    for (i = 0;i<8;i++){
        if(a&1)r^=tmp;
        tmp = MUL2(tmp);
        // 다음 비트를 확인하기 위함
        a>>=1; //(a = a>>1)
    }
    return r;
}

// 역원이란 x와 곱했을 때 1이 되는 값
// x * inv(x) = 1 인 값을 찾는 함수다
// 갈루아 필드에서 x^254는 역원이다
u8 inv(u8 a){
    u8 r = a;
    r = MUL(r, r); //a2
    r = MUL(r, a); //a3
    r = MUL(r, r); //a6
    r = MUL(r, a); //a7
    r = MUL(r, r); //a14
    r = MUL(r, a); //a15
    r = MUL(r, r); //a30
    r = MUL(r, a); //a31
    r = MUL(r, r); //a62
    r = MUL(r, a); //a63
    r = MUL(r, r); //a126
    r = MUL(r, a); //a127
    r = MUL(r, r); //a254
    return r;
}

u8 GenSbox(u8 a){
    u8 r = 0;
    u8 tmp;
    tmp = inv(a);
    // 아래는 특정 비트가 1인지 확인하는 코드이다
    // (1은 첫번째, 2는 2번째 등)
    if (tmp & 1)r ^= 0x1f;
    if (tmp & 2)r ^= 0x3e;
    if (tmp & 4)r ^= 0x7c;
    if (tmp & 8)r ^= 0xf8;
    if (tmp & 16)r ^= 0xf1;
    if (tmp & 32)r ^= 0xe3;
    if (tmp & 64)r ^= 0xc7;
    if (tmp & 128)r ^= 0x8f;
    return r^0x63;
}

void AddRoundKey(u8 S[16], u8 RK[16]){
    S[0] ^= RK[0]; S[1] ^= RK[1]; S[2] ^= RK[2]; S[3] ^= RK[3]; 
    S[4] ^= RK[4]; S[5] ^= RK[5]; S[6] ^= RK[6]; S[7] ^= RK[7]; 
    S[8] ^= RK[8]; S[9] ^= RK[9]; S[10] ^= RK[10]; S[11] ^= RK[11]; 
    S[12] ^= RK[12]; S[13] ^= RK[13]; S[14] ^= RK[14]; S[15] ^= RK[15]; 
}

void SubBytes(u8 S[16]){
    S[0]= Sbox[S[0]]; S[1] = Sbox[S[1]]; S[2] = Sbox[S[2]]; S[3] = Sbox[S[3]]; 
    S[4]= Sbox[S[4]]; S[5] = Sbox[S[5]]; S[6] = Sbox[S[6]]; S[7] = Sbox[S[7]]; 
    S[8]= Sbox[S[8]]; S[9] = Sbox[S[9]]; S[10] = Sbox[S[10]]; S[11] = Sbox[S[11]]; 
    S[12]= Sbox[S[12]]; S[13] = Sbox[S[13]]; S[14] = Sbox[S[14]]; S[15] = Sbox[S[15]]; 
}

void ShiftRows(u8 S[16]){
    u8 temp;
    temp = S[1]; S[1] = S[5]; S[5] = S[9]; S[9] = S[13]; S[13] = temp;

    temp = S[2]; S[2] = S[10]; S[10] = temp; 
    temp = S[6]; S[6] = S[14]; S[14] = temp;
    
    temp = S[15]; S[15] = S[11]; S[11] = S[7]; S[7] = S[3]; S[3] = temp;
}

void MixColumns(u8 S[16]){
    int i;
    u8 temp[16];

    for (i=0;i<16;i+=4){
        temp[i] = MUL2(S[i]) ^ MUL3(S[i+1]) ^ S[i + 2] ^ S[i + 3];
        temp[i+1] = S[i] ^ MUL2(S[i + 1]) ^ MUL3(S[i + 2]) ^ S[i + 3];
        temp[i+2] = S[i] ^ S[i + 1] ^ MUL2(S[i + 2]) ^ MUL3(S[i + 3]);
        temp[i+3] = MUL3(S[i]) ^ S[i + 1] ^ S[i + 2] ^ MUL2(S[i + 3]);
    }

    S[0] = temp[0]; S[1] = temp[1]; S[2] = temp[2]; S[3] = temp[3];
    S[4] = temp[4]; S[5] = temp[5]; S[6] = temp[6]; S[7] = temp[7];
    S[8] = temp[8]; S[9] = temp[9]; S[10] = temp[10]; S[11] = temp[11];
    S[12] = temp[12]; S[13] = temp[13]; S[14] = temp[14]; S[15] = temp[15];
}

void AES_ENC(u8 PT[16], u8 RK[], u8 CT[16], int keysize){
    int Nr = keysize / 32 + 6;
    int i;
    u8 temp[16];

    // for문 사용하는 게 그냥 쓰는 것보다 느리다
    // 아래처럼 쓰는 것보다 16줄을 지정해서 쓰는게 더 빠름
    for (i =0;i<16;i++) temp[i] = PT[i];

    AddRoundKey(temp, RK);

    for (i=0;i< Nr-1;i++){
        SubBytes(temp);
        ShiftRows(temp);
        MixColumns(temp);
        AddRoundKey(temp,RK+16*(i+1));
    }

    SubBytes(temp);
    ShiftRows(temp);
    AddRoundKey(temp,RK+16*(i+1));

    for ( i = 0;i<16;i++) CT[i] = temp[i];
}

// 마스터키의 배열을 옮긴다
// 0x01000000
// 0x00230000
// 0x00004500
// 0x00000067  모두 or연산 하면
// => 0x01234567
u32 u4byte_in(u8 *x){
    return (x[0]<<24) | (x[1]<<16)| (x[2] <<8)| x[3];
}

void u4byte_out(u8*x, u32 y){
    x[0] = (y>>24) & 0xff;
    x[1] = (y>>16) & 0xff;
    x[2] = (y>>8) & 0xff;
    x[3] = y & 0xff;
}

void AES_KeyWordToByte(u32 W[], u8 RK[]){
    int i;
    for (i=0;i<44;i++){
        u4byte_out(RK+4*i,W[i]);
    }
}

u32 Rcons[10] ={0x01000000,0x02000000,0x04000000,0x08000000,0x10000000,0x20000000,0x40000000,0x80000000,0x1b000000,0x36000000};

// 왼쪽으로 1바이트 쉬프트
// 0x12345678 예시
// 0x34567800 | 0x00000012 == 0x34567812
#define RotWord(x) ((x<<8) | (x>>24))

// 1바이트씩 쪼개서 Sbox에 넣는다
#define SubWord(x) \
  ((u32)Sbox[(u8)(x>>24)]<<24) \
| ((u32)Sbox[(u8)((x>>16)&0xff)] <<16) \
| ((u32)Sbox[(u8)((x>>8)&0xff)] <<8) \
| ((u32)Sbox[(u8)(x&0xff)]) \

void RoundkeyGeneration128(u8 MK[], u8 RK[]){
    u32 W[44];
    int i;
    u32 T;

    // 마스터키 배열을 4개씩 묶어 4바이트 배열로 전환한다
    W[0] = u4byte_in(MK);
    W[1] = u4byte_in(MK+4);
    W[2] = u4byte_in(MK+8);
    W[3] = u4byte_in(MK+12);

    for(i=0;i<10;i++){
        //T = G_func(W[4*i+3]);
        T = W[4*i+3];
        T = RotWord(T);
        T = SubWord(T);
        T ^= Rcons[i];

        W[4*i+4] = W[4*i] ^T;
        W[4*i+5] = W[4*i+1] ^ W[4*i+4];
        W[4*i+6] = W[4*i+2] ^ W[4*i+5];
        W[4*i+7] = W[4*i+3] ^ W[4*i+6];
    }
    AES_KeyWordToByte(W,RK);
}

void AES_KeySchedule(u8 MK[], u8 RK[], int keysize){
    if (keysize == 128) RoundkeyGeneration128(MK,RK);
    //if (keysize == 192) RoundkeyGeneration192(MK,RK);
    //if (keysize == 256) RoundkeyGeneration256(MK,RK);

}

int main(){
    int i;
    u8 PT[16] = { 0xDF, 0x6C, 0x5D, 0x18, 0xF9, 0x6E, 0x72, 0xE3, 0xBF, 0x57, 0xCE, 0x57, 0x6C, 0xD9, 0x9D, 0x6D };
    u8 MK[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
    u8 CT[16] = {0X00,};
    u8 RK[240] = {0X00, };
    int keysize = 128;
    
    AES_KeySchedule(MK, RK , keysize);
    AES_ENC(PT,RK,CT,keysize);

    for (i =0;i<16;i++){
        printf("%02x ", CT[i]);
    }

    return 0;
}


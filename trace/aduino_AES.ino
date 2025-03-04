#define PIN_TRIGG 5

uint8_t buffer_rx[256];
uint8_t buffer_index = 0;
uint8_t RK[176] = {0,};

uint8_t Sbox[256] = {
  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

uint8_t strToHex(char ch){
  uint8_t result;
  switch(ch){
    case '0' : result = 0; break;
    case '1' : result = 1; break;
    case '2' : result = 2; break;
    case '3' : result = 3; break;
    case '4' : result = 4; break;
    case '5' : result = 5; break;
    case '6' : result = 6; break;
    case '7' : result = 7; break;
    case '8' : result = 8; break;
    case '9' : result = 9; break;
    case 'a' : result = 10; break;
    case 'b' : result = 11; break;
    case 'c' : result = 12; break;
    case 'd' : result = 13; break;
    case 'e' : result = 14; break;
    case 'f' : result = 15; break;
    case 'A' : result = 10; break;
    case 'B' : result = 11; break;
    case 'C' : result = 12; break;
    case 'D' : result = 13; break;
    case 'E' : result = 14; break;
    case 'F' : result = 15; break;
  }
  return result;
}

void RotWord(uint8_t T[4]){
  uint8_t tmp;
  tmp = T[0];
  T[0] = T[1];
  T[1] = T[2];
  T[2] = T[3];
  T[3] = tmp;
}

void SubWord(uint8_t T[4]){
  T[0] = Sbox[T[0]];
  T[1] = Sbox[T[1]];
  T[2] = Sbox[T[2]];
  T[3] = Sbox[T[3]];
}

uint8_t Rcons[10] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};

void RoundKeyGeneration128(uint8_t MK[16],uint8_t RK[176]){
  int i;
  uint8_t T[4];

  for(i=0;i<16;i++){
    RK[i] = MK[i];
  }

  for(i=0;i<10;i++){
    T[0] = RK[16*i+12];
    T[1] = RK[16*i+13];
    T[2] = RK[16*i+14];
    T[3] = RK[16*i+15];

    RotWord(T);
    SubWord(T);
    T[0] ^= Rcons[i];

    RK[16*i+16] = RK[16*i] ^ T[0];
    RK[16*i+17] = RK[16*i+1] ^ T[1];
    RK[16*i+18] = RK[16*i+2] ^ T[2];
    RK[16*i+19] = RK[16*i+3] ^ T[3];
    RK[16*i+20] = RK[16*i+4] ^ RK[16*i+16];
    RK[16*i+21] = RK[16*i+5] ^ RK[16*i+17];
    RK[16*i+22] = RK[16*i+6] ^ RK[16*i+18];
    RK[16*i+23] = RK[16*i+7] ^ RK[16*i+19];
    RK[16*i+24] = RK[16*i+8] ^ RK[16*i+20];
    RK[16*i+25] = RK[16*i+9] ^ RK[16*i+21];
    RK[16*i+26] = RK[16*i+10] ^ RK[16*i+22];
    RK[16*i+27] = RK[16*i+11] ^ RK[16*i+23];
    RK[16*i+28] = RK[16*i+12] ^ RK[16*i+24];
    RK[16*i+29] = RK[16*i+13] ^ RK[16*i+25];
    RK[16*i+30] = RK[16*i+14] ^ RK[16*i+26];
    RK[16*i+31] = RK[16*i+15] ^ RK[16*i+27];
  }
}

void AddRoundKey(uint8_t s[16], uint8_t rk[16]) {
    s[0] ^= rk[0]; s[1] ^= rk[1]; s[2] ^= rk[2]; s[3] ^= rk[3];
    s[4] ^= rk[4]; s[5] ^= rk[5]; s[6] ^= rk[6]; s[7] ^= rk[7];
    s[8] ^= rk[8]; s[9] ^= rk[9]; s[10] ^= rk[10]; s[11] ^= rk[11];
    s[12] ^= rk[12]; s[13] ^= rk[13]; s[14] ^= rk[14]; s[15] ^= rk[15];
}

void SubBytes(uint8_t s[16]) {
    s[0] = Sbox[s[0]]; s[1] = Sbox[s[1]]; s[2] = Sbox[s[2]]; s[3] = Sbox[s[3]];
    s[4] = Sbox[s[4]]; s[5] = Sbox[s[5]]; s[6] = Sbox[s[6]]; s[7] = Sbox[s[7]];
    s[8] = Sbox[s[8]]; s[9] = Sbox[s[9]]; s[10] = Sbox[s[10]]; s[11] = Sbox[s[11]];
    s[12] = Sbox[s[12]]; s[13] = Sbox[s[13]]; s[14] = Sbox[s[14]]; s[15] = Sbox[s[15]];
}

void ShiftRows(uint8_t S[16]){
  uint8_t temp;
  temp = S[1]; S[1] = S[5]; S[5] = S[9]; S[9] = S[13]; S[13] = temp;
  temp = S[2]; S[2] = S[10]; S[10] = temp; 
  temp = S[6]; S[6] = S[14]; S[14] = temp;
  temp = S[15]; S[15] = S[11]; S[11] = S[7]; S[7] = S[3]; S[3] = temp;
}

#define MUL2(a) (a<<1)^(a&0x80?0x1b:0)
#define MUL3(a) (MUL2(a))^(a)

void MixColumns(uint8_t S[16]){
    int i;
    uint8_t temp[16];

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

void AES_ENC(uint8_t PT[16], uint8_t RK[176], uint8_t CT[16]){
    int Nr = 10;
    int i;
    uint8_t temp[16];

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

void setup(){
  pinMode(PIN_TRIGG, OUTPUT);
  digitalWrite(PIN_TRIGG,LOW);
  Serial.begin(115200);
}

void loop(){
  SerialEvent();
}

void SerialEvent(){
  uint8_t input[16]={0,};
  uint8_t output[16]={0,};
  char ioc[8];
  int i;
  uint8_t MK[16]={ 0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,
                 0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0 };

  while(Serial.available()){
      buffer_index = Serial.readBytes((char*)buffer_rx, 33);
  }

  //sprintf(ioc, "%02X", buffer_index);
  //Serial.print(ioc);

  if(buffer_index != 0) {
    for(uint8_t p=0; p<16; p++) {
        input[p] = (strToHex(buffer_rx[2*p])<<4) ^ (strToHex(buffer_rx[2*p+1]));
    }

    RoundKeyGeneration128(MK, RK);
    AES_ENC(input, RK, output);
    
    for(i=0; i<16; i++){
      sprintf(ioc, "%02X", output[i]);
      Serial.print(ioc);
    }

    Serial.print("\n");

  }

  buffer_index=0;
  Serial.flush();

}
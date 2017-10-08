// rf69_server.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing server
// with the RF69 class. RF69 class does not provide for addressing or reliability.
// It is designed to work with the other example rf69_client
// Demonstrates the use of AES encrpyion.

#include <SPI.h>
#include <RF69.h>

// Singleton instance of the radio
RF69 rf69;

uint8_t crc8_table[] = {
    0x00,0x5E,0xBC,0xE2,0x61,0x3F,0xDD,0x83,
    0xC2,0x9C,0x7E,0x20,0xA3,0xFD,0x1F,0x41,
    0x9D,0xC3,0x21,0x7F,0xFC,0xA2,0x40,0x1E,
    0x5F,0x01,0xE3,0xBD,0x3E,0x60,0x82,0xDC,
    0x23,0x7D,0x9F,0xC1,0x42,0x1C,0xFE,0xA0,
    0xE1,0xBF,0x5D,0x03,0x80,0xDE,0x3C,0x62,
    0xBE,0xE0,0x02,0x5C,0xDF,0x81,0x63,0x3D,
    0x7C,0x22,0xC0,0x9E,0x1D,0x43,0xA1,0xFF,
    0x46,0x18,0xFA,0xA4,0x27,0x79,0x9B,0xC5,
    0x84,0xDA,0x38,0x66,0xE5,0xBB,0x59,0x07,
    0xDB,0x85,0x67,0x39,0xBA,0xE4,0x06,0x58,
    0x19,0x47,0xA5,0xFB,0x78,0x26,0xC4,0x9A,
    0x65,0x3B,0xD9,0x87,0x04,0x5A,0xB8,0xE6,
    0xA7,0xF9,0x1B,0x45,0xC6,0x98,0x7A,0x24,
    0xF8,0xA6,0x44,0x1A,0x99,0xC7,0x25,0x7B,
    0x3A,0x64,0x86,0xD8,0x5B,0x05,0xE7,0xB9,
    0x8C,0xD2,0x30,0x6E,0xED,0xB3,0x51,0x0F,
    0x4E,0x10,0xF2,0xAC,0x2F,0x71,0x93,0xCD,
    0x11,0x4F,0xAD,0xF3,0x70,0x2E,0xCC,0x92,
    0xD3,0x8D,0x6F,0x31,0xB2,0xEC,0x0E,0x50,
    0xAF,0xF1,0x13,0x4D,0xCE,0x90,0x72,0x2C,
    0x6D,0x33,0xD1,0x8F,0x0C,0x52,0xB0,0xEE,
    0x32,0x6C,0x8E,0xD0,0x53,0x0D,0xEF,0xB1,
    0xF0,0xAE,0x4C,0x12,0x91,0xCF,0x2D,0x73,
    0xCA,0x94,0x76,0x28,0xAB,0xF5,0x17,0x49,
    0x08,0x56,0xB4,0xEA,0x69,0x37,0xD5,0x8B,
    0x57,0x09,0xEB,0xB5,0x36,0x68,0x8A,0xD4,
    0x95,0xCB,0x29,0x77,0xF4,0xAA,0x48,0x16,
    0xE9,0xB7,0x55,0x0B,0x88,0xD6,0x34,0x6A,
    0x2B,0x75,0x97,0xC9,0x4A,0x14,0xF6,0xA8,
    0x74,0x2A,0xC8,0x96,0x15,0x4B,0xA9,0xF7,
    0xB6,0xE8,0x0A,0x54,0xD7,0x89,0x6B,0x35,
};

unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
unsigned long interval1 = 30000; 
unsigned long interval2 = 30000; 
uint8_t auto1 = 0;
uint8_t auto2 = 0;
uint8_t tualet = 0;
uint8_t vanna = 0;

void setup() 
{
  Serial.begin(9600);
  if (!rf69.init())
    Serial.println("RF69 init failed");
  // Defaults after init are 434.0MHz, modulation FSK_Rb2Fd5, +13dbM
  // No encryption
  Serial.println("RF69");
  pinMode(6, OUTPUT); 
  pinMode(7, OUTPUT); 
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);
}

void loop()
{
  
  if (rf69.available())
  {
    // Should be a message for us now   
    uint8_t buf[60];
    uint8_t len = sizeof(buf);
    uint8_t buflen = len;
    uint8_t cmd = 0;
    uint8_t data[4];
    uint8_t adress[2];
    uint8_t fmt = 0;
    uint8_t crc = 0;
    uint8_t crc2 = 0;
    uint8_t f_bit = 0;
    adress[0] = 0;
    adress[1] = 0;
    
    uint8_t adress1[2]; //туалет
    adress1[0] = 0xC5;
    adress1[1] = 0x29;
    
    uint8_t adress1s[2]; //туалет сценарий
    adress1s[0] = 0xC4;
    adress1s[1] = 0x29;
    
    uint8_t adress2[2]; //ванна
    adress2[0] = 0x2B;
    adress2[1] = 0x2A;    

    uint8_t adress2s[2]; //ванна сценарий
    adress2s[0] = 0x2A;
    adress2s[1] = 0x2A; 
    
    uint8_t adress11[2]; // свет туалет
    adress11[0] = 0xB1;
    adress11[1] = 0x55;
    
    uint8_t adress22[2]; //свет ванна
    adress22[0] = 0xB0;
    adress22[1] = 0x55; 
    
    if (rf69.recv(buf, &len))
    {
        int i;
        int j = 0;
        
        buf[59] = 0;
        while ((buf[0] & 0xE0) > 0) {
            for (i = 0; i < 60; i++) {
                buf[i] <<= 1;
                buf[i] = buf[i] | (buf[i + 1] >> 7);
            }
        }
        
        buf[59] = 1;
        
        while ((buf[0] & 0x40) != 0x40) {
        for (i = 0; i < 60; i++) {
            buf[i] <<= 1;
            buf[i] = buf[i] | (buf[i + 1] >> 7);
        }
        }

        i = 0;
        buf[59] = 0;
        while (!((((buf[i / 8] >> (7 - i % 8)) & 0x1) == 0) && (((buf[(i + 1) / 8] >> (7 - (i + 1) % 8)) & 0x1) == 0) && (((buf[(i + 2) / 8] >> (7 - (i + 2) % 8)) & 0x1) == 0))) {
            i++;
        }
        
        //Serial.println(i);
        
        if (!(i % 2)) {
            i--;
        }
        
        for (j = 0; j < 8; j++) {
            crc <<= 1;
            crc = (crc | (((buf[i / 8] >> (7 - i % 8)) & 0x1)));
            i = i - 2;
        }

        for (j = 0; j < 8; j++) {
            fmt <<= 1;
            fmt = (fmt | (((buf[i / 8] >> (7 - i % 8)) & 0x1)));
            i = i - 2;
        }

        for (j = 0; j < 8; j++) {
            adress[1] <<= 1;
            adress[1] = (adress[1] | (((buf[i / 8] >> (7 - i % 8)) & 0x1)));
            i = i - 2;
        }

        for (j = 0; j < 8; j++) {
            adress[0] <<= 1;
            adress[0] = (adress[0] | (((buf[i / 8] >> (7 - i % 8)) & 0x1)));
            i = i - 2;
        }

        if (fmt = 0) {
            data[0] = 0;
            data[1] = 0;
            data[2] = 0;
            data[3] = 0;
        }

        j = 0;

        for (j = 0; j < 4; j++) {
            cmd <<= 1;
            cmd = (cmd | (((buf[i / 8] >> (7 - i % 8)) & 0x1)));
            i = i - 2;
        }
        
        f_bit = ((buf[i / 8] >> (7 - i % 8)) & 0x1);
        
        
        crc2 = 0;
        crc2 = crc8_table[(((cmd << 1) | f_bit) << 3) ^ crc2];
        crc2 = crc8_table[adress[0] ^ crc2];
        crc2 = crc8_table[adress[1] ^ crc2];
        crc2 = crc8_table[fmt ^ crc2];
 
       // for (i = 0; i < buflen; i++) {
        //    Serial.print(buf[i], BIN);
            //   Serial.print(mas[i], HEX);
       //     Serial.print(" ");
       // } 
       // Serial.println(" ");
        
       if ((crc == crc2) && (crc != 0))
        {
         Serial.print("Got: ");
         Serial.println("");
        digitalWrite(13, false);
        Serial.print(cmd, HEX);
        Serial.print(" ");
        Serial.print(adress[0], HEX);
        Serial.print(" ");
        Serial.print(adress[1], HEX);
        Serial.print(" ");
        Serial.print(fmt, HEX);
        Serial.print(" ");
        Serial.print(f_bit, HEX);
        Serial.print(" ");
        Serial.print(crc, DEC);
        Serial.print(" ");
        Serial.println("");
        
        Serial.print(crc2, DEC);
        Serial.print(" ");
        Serial.println("");
      } else {
      
      /// 2 paket
        i = 0;
        j = 0;
        buf[59] = 0;
        while ((buf[0] & 0xE0) > 0) {
            for (i = 0; i < 60; i++) {
                buf[i] <<= 1;
                buf[i] = buf[i] | (buf[i + 1] >> 7);
            }
        }
        
        buf[59] = 1;
        
        while ((buf[0] & 0x40) != 0x40) {
        for (i = 0; i < 60; i++) {
            buf[i] <<= 1;
            buf[i] = buf[i] | (buf[i + 1] >> 7);
        }
        }

        i = 0;
        buf[59] = 0;
        while (!((((buf[i / 8] >> (7 - i % 8)) & 0x1) == 0) && (((buf[(i + 1) / 8] >> (7 - (i + 1) % 8)) & 0x1) == 0) && (((buf[(i + 2) / 8] >> (7 - (i + 2) % 8)) & 0x1) == 0))) {
            i++;
        }
         
        crc = 0;
        fmt = 0;
        adress[0] = 0;
        adress[1] = 0; 
        cmd = 0;
         
        if (!(i % 2)) {
            i--;
        }
        
        for (j = 0; j < 8; j++) {
            crc <<= 1;
            crc = (crc | (((buf[i / 8] >> (7 - i % 8)) & 0x1)));
            i = i - 2;
        }

        for (j = 0; j < 8; j++) {
            fmt <<= 1;
            fmt = (fmt | (((buf[i / 8] >> (7 - i % 8)) & 0x1)));
            i = i - 2;
        }

        for (j = 0; j < 8; j++) {
            adress[1] <<= 1;
            adress[1] = (adress[1] | (((buf[i / 8] >> (7 - i % 8)) & 0x1)));
            i = i - 2;
        }

        for (j = 0; j < 8; j++) {
            adress[0] <<= 1;
            adress[0] = (adress[0] | (((buf[i / 8] >> (7 - i % 8)) & 0x1)));
            i = i - 2;
        }

        if (fmt = 0) {
            data[0] = 0;
            data[1] = 0;
            data[2] = 0;
            data[3] = 0;
        }

        j = 0;

        for (j = 0; j < 4; j++) {
            cmd <<= 1;
            cmd = (cmd | (((buf[i / 8] >> (7 - i % 8)) & 0x1)));
            i = i - 2;
        }
        
        f_bit = ((buf[i / 8] >> (7 - i % 8)) & 0x1);
        
        
        crc2 = 0;
        crc2 = crc8_table[(((cmd << 1) | f_bit) << 3) ^ crc2];
        crc2 = crc8_table[adress[0] ^ crc2];
        crc2 = crc8_table[adress[1] ^ crc2];
        crc2 = crc8_table[fmt ^ crc2];
        
       if ((crc == crc2) && (crc != 0))
        {
         Serial.print("Got2: ");
         Serial.println("");
        digitalWrite(13, false);
        Serial.print(cmd, HEX);
        Serial.print(" ");
        Serial.print(adress[0], HEX);
        Serial.print(" ");
        Serial.print(adress[1], HEX);
        Serial.print(" ");
        Serial.print(fmt, HEX);
        Serial.print(" ");
        Serial.print(f_bit, HEX);
        Serial.print(" ");
        Serial.print(crc, DEC);
        Serial.print(" ");
        Serial.println("");
        
        Serial.print(crc2, DEC);
        Serial.print(" ");
        Serial.println("");
      }
      
      }
      
      // алгоритм работы
      // включение
      if (cmd == 2){
        if ((adress[0] == adress1[0]) && (adress[1] == adress1[1])){
          if (interval1 >= 60000) {
            interval1 = interval1 + 30000;
          } else {
            interval1 = interval1 + 10000;
          }
        }
        if ((adress[0] == adress2[0]) && (adress[1] == adress2[1])){
          if (interval2 >= 60000) {
            interval2 = interval2 + 30000;
          } else {
            interval2 = interval2 + 10000;
          }
        }
        if ((adress[0] == adress11[0]) && (adress[1] == adress11[1])){
          if (auto1) {
            tualet = 1;
            previousMillis1 = millis();
            if (previousMillis1 > 4294367294){
              previousMillis1 = 4294367294;
            }
            digitalWrite(6, LOW);
          }
        }
        if ((adress[0] == adress22[0]) && (adress[1] == adress22[1])){
          if (auto2) {
            vanna = 1;
            previousMillis2 = millis();
            if (previousMillis2 > 4294367294){
              previousMillis2 = 4294367294;
            }
          }
        }
        
      }
      //
      
      // выключение
      if (cmd == 0){
        if ((adress[0] == adress1[0]) && (adress[1] == adress1[1])){
          if (interval1 >= 60000) {
            interval1 = interval1 - 30000;
          } else { 
              if (interval1 > 10000) {
              interval1 = interval1 - 10000;
            }
          }
        }
        if ((adress[0] == adress2[0]) && (adress[1] == adress2[1])){
          if (interval2 >= 60000) {
            interval2 = interval2 - 30000;
          } else {
              if (interval2 > 10000) {
              interval2 = interval2 - 10000;
            }
          }
        }
        if ((adress[0] == adress11[0]) && (adress[1] == adress11[1])){
          if (auto1) {
            tualet = 0;
            previousMillis1 = millis();
            if (previousMillis1 > 4294967000 - interval1){
              previousMillis1 = 4294967000 - interval1;
            }
            digitalWrite(6, LOW);
          }
        }
        if ((adress[0] == adress22[0]) && (adress[1] == adress22[1])){
          if (auto2) {
            vanna = 0;
            previousMillis2 = millis();
            if (previousMillis2 > 4294967000 - interval2){
              previousMillis2 = 4294967000 - interval2;
            }
            digitalWrite(7, LOW);
          }
        }
        
      }
      ///// сценарий
      if (cmd == 7){
        if ((adress[0] == adress1s[0]) && (adress[1] == adress1s[1])){
           if (!auto1) {
            tualet = 1;
            previousMillis1 = millis();
            if (previousMillis1 > 4294967000 - interval1){
              previousMillis1 = 4294967000 - interval1;
            }
            digitalWrite(6, LOW);}
        }
        if ((adress[0] == adress2s[0]) && (adress[1] == adress2s[1])){
            vanna = 1;
            previousMillis2 = millis();
            if (previousMillis2 > 4294967000 - interval2){
              previousMillis2 = 4294967000 - interval2;
            }
            digitalWrite(7, LOW);
        }
      }
      /////
      ///// переключение режима
      if (cmd == 3){
        if ((adress[0] == adress1[0]) && (adress[1] == adress1[1])){
          auto1 = 1;
        }
        if ((adress[0] == adress2[0]) && (adress[1] == adress2[1])){
          auto2 = 1;
        }
      }
      if (cmd == 1){
        if ((adress[0] == adress1[0]) && (adress[1] == adress1[1])){
          auto1 = 0;
        }
        if ((adress[0] == adress2[0]) && (adress[1] == adress2[1])){
          auto2 = 0;
        }
      }
      if (cmd == 8){ //reset
        if ((adress[0] == adress1s[0]) && (adress[1] == adress1s[1])){
          auto1 = 0;
          tualet = 0;
          interval1 = 30000;
        }
        if ((adress[0] == adress2s[0]) && (adress[1] == adress2s[1])){
          auto2 = 0;
          vanna = 0;
          interval2 = 30000;
        }
      }
      /////
      
      
    }
    else
    {
      Serial.println("recv failed");
    }
  }
  
  unsigned long currentMillis = millis();
  
  if (tualet) {
    if (auto1){
      if(currentMillis - previousMillis1 > 600000) {
        digitalWrite(6, HIGH);
        tualet = 0;
        previousMillis1 = currentMillis;
      }      
    } else {
      if(currentMillis - previousMillis1 > interval1) {
        digitalWrite(6, HIGH);
        tualet = 0;
        previousMillis1 = currentMillis;
      }
    }
  } else{
    if (auto1) {
      if(currentMillis - previousMillis1 > interval1) {
        digitalWrite(6, HIGH);
        tualet = 0;
        previousMillis1 = currentMillis;
    }      
  }
  }
  
  if (vanna) {
      if(currentMillis - previousMillis2 > interval2) {
        digitalWrite(7, HIGH);
        vanna = 0;
        previousMillis2 = currentMillis;
      }
  } else {
    if (auto2) {
      if(currentMillis - previousMillis2 > interval2) {
        digitalWrite(7, HIGH);
        tualet = 0;
        previousMillis2 = currentMillis;
    }      
  }
  }
  
}


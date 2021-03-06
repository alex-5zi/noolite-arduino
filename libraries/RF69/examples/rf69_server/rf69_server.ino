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

void setup() 
{
  Serial.begin(9600);
  if (!rf69.init())
    Serial.println("RF69 init failed");
  // Defaults after init are 434.0MHz, modulation FSK_Rb2Fd5, +13dbM
  // No encryption
  if (!rf69.setFrequency(433))
    Serial.println("RF69 setFrequency failed");
   if (!rf69.setModemConfig(RF69::GFSK_Rb250Fd250))
    Serial.println("RF69 setModemConfig failed");

 // The encryption key has to be the same as the one in the client
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69.setEncryptionKey(key);
  
#if 0
  // For compat with RFM69 Struct_send
  rf69.setModemConfig(RF69::GFSK_Rb250Fd250);
  rf69.setPreambleLength(3);
  uint8_t syncwords[] = { 0x2d, 0x64 };
  rf69.setSyncWords(syncwords, sizeof(syncwords));
  rf69.setEncryptionKey((uint8_t*)"thisIsEncryptKey");
#endif
}

void loop()
{
  if (rf69.available())
  {
    // Should be a message for us now   
    uint8_t buf[RF69_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf69.recv(buf, &len))
    {
//      RF69::printBuffer("request: ", buf, len);
      Serial.print("got request: ");
      Serial.println((char*)buf);
//      Serial.print("RSSI: ");
//      Serial.println(rf69.lastRssi(), DEC);
      
      // Send a reply
      uint8_t data[] = "And hello back to you";
      rf69.send(data, sizeof(data));
      rf69.waitPacketSent();
      Serial.println("Sent a reply");
    }
    else
    {
      Serial.println("recv failed");
    }
  }
}


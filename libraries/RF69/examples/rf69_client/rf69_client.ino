// rf69_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the RF69 class. RF69 class does not provide for addressing or reliability.
// It is designed to work with the other example rf69_server.
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
  if (!rf69.setFrequency(433.0))
    Serial.println("RF69 setFrequency failed");
  if (!rf69.setModemConfig(RF69::GFSK_Rb250Fd250))
    Serial.println("RF69 setModemConfig failed");
  
  // The encryption key has to be the same as the one in the server
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69.setEncryptionKey(key);
}


void loop()
{
  while (1)
  {
    Serial.println("Sending to rf69_server");
    // Send a message to rf69_server
    uint8_t data[] = "Hello World!";
    rf69.send(data, sizeof(data));
   
    rf69.waitPacketSent();
    // Now wait for a reply
    uint8_t buf[RF69_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf69.waitAvailableTimeout(500))
    { 
      // Should be a reply message for us now   
      if (rf69.recv(buf, &len))
      {
        Serial.print("got reply: ");
        Serial.println((char*)buf);
      }
      else
      {
        Serial.println("recv failed");
      }
    }
    else
    {
      Serial.println("No reply, is rf69_server running?");
    }
    delay(400);
  }
}


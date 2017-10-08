// Input pin 11 

#include <VirtualWire.h>

byte cmd;
byte data[4];
byte adress[2];
byte fmt;
byte crc;

void setup() {
    Serial.begin(9600); // Debugging only
    Serial.println("setup");

    // Initialise the IO and ISR
    vw_set_ptt_inverted(true); 0
    vw_setup(2000); // Bits per sec

    vw_rx_start(); 
}

void loop() {
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;
    uint8_t cmd;
    uint8_t data[4];
    uint8_t adress[2];
    uint8_t fmt;
    uint8_t crc;


    if (vw_get_message(buf, & buflen)) 
    {
        int i;
        int j = 0;
        while ((buf[0] & 0xE0) > 0) {
            for (i = 0; i < 80; i++) {
                buf[i] <<= 1;
                buf[i] = buf[i] | (buf[i + 1] >> 7);
            }
        }
        for (i = 0; i < 80; i++) {
            buf[i] <<= 1;
            buf[i] = buf[i] | (buf[i + 1] >> 7);
        }
        for (i = 0; i < 80; i++) {
            buf[i] <<= 1;
            buf[i] = buf[i] | (buf[i + 1] >> 7);
        }
        for (i = 0; i < 80; i++) {
            buf[i] <<= 1;
            buf[i] = buf[i] | (buf[i + 1] >> 7);
        }

        i = 0;
        while (!((((buf[i / 8] >> (7 - i % 8)) & 0x1) == 0) && (((buf[(i + 1) / 8] >> (7 - (i + 1) % 8)) & 0x1) == 0) && (((buf[(i + 2) / 8] >> (7 - (i + 2) % 8)) & 0x1) == 0))) {
            i++;
        }

        if (!(i % 2)) {
            i++;
        }

        for (j = 0; j < 8; j++) {
            crc = (crc | (((buf[i / 8] >> (7 - i % 8)) & 0x1) << 7)) >> 1;
            i = i - 2;
        }

        for (j = 0; j < 8; j++) {
            fmt = (fmt | (((buf[i / 8] >> (7 - i % 8)) & 0x1) << 7)) >> 1;
            i = i - 2;
        }

        for (j = 0; j < 8; j++) {
            adress[1] = (adress[1] | (((buf[i / 8] >> (7 - i % 8)) & 0x1) << 7)) >> 1;
            i = i - 2;
        }

        for (j = 0; j < 8; j++) {
            adress[0] = (adress[0] | (((buf[i / 8] >> (7 - i % 8)) & 0x1) << 7)) >> 1;
            i = i - 2;
        }

        if (fmt = 0) {
            data[0] = 0;
            data[1] = 0;
            data[2] = 0;
            data[3] = 0;
        }

        j = 0;

        while ((j < 8) || (i >= 0)) {
            j++;
            cmd = (cmd | (((buf[i / 8] >> (7 - i % 8)) & 0x1) << 7)) >> 1;
            i = i - 2;
        }
        cmd = cmd >> (8 - j);


        // Message 
        Serial.print("Got: ");
        Serial.println("");
        Serial.print("cmd: ");
        Serial.println(cmd, BIN);
        Serial.print("adress[0]: ");
        Serial.println(adress[0], BIN);
        Serial.print("adress[1]: ");
        Serial.println(adress[1], BIN);
        Serial.print("fmt: ");
        Serial.println(fmt, BIN);
        Serial.print("crc: ");
        Serial.println(crc, BIN);
        Serial.println("");


    }
}

#include <WiFi.h>

const char *ssid = "WifiOBD";
const char *senha = "minhasenha";
WiFiServer wifiServer(333);

void setup() {
  Serial.begin(115200);
  // WiFi.disconnect(true);
  delay(500);  
  Serial.printf("Configurando ponto de acesso '%s - %s'\n", ssid, senha);      

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, senha);
  wifiServer.begin();
  Serial.println("Configuração concluída");              
}

unsigned int const wCRC16Table[256] = { 
	0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
	0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
	0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40, 
	0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
	0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,  
	0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
	0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
	0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
	0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
	0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,   
	0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
	0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
	0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
	0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
	0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,   
	0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
	0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
	0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
	0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
	0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,   
	0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
	0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
	0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
	0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
	0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,  
	0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
	0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
	0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
	0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
	0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,  
	0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
	0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

uint16_t getBufCRC( uint8_t *buf, uint16_t len) {
  uint16_t wResult = 0x5566;
  uint16_t wTableNo = 0;
  uint16_t i;  

  for(i = 0; i < len; i++)
  {
      wTableNo = ((wResult & 0xff) ^ (buf[i] & 0xff));
      wResult = ((wResult >> 8) & 0xff) ^ wCRC16Table[wTableNo];
  }
  return wResult;
}

void loop() {
  // put your main code here, to run repeatedly:
  WiFiClient client = wifiServer.available();  
  if (client) {
 
    while (client.connected()) {      
      int sizeAvailable = 0;
      if ((sizeAvailable = client.available()) <= 0) continue;            
      char* dataReceived = new char[sizeAvailable];            
      client.readBytes(dataReceived, sizeAvailable);
      Serial.println(sizeAvailable);

      if (sizeAvailable >= 8) {
        if (dataReceived[4] == 0x82) {
          char command[455] = {0x55,0xAA,0xC1,0x01,0x82,0x27,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x0A,0x41,0x6C,0x66,0x61,0x20,0x52,0x6F,0x6D,0x65,0x6F,0x02,0x00,0x00,0x00,0x0C,0x41,0x73,0x74,0x6F,0x6E,0x20,0x4D,0x61,0x72,0x74,0x69,0x6E,0x03,0x00,0x00,0x00,0x04,0x41,0x75,0x64,0x69,0x04,0x00,0x00,0x00,0x03,0x42,0x6D,0x77,0x05,0x00,0x00,0x00,0x09,0x43,0x68,0x65,0x76,0x72,0x6F,0x6C,0x65,0x74,0x06,0x00,0x00,0x00,0x08,0x43,0x68,0x72,0x79,0x73,0x6C,0x65,0x72,0x07,0x00,0x00,0x00,0x07,0x63,0x69,0x74,0x72,0x6F,0x65,0x6E,0x08,0x00,0x00,0x00,0x05,0x44,0x61,0x63,0x69,0x61,0x09,0x00,0x00,0x00,0x06,0x44,0x61,0x74,0x73,0x75,0x6E,0x0A,0x00,0x00,0x00,0x05,0x44,0x6F,0x64,0x67,0x65,0x0B,0x00,0x00,0x00,0x04,0x46,0x69,0x61,0x74,0x0C,0x00,0x00,0x00,0x04,0x46,0x6F,0x72,0x64,0x0D,0x00,0x00,0x00,0x05,0x48,0x6F,0x6E,0x64,0x61,0x0E,0x00,0x00,0x00,0x07,0x48,0x79,0x75,0x6E,0x64,0x61,0x69,0x0F,0x00,0x00,0x00,0x06,0x4A,0x61,0x67,0x75,0x61,0x72,0x10,0x00,0x00,0x00,0x04,0x4A,0x65,0x65,0x70,0x11,0x00,0x00,0x00,0x03,0x4B,0x69,0x61,0x12,0x00,0x00,0x00,0x06,0x4C,0x61,0x6E,0x63,0x69,0x61,0x13,0x00,0x00,0x00,0x09,0x4C,0x61,0x6E,0x64,0x72,0x6F,0x76,0x65,0x72,0x14,0x00,0x00,0x00,0x05,0x4C,0x65,0x78,0x75,0x73,0x15,0x00,0x00,0x00,0x08,0x4D,0x61,0x73,0x65,0x72,0x61,0x74,0x69,0x16,0x00,0x00,0x00,0x05,0x4D,0x61,0x7A,0x64,0x61,0x17,0x00,0x00,0x00,0x08,0x4D,0x65,0x72,0x63,0x65,0x64,0x65,0x73,0x18,0x00,0x00,0x00,0x04,0x4D,0x69,0x6E,0x69,0x19,0x00,0x00,0x00,0x0A,0x4D,0x69,0x74,0x73,0x75,0x62,0x69,0x73,0x68,0x69,0x1A,0x00,0x00,0x00,0x06,0x4E,0x69,0x73,0x73,0x61,0x6E,0x1B,0x00,0x00,0x00,0x04,0x4F,0x70,0x65,0x6C,0x1C,0x00,0x00,0x00,0x07,0x50,0x65,0x75,0x67,0x65,0x6F,0x74,0x1D,0x00,0x00,0x00,0x07,0x50,0x6F,0x72,0x73,0x63,0x68,0x65,0x1E,0x00,0x00,0x00,0x07,0x52,0x65,0x6E,0x61,0x75,0x6C,0x74,0x1F,0x00,0x00,0x00,0x05,0x53,0x63,0x69,0x6F,0x6E,0x20,0x00,0x00,0x00,0x04,0x53,0x65,0x61,0x74,0x21,0x00,0x00,0x00,0x05,0x73,0x6B,0x6F,0x64,0x61,0x22,0x00,0x00,0x00,0x05,0x53,0x6D,0x61,0x72,0x74,0x23,0x00,0x00,0x00,0x06,0x53,0x75,0x62,0x61,0x72,0x75,0x24,0x00,0x00,0x00,0x06,0x54,0x6F,0x79,0x6F,0x74,0x61,0x25,0x00,0x00,0x00,0x08,0x56,0x61,0x75,0x78,0x68,0x61,0x6C,0x6C,0x26,0x00,0x00,0x00,0x05,0x56,0x6F,0x6C,0x76,0x6F,0x27,0x00,0x00,0x00,0x02,0x56,0x57,0x64,0x5E,0xBC,0x72,0xB9,0xA2,0xF3,0x27,0x2D,0x24,0x79,0x5C,0x48,0x92,0xA8,0xA3,0x2F};
          client.write(command, 455);      
        }

        if (dataReceived[4] == 0x83) {
          for (int i = 0; i < 10; i++) {
            
            // char command[33] = {0x55,0xAA,0x1B,0x00,0x83,0x04,0x00,0x00,0x00,0x11,0x57,0x42,0x41,0x5A,0x56,0x34,0x31,0x30,0x32,0x44,0x4C,0x39,0x33,0x32,0x35,0x35,0x35,0x03,0x43,0x41,0x53,0x7B,0xA8};
            char command[33] = {0x55,0xaa,0x1b,0x00,0x83,0x24,0x00,0x00,0x00,0x11,0x41,0x48,0x54,0x4a,0x41,0x33,0x47,0x53,0x31,0x30,0x30,0x32,0x32,0x32,0x34,0x33,0x32,0x03,0x45,0x43,0x4d,0x40,0x9a};
            client.write(command, 33);                        
            delay(500);

            unsigned char progressAtual = (i+1)*10;
            unsigned char progress[11] = {0x55,0xAA,0x05,0x00,0x84,progressAtual,0x00,0x00,0x00};
            uint16_t crc = getBufCRC(progress, 9);            
            
            progress[9] = crc % 256;
            progress[10] = crc / 256;            

            client.write(progress, 11);
            delay(500);
          }

          char end[7] = {0x55,0xAA,0x01,0x00,0x85,0xA4,0x73};
          client.write(end, 7);
        }
      }
       
      delay(10);
    }
 
    client.stop();
    Serial.println("Client disconnected");
 
  }

}
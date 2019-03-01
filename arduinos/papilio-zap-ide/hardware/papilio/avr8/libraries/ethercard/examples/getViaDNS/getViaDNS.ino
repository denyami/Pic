// This demo does web requests via DNS lookup, using a fixed gateway.
// 2010-11-27 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

#include <EtherCard.h>

#define REQUEST_RATE 5000 // milliseconds

// ethernet interface mac address
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
// ethernet interface ip address
static byte myip[] = { 192,168,1,203 };
// gateway ip address
static byte gwip[] = { 192,168,1,1 };
// remote website name
char website[] PROGMEM = "google.com";

byte Ethernet::buffer[300];   // a very small tcp/ip buffer is enough here
static long timer;

// called when the client request is complete
static void my_result_cb (byte status, word off, word len) {
  Serial.print("<<< reply ");
  Serial.print(millis() - timer);
  Serial.println(" ms");
  Serial.println((const char*) Ethernet::buffer + off);
}

void setup () {
  delay(3000);
  
  // set up pins to use Ethernet Wing on Wingslot AL
  SPI1_cs_LOC   = WAL6;
  SPI1_sck_LOC  = WAL5;
  SPI1_mosi_LOC = WAL4;
  SPI1_miso_LOC = WAL3;
  
  Serial.begin(57600);
  Serial.println("\n[getViaDNS]");
  
  if (ether.begin(sizeof Ethernet::buffer, mymac, SPI1_cs_LOC, SPI1_sck_LOC, SPI1_mosi_LOC, SPI1_miso_LOC) == 0) 
    Serial.println( "Failed to access Ethernet controller");

  ether.staticSetup(myip, gwip);

  if (!ether.dnsLookup(website))
    Serial.println("DNS failed");
  ether.printIp("Server: ", ether.hisip);
  
  timer = - REQUEST_RATE; // start timing out right away
}

void loop () {
  ether.packetLoop(ether.packetReceive());
  
  if (millis() > timer + REQUEST_RATE) {
    timer = millis();
    Serial.println("\n>>> REQ");
    ether.browseUrl(PSTR("/foo/"), "bar", website, my_result_cb);
  }
}

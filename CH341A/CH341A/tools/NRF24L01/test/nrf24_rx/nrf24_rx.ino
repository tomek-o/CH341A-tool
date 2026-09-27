/*
  nRF24L01 test receiver for the CH341A tool "nRF24L01+ transmitter/receiver"
  (tool in Transmitter mode).

  Prints every received payload on Serial (115200) as hex + ASCII.
  With auto-ack enabled the tool should report "Sent OK (ACK received)".

  Requires the "RF24" library by TMRh20 (Arduino Library Manager).

  Wiring (Arduino Uno/Nano, ATmega328P):
    nRF24  VCC  -> 3.3 V  (NOT 5 V; a 10 uF cap across VCC/GND helps a lot)
           GND  -> GND
           CE   -> D9   (default; see pin auto-detection below)
           CSN  -> D10
           SCK  -> D13
           MOSI -> D11
           MISO -> D12
           IRQ  -> not connected

  CE/CSN pin auto-detection: the sketch tries these CE/CSN pairs in order and
  uses the first one where the chip answers:
    D9/D10  - separate module, most common wiring (tutorials, adapter shields)
    D10/D9  - RF-Nano (Keywish/Emakefun Nano with nRF24L01+ on the same PCB)
    D7/D8   - wiring used by the current RF24 library examples
  Trying a wrong pair is harmless (CE and CSN are both nRF inputs). For another
  board, add its pair to the CE_CSN_PINS table. RF-Nano note: the radio is fed
  from the board's own 3.3 V regulator, so the VCC/capacitor wiring above does
  not apply - but some cheap versions are still marginal under TX load.

  Settings below match the tool defaults - change both sides together.
*/

#include <SPI.h>
#include <RF24.h>
#include <printf.h>              // from RF24 library: routes printf() to Serial for printDetails()

// CE/CSN pairs, tried in this order - first entry is the default wiring
struct PinOption {
  uint8_t ce;
  uint8_t csn;
  const char *description;
};
const PinOption CE_CSN_PINS[] = {
  { 9, 10, "separate nRF24L01 module or adapter/IO shield (most common tutorial wiring)" },
  { 10, 9, "RF-Nano (Keywish/Emakefun Nano with on-board nRF24L01+) or similar all-in-one board" },
  { 7, 8,  "separate module wired as in the current RF24 library examples" },
};

#define RF_CHANNEL      5               // tool: "RF channel"
#define RF_DATA_RATE    RF24_2MBPS      // tool: "RF speed" (RF24_250KBPS / RF24_1MBPS / RF24_2MBPS)
#define PAYLOAD_LENGTH  32              // tool: "Payload length"
#define AUTO_ACK        true            // tool: "Enable Auto-ACK"

// Written to the chip byte by byte, LSByte first - exactly like the tool
const uint8_t address[5] = { 0x11, 0x22, 0x33, 0x44, 0x55 };

RF24 radio(CE_CSN_PINS[0].ce, CE_CSN_PINS[0].csn);   // pins re-set by begin(ce, csn) below
uint32_t received = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println(F("nRF24 test receiver"));

  bool found = false;
  const uint8_t optionCount = sizeof(CE_CSN_PINS) / sizeof(CE_CSN_PINS[0]);
  Serial.println(F("Looking for nRF24 on known CE/CSN pin pairs:"));
  for (uint8_t i = 0; i < optionCount; i++) {
    const PinOption &opt = CE_CSN_PINS[i];
    Serial.print(F("  CE = D"));
    Serial.print(opt.ce);
    Serial.print(F(", CSN = D"));
    Serial.print(opt.csn);
    Serial.print(F(" ... "));
    if (radio.begin(opt.ce, opt.csn) && radio.isChipConnected()) {
      Serial.println(F("found"));
      Serial.print(F("Probable hardware: "));
      Serial.println(opt.description);
      // RF_SETUP bit 5 (RF_DR_LOW, 250 kbps) only exists on the + variant; clones
      // (e.g. Si24R1, often marked "NRF24L01+") also report as + here
      Serial.print(F("Chip: "));
      Serial.println(radio.isPVariant()
                     ? F("nRF24L01+ or compatible clone (250 kbps supported)")
                     : F("original nRF24L01 (no 250 kbps) or clone without RF_DR_LOW"));
      if (i != 0) {
        Serial.println(F("Note: not the default wiring - pins were auto-detected"));
      }
      found = true;
      break;
    }
    Serial.println(F("no response"));
    // leave the unsuccessful CSN pin deselected (high) before trying the next pair
    pinMode(opt.csn, OUTPUT);
    digitalWrite(opt.csn, HIGH);
  }
  if (!found) {
    Serial.println(F("nRF24 not responding on any known CE/CSN pins. Check:"));
    Serial.println(F("  - SCK/MOSI/MISO on D13/D11/D12 (hardware SPI, not configurable)"));
    Serial.println(F("  - 3.3 V supply (not 5 V), 10 uF capacitor across VCC/GND"));
    Serial.println(F("  - CE/CSN wiring; add your pair to CE_CSN_PINS if non-standard"));
    while (1) {}
  }

  radio.setChannel(RF_CHANNEL);
  radio.setDataRate(RF_DATA_RATE);
  radio.setPALevel(RF24_PA_LOW);
  radio.setCRCLength(RF24_CRC_16);      // tool uses 2-byte CRC
  radio.setAddressWidth(sizeof(address));
  radio.disableDynamicPayloads();
  radio.setPayloadSize(PAYLOAD_LENGTH);
  radio.setAutoAck(AUTO_ACK);

  radio.openReadingPipe(1, address);
  radio.startListening();

  printf_begin();
  radio.printDetails();                 // register dump, handy for comparing with the tool
}

void loop()
{
  if (!radio.available()) {
    return;
  }

  uint8_t payload[PAYLOAD_LENGTH];
  radio.read(payload, sizeof(payload));
  received++;

  Serial.print(F("RX #"));
  Serial.print(received);
  Serial.print(F(": "));
  for (uint8_t i = 0; i < sizeof(payload); i++) {
    if (payload[i] < 0x10) Serial.print('0');
    Serial.print(payload[i], HEX);
    Serial.print(' ');
  }
  Serial.print(F(" |"));
  for (uint8_t i = 0; i < sizeof(payload); i++) {
    char c = (char)payload[i];
    Serial.print((c >= 32 && c < 127) ? c : '.');
  }
  Serial.println('|');
}

#include <M5StickCPlus.h>
#include <RCSwitch.h>

struct RFSignal {
  unsigned int pulseLength;
  unsigned int protocol;
  unsigned int bitLength;
  unsigned long message;
};

RFSignal received = {0,0,0,0};

RCSwitch mySwitch = RCSwitch();

#define RX_PIN 33 // HY2.0 PIN IN
#define TX_PIN 32 // HY2.0 PIN OUT

enum MenuOption {
  SEND,
  RECEIVE
};

bool inMenu = true;
bool printedRF = true;


MenuOption selectedOption = SEND; // default Option

void setup() {
  M5.begin();

  mySwitch.setPulseLength(488);
  mySwitch.setProtocol(6);
  mySwitch.enableTransmit(TX_PIN);
  mySwitch.enableReceive(RX_PIN);

  M5.Lcd.setRotation(1);
  M5.Lcd.fillScreen(TFT_BLACK);
  displayMenu();
}

void loop() {
  M5.update();

  if (M5.BtnB.wasReleased()) {
    inMenu = true;
    changeSelection();
    displayMenu();
    delay(300); // Delay to avoid over selection
  }

  if (inMenu && M5.BtnA.wasReleased()) {
    inMenu = false;
    changeOption(selectedOption);
  }

  if(!inMenu){
    switch(selectedOption){
      case RECEIVE:
        if (mySwitch.available()) {
          getRFSignal();
          printedRF = false;
        }
        if(!printedRF && received.message > 0){
          printRFSignal();
          printedRF = true;
        }
        break;
      case SEND:
        if (M5.BtnA.wasReleased() && received.message > 0){
          sendRFSignal();
        }
    }
  }
}

void displayMenu() {
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setCursor(20, 20);
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("Select:");
  M5.Lcd.print(selectedOption == SEND ? "> " : "  ");
  M5.Lcd.println("A. Send");
  M5.Lcd.print(selectedOption == RECEIVE ? "> " : "  ");
  M5.Lcd.println("B. Receive");
}

void changeSelection() {
  selectedOption = (selectedOption == SEND) ? RECEIVE : SEND;
}

void getRFSignal() {
  received.pulseLength = mySwitch.getReceivedDelay();
  received.bitLength = mySwitch.getReceivedBitlength();
  received.protocol = mySwitch.getReceivedProtocol();
  received.message = mySwitch.getReceivedValue();

  mySwitch.resetAvailable();
}

void printRFSignal() {
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setCursor(20, 20);
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.setTextSize(2);

  if(selectedOption == RECEIVE){
    M5.Lcd.println("Received: ");
  }else{
    M5.Lcd.println("Sending: ");
  }
  
  M5.Lcd.print( received.message );
  M5.Lcd.print(" / ");
  M5.Lcd.print( received.bitLength );
  M5.Lcd.print("bit ");
  M5.Lcd.print("Protocol: ");
  M5.Lcd.println( received.protocol );
  M5.Lcd.print("Delay: ");
  M5.Lcd.println( received.pulseLength );
}

void sendRFSignal() {
  mySwitch.setPulseLength(received.pulseLength);
  mySwitch.setProtocol(received.protocol);
  mySwitch.send(received.message, received.bitLength);
  printRFSignal();
}

void changeOption(MenuOption option) {
  switch (option) {
    case SEND:
      mySwitch.disableReceive();
      mySwitch.enableTransmit(TX_PIN);

      M5.Lcd.fillScreen(TFT_BLACK);
      M5.Lcd.setCursor(20, 20);
      M5.Lcd.setTextColor(TFT_WHITE);
      M5.Lcd.setTextSize(2);
      M5.Lcd.println("Press BtnA to send...");
      delay(2000);
      break;
    case RECEIVE:
      mySwitch.disableTransmit();
      mySwitch.enableReceive(RX_PIN);

      M5.Lcd.fillScreen(TFT_BLACK);
      M5.Lcd.setCursor(20, 20);
      M5.Lcd.setTextColor(TFT_WHITE);
      M5.Lcd.setTextSize(2);
      M5.Lcd.println("Receiving...");
      delay(2000);
      break;
  }
}

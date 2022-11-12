//for endoscope side
//atuto report = on


#include <BleGamepad.h>
#include <TinyPICO.h>
BleGamepad bleGamepad("endocope_side", "lemmingDev", 100);
//CB-27-28-1A-F6-D0 us
//98-CD-AC-D3-58-1A camera
const int Button1 = 5;
const int Button2 = 21;
const int Button3 = 14;
const int Button4 = 15;
int previous;
TinyPICO tp = TinyPICO();

uint8_t newMACAddress[] = {0x98, 0xCD, 0xAC, 0xD3, 0x58, 0x1B - 0x02}; //0X1B -> originaly 1A but not working


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(2000);
  //GPIO set up
  pinMode(Button1,INPUT_PULLUP);
  pinMode(Button2,INPUT_PULLUP);
  pinMode(Button3,INPUT_PULLUP);
  pinMode(Button4,INPUT_PULLUP);
  //---
  BleGamepadConfiguration bleGamepadConfig;
  bleGamepadConfig.setVid(0xe502);
  bleGamepadConfig.setPid(0xBBAB);

  bleGamepadConfig.setAutoReport(true);
  bleGamepad.begin(&bleGamepadConfig);
  //bleGamepad.begin();
  esp_base_mac_addr_set(&newMACAddress[0]);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (bleGamepad.isConnected())
    {
      tp.DotStar_SetPixelColor( 0xFFC900 );
      int current = 0;
      if(digitalRead(Button1) == LOW) {
        bleGamepad.press(BUTTON_1);
        current += 1;
      }else{
        bleGamepad.release(BUTTON_1);
      }
      if(digitalRead(Button2) == LOW) {
        bleGamepad.press(BUTTON_2);
        current += 2;
      }else{
        bleGamepad.release(BUTTON_2);
      }
      if(digitalRead(Button3) == LOW) {
        bleGamepad.press(BUTTON_3);
        current += 4;
      }else{
        bleGamepad.release(BUTTON_3);
      }
      if(digitalRead(Button4) == LOW) {
        bleGamepad.press(BUTTON_4);
        current += 8;
      }else{
        bleGamepad.release(BUTTON_4);
      }
    /*
      if ( current != previous){
        bleGamepad.sendReport();
      }
      */
      delay(20);
      previous = current;
    }else{
      tp.DotStar_CycleColor(25);
    }
    
}

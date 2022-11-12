#include <BleGamepad.h>
#include <TinyPICO.h>
BleGamepad bleGamepad("ultrasond", "lemmingDev", 100);
//BleGamepad bleGamepad("ultrasond", ?, バッテリーに関連したものらしい);

const int Button1 = 5;
const int Button2 = 21;
const int Button3 = 14;
const int Button4 = 15;
int previous;
TinyPICO tp = TinyPICO();

VID PIDアドレスを変更する場合は一度接続したことがmacアドレスだとなぜか認識しない。あるいはオリジナルなものだとダメらしい。
//CB-27-28-1A-F6-D0 us
//98-CD-AC-D3-58-1A camera
がそれぞれがオリジナルなtinyPicoのアドレス
シリアルナンバーなども設定可能


uint8_t newMACAddress[] = {0xCB, 0x27, 0x28, 0x1A, 0xf6, 0xD1 - 0x02};


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
  bleGamepadConfig.setPid(0xabcd);

  bleGamepadConfig.setAutoReport(false);　スイッチが押された時のみ作動する設定。
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
    
      if ( current != previous){
        bleGamepad.sendReport();
      }
      delay(20);
      previous = current;
    }else{
      tp.DotStar_CycleColor(25);
    }
    
}

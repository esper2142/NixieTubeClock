/* Basic sample code for nixie-tube module for Arduino
 *
 * licensed under Attribution-NonCommercial 3.0 Unported (CC BY-NC 3.0)
 * http://creativecommons.org/licenses/by-nc/3.0/
 *
 * Hardware Designer: Yan Zeyuan
 * Blog: http://nixieclock.org
 * Email: yanzeyuan@163.com
 *
 * Library Author: Weihong Guan (aGuegu)
 * Blog: http://aguegu.net
 * Email: weihong.guan@gmail.com
 *
 * Sublety tuning of the parameters and modification to work with 
 * a four Nixie setup by Bob Edmiston
 *
 * Pin Layout, right-side view, input from pins(male)
 *
 *              /-------------\
 *              |             |
 *              |             |
 *              |             |
 *              \-------------/
 *               |           |
 * -----------------------------
 * | SH  | ST  | DS  |
 * | 5V  | GND | OE  |
 * -------------------
 * | 12V | 12V | 12V |
 * | GND | GND | GND |
 * -----------------------------
 */
     //  DIN(D11) - PIN1
    //  ST(D12)  - PIN2
    //  SH(D13)  - PIN3
    //  OE(D10)  - PIN4
    //  GND      - PIN5
    //  5v       - PIN6  
    //  The pin number of the module is defined in the datasheet
//D10-OE Output Enable brightness
//D11-DS serial data input
//D12-ST DTorage register clock input
//D13-SH SHift register clock input

//PIN SYMBOL DESCRIPTION
//INPUT (on the RIGHT side, plugs)
//1 DIN DS, serial data input
//2 STCP ST, storage register clock input
//3 SHCP SH, shift register clock input
//4 OE OE, output enable (active LOW), brightness control
//5 GND ground (0V)
//6 5V out 5V power out (300mA Maximum)
//7‐9 12V in 12V power in
//10‐12 GND ground (0V)

//OUTPUT (on the LEFT side, sockets)
//1 DOUT DOUT, serial data output
//2 STCP ST, storage register clock output
//3 SHCP SH, shift register clock output
//4 OE OE, output enable (active LOW), brightness control
//5 GND ground (0V)
//6 N/A
//7‐9 12V out 12V power out
//10‐12 GND ground (0V)

#include "NixieTube.h"
#include <RTClib.h>

#include <Wire.h>
//#include <DS1307.h>


RTC_DS1307 rtc;
#define COUNT	4
// define how many modules in serial

NixieTube tube(11, 12, 13, 10, COUNT); // NixieTube(DIN,ST,SH,OE,NUM)
// pin_ds, pin_st, pin_sh, pin_oe(pwm pin is preferred), COUNT

  
  static int dispVal = 0;
  static boolean dir = true;
  static byte brightness = 0;
  static int colorNum = 0;    // the number of color in array of the library 
  unsigned long backgroundCtrl_Timer = 0;  // the timer for managing backlight
  unsigned long numCtrl_Timer = 0;         // the timer for managing number displayed
  
void setup()
{
  Serial.begin(9600);
    Wire.begin();
  rtc.begin();  //Tiny RTC module  http://www.hobbyist.co.nz/?q=real_time_clock SDA-A4, SCL-A5
    if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(__DATE__, __TIME__));
  }
//  if(rtc[6]<12){
//    RTC.stop();
//    RTC.set(DS1307_SEC,1);
//    RTC.set(DS1307_MIN,27);
//    RTC.set(DS1307_HR,01);
//    RTC.set(DS1307_DOW,7);
//    RTC.set(DS1307_DATE,12);
//    RTC.set(DS1307_MTH,2);
//    RTC.set(DS1307_YR,12);
//    RTC.start();
//  }
	for(byte i=0; i<COUNT; i++)
	{
		tube.setBackgroundColor(i, (Color)(i%8));	// set different background color for each module
		tube.setColon(i, (Colon)(i%4));
		tube.setNumber(i, i);
	}

	tube.setBrightness(0xff);	// brightness control, 0x00(off)-0xff
	tube.display();
}

void loop()
{
	static byte h = 0, m = 0, s = 0, cs = 0;
  //unsigned int hour;
        DateTime now = rtc.now();
        //hour = now.hour;
            Serial.print(now.hour(), DEC);
            Serial.print(":");
            Serial.println(now.minute(), DEC);
            
	tube.printf("%02d:%02d%", now.hour()%12, now.minute());

	tube.display();
	delay(10);
	cs++;
	if (cs == 100) {cs=0; s++;}
	if (s == 60) {s=0; m++;}
	if (m == 60) {m=0; h++;}
	if (h == 12) {h=0;}

  if(millis() - backgroundCtrl_Timer > 40){ // Manage the backlight controlling flash rate
    
    if(dir)  tube.setBrightness(++brightness);    // Set the brightness of the background
    else tube.setBrightness(--brightness);
    
    if(brightness > 250)  dir = false;
    else if(brightness < 50)  dir = true,colorNum++;
    
    if(colorNum > 7)  colorNum = 0;
    tube.setBackgroundColor((Color)colorNum); //set the background color. Call the element from the color array
    backgroundCtrl_Timer = millis();
  }

}

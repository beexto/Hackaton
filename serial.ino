#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
SoftwareSerial hc06(12,13);
int n;
float y;
float x;
String s="a";
String s1="b";
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);


/** Fonction setup() */
void setup() {
 
 lcd.begin(16, 2);
 lcd.print("Hello World !");
 hc06.begin(9600);

}
//=============================
void loop() 
{

     if(hc06.available() > 0)
    {
     n=hc06.parseInt();
      if (n==0)
        s = hc06.readStringUntil('#');
      if (n==1)
      s1 = hc06.readStringUntil('#');
      
        x = hc06.parseFloat();
        y= hc06.parseFloat();
    }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(s);
    lcd.print(x);
    lcd.print("|");
    lcd.print(y);
    lcd.print(s1);
    delay(700);
}


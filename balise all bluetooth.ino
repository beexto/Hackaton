#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
/** Énumération of boutons  */
enum {
  BUTTON_NONE,  
  BUTTON_UP,   
  BUTTON_DOWN,  
  BUTTON_LEFT, 
  BUTTON_RIGHT,
  BUTTON_SELECT 
};
// enumeration of menus, each is for a complete 16*2 screen(navigation is right/left)
enum {
  MENU_coord=1,  
  MENU_desc=2,
  MENU_news=3,
  MENU_owner=4
};
//enumeration of bluetooth comm parameters
enum {
  B_name=1, //name of the beacon
  B_coord=0, //x= latitude y= longitude
  B_desc=2,//description of the beacon
  B_news=3,
  B_owner=4//news: everybody can write on this part
};
/* bluetooth protocol 
exemple : PSWD,n,x,y for B_coord 
          PSWD,nowner for B_owner   */

/*voltage divider bridge R1 20k in serie with R2 10k*/
#define TENSION_MAX 860 //analogRead() 12.3V ==> 4.1 after voltage divider bridge
#define TENSION_MIN 839 //analogRead() 12.6V ==> 4.2 
#define PIN_BATT A5
#define PSWD 42
//struct of coordinates
typedef struct Coordonnees Coordonnees;
struct Coordonnees
{
    float x;//latitude
    float y;//longitude
};
Coordonnees pos;
//============================== battery pixels
byte batterie_0[8]={
  B01110,
  B11111,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B11111
};

byte batterie_20[8]={
  B01110,
  B11111,
  B10001,
  B10001,
  B10001,
  B10001,
  B11111,
  B11111
};

byte batterie_40[8]={
  B01110,
  B11111,
  B10001,
  B10001,
  B10001,
  B11111,
  B11111,
  B11111
};

byte batterie_60[8]={
  B01110,
  B11111,
  B10001,
  B10001,
  B11111,
  B11111,
  B11111,
  B11111
};

byte batterie_80[8]={
  B01110,
  B11111,
  B10001,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

byte batterie_100[8]={
  B01110,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};
//==============================up arrow pixels
byte point_haut[8]={
  B00100,
  B01110,
  B11111,
  B00100,
  B00100,
  B00100,
  B00000,
  B00000
};
//============================================string to get info frome bluetooth serial and print on lcd
String description="description";
String news="news";
String name="name";
String owner="WHO'S THE OWNER?";
String trash="";// to store the excess info from serial if pswd!=PSWD to write on "owner"
/** Objet LiquidCrystal pour communication avec l'écran LCD */
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
//comm SoftwareSerial
SoftwareSerial hc06(12,13);//TX-RX
//================================================== 
int lum=255;
char menu=1;
double t2=0;
double t1=0;
//=============================================initialize new pixels, lcd and serial over hc06
void setup() {
  lcd.begin(16, 2);
  lcd.createChar(0,batterie_0);
  lcd.createChar(1,batterie_20);
  lcd.createChar(2,batterie_40);
  lcd.createChar(3,batterie_60);
  lcd.createChar(4,batterie_80);
  lcd.createChar(5,batterie_100);
  lcd.createChar(6,point_haut);
  hc06.begin(9600);
  owner.reserve(40);
  name.reserve(40);
  description.reserve(40);
  news.reserve(40);
  trash.reserve(40);
  pinMode(10,OUTPUT);
  analogWrite(10,lum);
}


void loop() {
t1=millis();
comm_hc06();
affichage();
bouton();
loop_variable();
brightness();
delay(250);
}
//=============================================
void comm_hc06()
{
  if(hc06.available() > 0)
  {
    int pswd=hc06.parseInt();
    int n=hc06.parseInt();
    switch (n)
    {
      case B_name:
        name=hc06.readString();
      break;

      case B_desc:
        description=hc06.readString();
      break;

      case B_news:
        news=hc06.readString();
      break;

      case B_owner:
      if(pswd!=PSWD)
        trash=hc06.readString();
        else
        {
          owner=hc06.readString();
        }
        
      break;

      case B_coord:
        pos.x = hc06.parseFloat();
        pos.y= hc06.parseFloat();
      break ;
    }     
  } 
}
//=============================================
byte getPressedButton()
{
  int value = analogRead(A0);// read the button state 

  if (value < 99)
    return BUTTON_RIGHT;
  else if (value < 255)
    return BUTTON_UP;
  else if (value < 409)
    return BUTTON_DOWN;
  else if (value < 639)
    return BUTTON_LEFT;
  else if (value < 1010)
    return BUTTON_SELECT;
  else
    return BUTTON_NONE;
}
//=============================================
void bouton()
{
    switch(getPressedButton())
    {
        case BUTTON_UP :
        t2=millis();
        lum+=51;
        break;

        case BUTTON_DOWN :
        t2=millis();
        lum-=51;
        break ;

        case BUTTON_LEFT :
          t2=millis();
          if (select==0)
            menu--;
        break ;

        case BUTTON_RIGHT :
          t2=millis();
        break;

        case BUTTON_SELECT :
          t2=millis();       
        break ;
    }
}
//=============================================
void loop_variable()
{
  //loop between menus
  if (menu<MENU_coord)
    menu=MENU_owner;  

  if (menu>MENU_owner)
    menu=MENU_coord;
  //loop between possible value of latitude=x {-80;80}
  if (pos.x>80) 
    pos.x=-80;
  
  if (pos.x<-80) 
    pos.x=80;
  //loop between possible value of longitude=y {-160;160}
  if (pos.y>160) 
    pos.y=-160;
  
  if (pos.y<-160) 
    pos.y=160;
  
  
}
//=============================================
void affichage()
{
  lcd.clear();
  switch(menu)
  {
    case MENU_coord :
      affichage_coord();
      affichage_batterie();
      affichage_name();   
    break ;

    case MENU_desc :
      affichage_description();
    break ;

    case MENU_news :
      affichage_news();
    break ;
    
    case MENU_owner :
      affichage_owner();
    break;
      
  }
}
//=============================================
void affichage_batterie()
{
  lcd.setCursor(15,0);
  int segment_batt=lecture_battery();
  if (batt<1)
  {
  lcd.write((byte)5);
  }
  else if (batt<2)
  {
  lcd.write((byte)4);
  }
  else if (batt<3)
  {
  lcd.write((byte)3);
  }
  else if (batt<4)
  {
  lcd.write((byte)2);
  }
  else if (batt<5)
  {
    lcd.write((byte)1);
  }
  else if(batt>=5)
  {
    lcd.write((byte)0);
  }
}
//=============================================reads an analogRead on a voltage dividing bridge
int lecture_battery()
{
  int tension =analogRead(PIN_BATT);
  tension = TENSION_MAX-tension;
  int segment_batt= tension/4.2;
  return segment_batt;      //return the number {0;5} of segment in the battery
}

//==================================display x and y coordinates
void affichage_coord()
{
  lcd.setCursor(0,0);
  lcd.print("x:");
  if (pos.x<10 && pos.x>=0)
  lcd.print("0");
  lcd.print(pos.x);

  lcd.setCursor(7,0);
  lcd.print("y:");
  if (pos.y<10 && pos.y>=0)
  lcd.print("0");
  if (pos.y<100 && pos.y>=0)
  lcd.print("0");
  lcd.print(pos.y);
}
//=============================================displays a description of the beacon(whats near etc)
void affichage_description()
{
  lcd.setCursor(0,0);
  if (description.length()>15)
  {
    for(int i=0;i<16;i++)
    {
      lcd.print(description[i]);
    }
    lcd.setCursor(0,1);
    for(int i=16;i<description.length();i++)
    {
      lcd.print(description[i]);
    }
  }
  else
    lcd.print(description);
}
//=============================================displays local news ( writabe by everyone )
void affichage_news()
{
  lcd.setCursor(0,0);
  if (news.length()>15)
  {
    for(int i=0;i<16;i++)
    {
    lcd.print(news[i]);
    }
    lcd.setCursor(0,1);
    for(int i=16;i<news.length();i++)
    {
    lcd.print(news[i]);
    }
  }
  else
    lcd.print(news);
}
//============================================= displays the beacon's name(locked to the owner)
void affichage_name()
{
  lcd.setCursor(0,1);
  if (name.length()>15)
  {
    for(int i=0;i<name.length();i++)
    {
    lcd.print(name[i]);
    }
  }
  else
  {
    lcd.print(name);
  }
}
//====================================== show the identity of the beacon's placer (locked to the owner)
void affichage_owner()
{
  lcd.setCursor(0,0);
  if (owner.length()>15)
  {
    for(int i=0;i<16;i++)
    {
      lcd.print(owner[i]);
    }
    lcd.setCursor(0,1);
    for(int i=16;i<owner.length();i++)
    {
      lcd.print(owner[i]);
    }
  }
  else
  {
    lcd.print(owner);
  }
}
//=========================================
void brightness()
{
  analogWrite(10,lum);
  if ((t1-t2)>3600000)
    analogWrite(10,15);
}

#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#define I2C_ADDRESS 0x3C

#include <Keypad.h>

#include <Stepper.h>
#define STEPS 200
#define motorInterfaceType 1
Stepper stepper(STEPS, 12, 10); //stepperPins(Anz. Steps, Direction, Step)

SSD1306AsciiWire oled;

int maxhoehe = 88;
int hoehemotor = 0;
int differenz = 0;
int endeUnten = digitalRead(11); // pin schalter oben
int endeOben = digitalRead(5); // pin schalter unten



const int ROW_NUM = 3; //Reihen Keypad
const int COLUMN_NUM = 4; //Spalten Keypad
char keys[3][4] = 
{
  {'1','2','3','4'},
  {'5','6','7','8'},
  {'9','0','O', 'R'}  
};//O=Ok, R=Reset(auf null)

byte pin_rows[ROW_NUM] = {2, 3, 4}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {9, 8, 7, 6};

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

char hoehe[2] = {'0','0'};//höhe in mm
String zeiger = "      ^ ";
int pos = 0;

void anzeige()
{
      oled.clear(); //Löschen der aktuellen Displayanzeige
      oled.print("soll: ");
      oled.print(hoehe[0]);
      oled.print(hoehe[1]);
      oled.print(" ");
      oled.println("mm");
      oled.println(zeiger);
      oled.print("ist:  ");
      oled.print(hoehemotor);
      oled.print(" ");
      oled.println("mm");
}

int lift(int steps)
{
  stepper.step(steps);
 // return way;
}

void kalibrierung()
{
  oled.clear(); //Löschen der aktuellen Displayanzeige
  oled.println("Kalibrieren");
  oled.print("laeuft");
  //lift(1);

  
  hoehemotor = 0; 
  oled.clear(); //Löschen der aktuellen Displayanzeige
  oled.println("Kalibrieren");
  oled.print("fertig");
  delay(1000);
  anzeige();
}

void setup() 
{
  pinMode(endeUnten, INPUT);
  pinMode(endeOben, INPUT);
  Serial.begin(9600);//serial für debugging
  stepper.setSpeed(500);
  Wire.begin();//oled
  Wire.setClock(400000L);//oled
  oled.begin(&Adafruit128x64, I2C_ADDRESS);//oled
  oled.setFont(fixed_bold10x15); // Auswahl der Schriftart
  anzeige();
  kalibrierung();
}


int motorDrehen(int hoehealt, int hoeheneu) //hoehemotor = 90, eingabe = 30  
{  
    differenz = hoehealt - hoeheneu;  
    differenz *= 100;
    while (stepper.step(differenz))
    {
      hoehemotor -= differenz / 100;
      Serial.print(differenz);
      Serial.println(hoehemotor);
    }

    anzeige();
}

void loop() 
{
  char key = keypad.getKey();
  unsigned long currentMillis = millis();

  if (key)
  {
    if(pos > 1)
    {
      pos = 0;
    }

    if (key == 'R')
    {
      hoehe[0] = '0';
      hoehe[1] = '0';
      zeiger = "      ^ ";
      pos = 0;
    }
    else if (key == 'O')
    {
      if (hoehemotor != atoi(hoehe))
      {
        zeiger = "      ^ ";
        pos = 0;
        if (atoi(hoehe) > maxhoehe)
        {
          atoi(hoehe) == hoehemotor;
          oled.clear(); //Löschen der aktuellen Displayanzeige
          oled.println("Hoehe ist");
          oled.print("ungueltig");
          delay(1000);
          anzeige();
        }
        if (atoi(hoehe) != 0 && atoi(hoehe) <= maxhoehe)
        {
          motorDrehen(hoehemotor, atoi(hoehe));
        }
        if (atoi(hoehe) == 0)
        {
         kalibrierung();
        }
      }
    }
    else
    {
      hoehe[pos] = key;
      switch (pos) 
      {
        case 1:
          zeiger = "      ^ ";
          break;
        case 0:
          zeiger = "       ^";
          break;
      }
      pos++;
    } 
  anzeige();
  }
}

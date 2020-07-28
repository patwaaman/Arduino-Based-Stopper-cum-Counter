#include <Wire.h>   
#include <LiquidCrystal.h>
#include <EEPROM.h>

const int reedswitch = 4;
const int ledpin = 12;
const int ok_button = 2;

//LCD pin to Arduino
const int pin_RS = 8; 
const int pin_EN = 9; 
const int pin_d4 = 4; 
const int pin_d5 = 5; 
const int pin_d6 = 6; 
const int pin_d7 = 7; 

const int pin_BL = 10;  // use pin for lcd backlight percentage change .

LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);

// Creates 1 custom characters for the menu display
byte questionMark[8] = {
  0b00100, 
  0b01010, 
  0b10001,
  0b00010, 
  0b00100, 
  0b00100, 
  0b00000, 
  0b00100  
};

int stopper[100] ={0};     //Temporary array to store users input.
int no = 1;
int activeButton =1023 ;

#define RIGHT 0
#define UP 1
#define DOWN 2
#define LEFT 3
#define SELECT 4


// This function is called whenever a button press is evaluated. The LCD shield works by observing a voltage drop across the buttons all hooked up to A0.
int evaluateButton(int x) {
   int res;
  if (x < 5) 
      res = 0 ;       // right button pressed.
  else if (x < 150) 
      res = 1;        // up button pressed
  else if (x < 300)
      res = 2;        // down button pressed.
  else if (x < 500)
      res = 3;        // left button is pressed. 
  else if (x < 700)   
      res= 4;         // select button is pressed.
  return res;
} 

int takeInput(int activeButton)
{     
    int temp = activeButton ;
         while (temp>650)
       {
          temp=analogRead(0);
          if(activeButton<650)
               break;
       }
     goloop();
       return evaluateButton(temp);
}


 void goloop()
 {
    int go =0 ;
    while(go <650)
      go = analogRead(0);
 }

void assign_stopper()
{
        int key = 0 ;                                   Serial.println(" KEY MATCHED ");
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("ENTER STOPPER NO");     Serial.println("ENTER STOPPER NO");
        lcd.setCursor(0,1);
        lcd.print("SIZE ");Serial.println("SIZE  ");
        lcd.print(no);

        while(true)
        {                                                                                       Serial.println("Gone to while loop");
          
           key = takeInput(activeButton);
             if(key == UP)
               {
                    no++;
                    lcd.setCursor(5,1);
                    lcd.print(no) ;
                    Serial.println(no);
               }
             else if( key == DOWN)
                    {
                      no--;
                      lcd.setCursor(5,1);
                    lcd.print(no) ;
                   Serial.println(no);
                    }
             else if( key == SELECT)
                  {
                      lcd.setCursor(5,1);
                      lcd.print("OK");
                      Serial.println(" OK ");
                      break;                
                  }
 
        }
        EEPROM.update(0,no);
        Serial.print("EEPROM VALUE AT ADDRESS 0  :  ");
        Serial.println(EEPROM.read(0));
        lcd.clear();
        Serial.println("OUT OF SIZE LOOP");
        Serial.println(" ENTERING TO FOR LOOP");
        int address = 2 , eeprom_data;
        for(int i=0; i< no ;i++)
        {   
            lcd.clear();
            lcd.print("ENTER VALUE ");     Serial.print("ENTER VALUE ");
            lcd.print(i+1) ;               Serial.println(i+1);
            lcd.setCursor(0,1);
            
             while(true)
               {   
                  key = takeInput(activeButton);
                    if(key == UP)
                        {
                             stopper[i]+= 10;
                             lcd.setCursor(0,1);
                             lcd.print(stopper[i]) ;Serial.println(stopper[i]);
                         }
                    else if( key == DOWN)
                         {
                               stopper[i]-=10;
                               lcd.setCursor(0,1);
                               lcd.print(stopper[i]) ;Serial.println(stopper[i]);
                         }
                    else if( key == RIGHT)
                         {
                               stopper[i]++;
                               lcd.setCursor(0,1);
                              lcd.print(stopper[i]) ;Serial.println(stopper[i]);
                         }
                    else if( key == LEFT)
                         {
                               stopper[i]--;
                               lcd.setCursor(0,1);
                              lcd.print(stopper[i]) ;Serial.println(stopper[i]);
                         }
                         
                     else if( key == SELECT)
                         {    
                          lcd.setCursor(0,1);
                              Serial.println(" OK ");
                              
                              EEPROM.put(address , stopper[i]);
                              EEPROM.get(address,eeprom_data);
                              Serial.print("eeprom_data  :  ");
                              Serial.println(eeprom_data);
                                address += sizeof(int);
                              stopper[i+1] =stopper[i] ;
                               break;
                          }

                 }
          }    
}


void decrementer()
{
        int i=0 , j=0 , key = 0;
        lcd.clear();
        lcd.print("DEC");
        lcd.setCursor(13,0);
        lcd.print(EEPROM.read(0));
        lcd.setCursor(0,1);          
        lcd.print( "STOPPER END");
        //stopper's index wise element decrementer .
        key = takeInput(activeButton);
        if(key==RIGHT )
         { 
            Serial.println("AFTER ASSIGN STOPPER");
            assign_stopper();
         }
        else if(key == SELECT )
         {   
             for( ; i< EEPROM.read(0) ;i++)
             {      
                       lcd.setCursor( 5, 0);
                       lcd.print(i+1);
                       lcd.setCursor(0,1);
                       lcd.print("                ");
             
                       for( ; j<stopper[i] ; )
                       {  
                           lcd.setCursor(0,1);
                           int buttonstate = digitalRead(reedswitch);
                              if(buttonstate == LOW )
                              {
                                  j++ ;
                                  lcd.print(j);
                                  Serial.println(j);
                                  delay(500);
                              }
                        }                           
                           lcd.setCursor(0,1);
                           digitalWrite(ledpin, HIGH);          
                           lcd.print( "stopped");
                           j=stopper[i] ;
                           key = takeInput(activeButton);
                           if(key == SELECT)
                           {
                               digitalWrite(ledpin , LOW);
                           }
                           
                        }                   
          }                  
}

void setup()
{
  Serial.begin(9600);
  // Initializes and clears the LCD screen
  lcd.begin(16 , 2);
  lcd.setCursor(0, 0); 
  pinMode(reedswitch ,INPUT);
  //pinMode(ok_button, INPUT);
  pinMode(ledpin , OUTPUT);
  lcd.print("MANOJ ELECTRONIC");                               Serial.println("MANOJ ELECTRONIC");// splash screen
  lcd.setCursor(0, 1); 
  lcd.print("   COUNTER");                                     Serial.println("counter");// splash screen
  delay (3000); // pause 3 seconds
  lcd.clear();

  // Creates the byte for the 1 custom characters
  lcd.createChar(0, questionMark);
}

void loop()
{
       // assign_stopper();
        //Serial.println("AFTER ASSIGN STOPPER");
        //function to assign values to stopper array.
        decrementer();
        Serial.println("");        
}

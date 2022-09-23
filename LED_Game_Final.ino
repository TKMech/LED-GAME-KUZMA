//Accelerometer Code
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

// Used for software SPI
#define LIS3DH_CLK 13
#define LIS3DH_MISO 9//SDO 15
#define LIS3DH_MOSI 10
// Used for hardware & software SPI
#define LIS3DH_CS 8
// hardware SPI
//Adafruit_LIS3DH lis = Adafruit_LIS3DH(LIS3DH_CS);

// software SPI
Adafruit_LIS3DH lis = Adafruit_LIS3DH(LIS3DH_CS, LIS3DH_MOSI, LIS3DH_MISO, LIS3DH_CLK);


//Shift Register 
int latchPin = A4; //pin #12
int clockPin = A5; //pin #11
int dataPin = 6; //pin #14

byte leds = 0;
bool fortnite = false;
int ledlocations[] = {2, 7, 3, 4, 5, 6, 0, 1};

//RandN - LED CONVENTIONS 
//0 - 2 = Top Left
//1 - 7 = Top Middle
//2 - 3 = Top Right
//3 - 4 = Middle Right 
//4 - 5 = Bottom Right 
//5 - 6 = Bottom Middle
//6 - 0 = Bottom Left
//7 - 1 = Middle Left 

//Buttons 
int buttonState = 0; 
int restartButton = 3; 
int mainButton = 2; 

//Random Number 
int RandomN = 0;

//Conditionals 
int a = 1; 
int i = 0; 
int y = 20; 

//Joystick Constants
int VRx = A0;
int VRy = A1;
int SW = 0;

int xPosition = 0;
int yPosition = 0;
int SW_state = 0;
int mapX = 0;
int mapY = 0;

// LCD setup
#include <LiquidCrystal.h>
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 7, d7 = A2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//Score Keeping 
int player1 = 0;
int player2 = 0;
int highscore = 0;
int winner = 0;
int highscore_check = 0; 

//Game Conditionals


//Time 
float winningTime; 
float Tdiff; 
unsigned long startTime;
unsigned long endTime;


//Interupt Constants
volatile bool state_interrupt = false;  //Used in Interrupt
volatile long time_interrupt = 0; //Used in Debounce
int standardDelay = 200; //Used in debounce.
int On = 0; //1 = On, 0 = Off. Used in Button Argument.

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() 
{

  //LCD Initialize 
  lcd.begin(16, 2);
  Serial.begin(9600); 

  //Interrupt Initialize 
  attachInterrupt(digitalPinToInterrupt(mainButton), gameStart, FALLING);

  attachInterrupt(digitalPinToInterrupt(restartButton), gameReset, FALLING);
  
  
  //Shift Register Initialize
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);

  //Joystick Initialize 
  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(SW, INPUT_PULLUP); 
  
  //Button Pin 
  pinMode(2, INPUT);
  pinMode(3, INPUT); 

  //Begin Serial Communication 

if (! lis.begin(0x15)) {   // change this to 0x19 for alternative i2c address
    //Serial.println("Couldnt start");
    while (1) yield();

     lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!

  //Serial.print("Range = "); Serial.print(2 << lis.getRange());
  //Serial.println("G");

   lis.setDataRate(LIS3DH_DATARATE_50_HZ);
  //
  
}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop()
{

   leds = 0; 
    updateShiftRegister();
 if (On == 1) 
 {
   
  updateLCD();
  gameLoop();
  On = 0; 
 }

}
  
  


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void gameLoop()
{
 
   countdownLED(); 
   delay(10);
    startTime = millis(); 
   lightLED(); 
  while (winner == 0)
  { 
    pollAcell();
    pollJoystick();
   
  }

  if (winner == 1)
  //player 1 wins 
{
  player1 = player1 + 1; 
}
  else if(winner == 2)
  //player 2 wins
  {
    player2 = player2 + 1;
  }
  Tdiff = endTime - startTime; 
  winningTime = ((Tdiff) / 1000.0 );
  highscore_check = max(player1, player2); 
  if(highscore_check >= highscore)
  {
   highscore = highscore_check; 

    updateLCD(); 
      winner = 0;

    y = 8;
    
    
}

}








/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void updateShiftRegister()
{
   digitalWrite(latchPin, LOW);
   shiftOut(dataPin, clockPin, LSBFIRST, leds);
   digitalWrite(latchPin, HIGH);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void updateLCD()
{
  lcd.clear();
   lcd.setCursor(0,0);
  lcd.print("P1:");
  lcd.print(player1);
  lcd.print(" P2:");
  lcd.print(player2);
  lcd.print(" High:");
  lcd.print(highscore);
  lcd.setCursor(0, 1);
    lcd.print("t:");
  lcd.print(winningTime);
  lcd.print("s ");
  if (winner == 0)
  {
  }
  else 
  {
    lcd.print("P");
  lcd.print(winner);
  lcd.print(" wins!");
  }
  
    
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void lightLED() 
{
  RandomN = random(0, 8); 
  //Serial.println(RandomN); 
  if (a == 1)
  {
    //for (i = 0; i<8; i++)
   // {
   // bitClear(leds, ledlocations[i]);
   leds = 0; 
    updateShiftRegister(); 
    
    bitSet(leds, ledlocations[RandomN]);
    updateShiftRegister();
   
    a = 1;  
    delay(100);
    
  }

 
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void pollAcell()
{
lis.read();      
  sensors_event_t event;
  lis.getEvent(&event);

 float x_val = (event.acceleration.x);
 float y_val = (event.acceleration.y);
 float z_val = (event.acceleration.z);

/////Start Conditional, Check for Z movement///////
if (event.acceleration.z < 4)
{
  Serial.println("Oh we movin' huh");
  Serial.print("\t\tX: "); Serial.print(event.acceleration.x);
  Serial.print(" \tY: "); Serial.print(event.acceleration.y);
  Serial.print(" \tZ: "); Serial.print(event.acceleration.z);
  Serial.println(" m/s^2 ");

///////IS it Top?//////////////
  if ( event.acceleration.y < -2 )
  {
  Serial.println("Top");
  
    if(x_val  > 1.5)
    {
    Serial.println("Top Left");
    y = 6; 
   // lcd.print("0");
    }


     else if(-1.5<= x_val && x_val<=1.5)
     {
    Serial.println("Top Middle");
    y = 7; 
     //lcd.print("1");
     }
     
     else if(x_val < -1.5)
     {
    Serial.println("Top Right");
    y = 0; 
    //lcd.print("2");
     }
  }

///////IS it Bottom?//////////////////
    else if(event.acceleration.y > 2)
    {
    Serial.println("Bottom");
    

     if (x_val > 1.5)
     {
    Serial.println("Bottom Left");
    y = 4; 
   // lcd.print("6");
    }
     
     else if(-1.50 <= x_val && x_val <= 1.50)
     {
    Serial.println("Bottom Middle");
    y = 3; 
    //lcd.print("5");
     }
     
     else if(x_val < -1.5)
     {
    Serial.println("Bottom Right");
    y = 2; 
    //lcd.print("4");
     }
    }

///////IS it Middle?//////////////////
//  else if ( -3 < event.acceleration.y && event.acceleration < 3)
else{

    Serial.println("Middle");

     if (x_val > 2)
     {
    Serial.println("Middle Left");
    y = 5; 
   // lcd.print("7");
     }
     
      else if(x_val < -2)
      {
    Serial.println("Middle Right");
    y = 1;
    //lcd.print("3"); 
      }
}
  
 // delay(20000);
}

else{

y = 8; 
//lcd.print("8");
}


 if(y == RandomN)
    {
      //Player 2 Wins// 
      winner = 2; 
      endTime = millis(); 
 
    }

     else if (y == 8)
    {
 //Player Hasn't Moved
    }

    else 
    {
      //PLayer 2 Disqualified
      winner = 1;
      
    }
 
  

  delay(5);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void pollJoystick()
{
    
xPosition = analogRead(VRx);
  yPosition = analogRead(VRy);
  SW_state = digitalRead(SW);
  mapX = map(xPosition, 0, 1023, -512, 512);
  mapY = map(yPosition, 0, 1023, -512, 512);
  if ((-100 < mapX && mapX < 100) && (-100 < mapY && mapY < 100)){
  //lcd.print("8")
  }
  y = 8;
   if ((-512 < mapX && mapX< -300) && (400 < mapY && mapY < 512)){
  //lcd.print("0");
  y = 6; 
  }
  if ((-512 < mapX && mapX< -300) && (-100 < mapY && mapY < 100)){
  //lcd.print("1");
  y = 7; 
  }
  if ((-512 < mapX && mapX< -300) && (-450 < mapY && mapY < -300)){
  //lcd.print("2");
  y = 0;
  }
  if ((-100 < mapX && mapX< 100) && (-512 < mapY && mapY < -300)){
  //lcd.print("3");
  y = 1; 
  }
  if ((400 < mapX && mapX< 512) && (-512 < mapY && mapY < -300)){
  //lcd.print("4");
  y = 2; 
  }
  if ((400 < mapX && mapX< 512) && (-100 < mapY && mapY < 100)){
  //lcd.print("5");
  y = 3; 
  }
  if ((400 < mapX && mapX< 512) && (400 < mapY && mapY < 512)){
  //lcd.print("6");
  y = 4; 
  }
  if ((-100 < mapX && mapX< 100) && (400 < mapY && mapY < 512)){
  //lcd.print("7");
  y = 5; 
  }

  if(y == RandomN)
    {
      //Player 1 Wins// 
      winner = 1; 
      endTime = millis(); 
 
    }

    else if (y == 8)
    {
 //Player Hasn't Moved
    }

    else 
    { 
      //PLayer Disqualified
     winner = 2;
      
    }
    delay(5);
}

  

  
void gameStart() 
{
   if (!state_interrupt) {
    //state_interrupt = true;

    if (millis() - time_interrupt > standardDelay)
    {
      
    
        time_interrupt = millis();
        On = 1; 
        
  
}
   }
}

void gameReset()
{
  /*Serial.println("I'm in the gameReset Ha Ha Ha");
  
   if (!state_interrupt) {
    //state_interrupt = true;

    if (millis() - time_interrupt > standardDelay)
    {
      
    
        time_interrupt = millis();
        */
        
  player1 = 0; 
  player2 = 0; 
  winningTime = 0; 
winner = 0;
   updateLCD(); 

  
  
}
   


void countdownLED()
{
  int j = 0;
  for (j=0; j<3; j++)
  {

     for (i = 0; i<8; i++)
   {
     
   bitSet(leds,i);
}
updateShiftRegister(); 
 delay(500);
   leds = 0; 
    updateShiftRegister(); 
    delay(500);


  }
}
    

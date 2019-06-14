
  /*
  TO DO Remove non annunciator extractions from Experts SimConnects Extractions 
  23 may  19 add dimmer circuit for all LCDs
  21 may  19 separate landing light leds from annunciator leds
  TO DO check breakout board pin names     
  16 may  19 copied from breadboard instrument switch panel

ANNUNCIATOR PANEL FOR THE FSX DEFAULT AIRCRAFT 'MOONEY ORION' USING AN ARDUINO BOARD AND JIM'S LINK2FS INTERFACE PROGRAM.

READ the README.TXT file included in .zip file

My thanks to github.com, the Arduino.cc community, Mark1980 @ My Cockpit.org, and especially to Jim - jimspage.co.nz. 
Without them doing this by other methods would be way beyond me. 

This code is provided in the spirit of Open Source. Please copy and distribute for personal use. Poke holes in my logic
and improve to your hearts content.

BushPilotSimmer
May 2019
*/

//CIRCUIT WIRING
// SWAP '*' with '/' in following line to make group comments easier to read. SWAP back before verifying or downloading.
/*
DRAW UP YOUR OWN circuit diagram - simple circuit - large file on somebody else's website - see YouTube by BushPilotSimmer
All Arduino board pin numbers can be changed. OPTIONAL external power supply and LED dimmer circuit can me moved off
project and used to control all LEDs including LCD display backlight LEDs. Measure twice, solder until satisfied. External
5V power is not required for the demo; however, an Arduino Nano running on USB power only will show a marked voltage drop
as each annunciator LED is illuminated. Total circuit draw including 17 5mm LEDs using 560 ohm current limiting resistors
is about 80 mA.

ARDUINO BOARD to 74HC595 SHIFT REGISTER connect:
- Arduino pin D6 to 74HC595N pin 12 (RCLK)  -- breakout board pin /L_Clock     -- GREEN
- Arduino pin D8 to 74HC595N pin 11 (SRCLK) -- breakout board pin /Clock   -- ORANGE
- Arduino pin D7 to 74HC595N (0) pin 14 (SER in) -- breakout board (0) pin /SER_IN -- WHITE
- Daisy chain 74HC595N #0 pin 11 (QH prime-SER out) to 74HC595N #1 pin 14 (SER in) -- breakout board #0 /Serial_OUT
  to breakout board #1 /Serial_In
- Arduino 5V to 74HC595N VCC and Arduino GND to 74HC595N GND pins respectively
ARDUINO BOARD to other pins connect:
- Arduino pin D2 to push2test pushbutton -- connect other terminal of switch to GND
- Arduino pin D3 to OPTIONAL piezo buzzer +ve terminal -- connect the other termimal to GND 
- Arduino pin D4 to blankAnnunciator pushbutton -- connect other terminal of switch to GND
- Arduino pin D9 to gate pin of OPTIONAL P-channel MOSFET
- Arduino pin A0 (D14) to cathode of the 'MASTER WARNING' LED 
- Arduino pin A5 to wiper of 10k ohm potentiometer
LEDs require current limiting resistor in each LED's circuit (size to balance colours is device dependent) connect:
- all LED anodes to LED power (Arduino 5V or external +5V power supply or DRAIN terminal of OPTIONAL P-channel MOSFET)
- all LED cathodes to 74HC595N or Arduino as above
OPTIONAL P-channel MOSFET connect:
- SOURCE to Arduino 5V or OPTIONAL external source or P-channel MOSSFET
- DRAIN  to LEDs as above
- GATE   to Arduino as above
OPTIONAL 10k potentiometer connect:
- wiper to Arduino as above
- other terminals to +5V and GND respectively -- reverse connection if wiper voltage decreases with CW rotation
OPTIONAL External power supply connect:
- +5V output to MOSFET or LEDs as above
- GND to Arduino GND pin
*/

//User Functions Finder -- this sketch could run 500 lines
//Highlight a line below, press <CTRL> + <F>, then <ENTER>
//OR press <CTRL> + <F>, followed by the pound sign and the first few letters of the function name eg. #loop
/*
void setup()
void loop()
void XX595REG_OUT(byte
void KEYS(){
void EQUALS(){
void LESSTHAN(){
void QUESTION(){
void SLASH(){
void POUND(){
void ANNUN_STORE_BIT(
void DISPLAY_WARN_LED() {
void TURN_ALL_LEDs(byte
void BLANK_ANNUNCIATOR_ONLY() {
void LED_DIMMer() {
*/

//************
//UTILITIES  ************************************** 
//************
// next line displays cushion between program memory and user memory. https://playground.arduino.cc/Code/AvailableMemory/
#include <MemoryFree.h>
// To use, copy instruction Serial.println("freeMemory()=   " + String(freeMemory())); where you suspect memory overlap.
// next two lines enable use of F( macro. Stores lcd.print(F(" ")) or Serial.println(F(" ")) strings in program memory.
#include <WString.h>
#define F(string_literal) (reinterpret_cast<const __FlashStringHelper *>(PSTR(string_literal)))
//end utilities

//************************************
//ASSIGN ALIAS TO INPUT/OUTPUT PINS  **************************************
//************************************

//74XX595 SHIFT REGISTER
const byte RCLK   = 6; // GREEN  -- chip pin # 12 -- breakout board pin /Clock
const byte SER    = 7; // WHITE  -- chip pin # 14 -- breakout board pin /Serial_IN
const byte SRCLK  = 8; // ORANGE -- chip pin # 11 -- breakout board pin /L_Clock
//OTHER NAMED PINS
const byte pin_push2test = 2;        // ground pin to light all LEDs
const byte pin_blankAnnunciator = 4; // ground pin to blank all LEDs
const byte pin_readPot = 5;          // see pin A5 below
const byte pin_warnLED = 14;         // LED cathode -- sinks voltage from 'WARN' LED (UNO and NANO pin A0)

//ANALOG PINS -- 
//pin D3 - tone()      -- piezo +ve -- provides PWM to piezo buzzer -- optional
//pin D9 - analogWrite -- P-channel MOSFET gate -- provides PWM to MOSFET LED intensity control
//pin A5 - analogRead  -- wiper 10k ohm pot -- reads P-channel MOSFET control pot
//end assign alias to ....

//*************
//USER SETUP  **************************************  
//*************

const byte XX595_chipCount = 2; // ENTER NUMBER OF SHIFT REGISTER CHIPS HERE
const int  loop_timer = 60;     // set value to 600 for programming and debugging, return to 60 when happy
const byte dimmer_max = 150;    // ENTER VALUE to limit maximum LED brightness -- smaller number raises brightness
// end user setup

const byte XX595_bitCount  = XX595_chipCount * 8;
byte XX595Register[XX595_chipCount]; // holds LED matrix before passing to the shift register

//SYSTEM -- store downloaded values from Link2FS or status of grouped downloads
bool system_busVoltage  = true;

//XX595 PARALLEL OUT REGISTER
long XX595Reg_slot; // schedules call to XX595REG_OUT()

//REGISTERS or STATUS
byte gear_status;
bool annunciator_blankedState;



// *******
//SETUP  **************************************
// *******
/*
Defines digital pins 2 & 4 as input pins using internal pullups.
Defines digital pins 6, 7, 8, & 19 as output pins. Initial output is HIGH
Defines parameters for the serial bus
Resets all bits in the annunciator register array elements
*/

void setup() { //  #setup

//PIN ALIAS NAMES ARE USED EXCLUSIVELY IN THE SKETCH -- update ** DEFINE INPUT/OUTPUT PINS ** to change pin assignment

//Annunciator P2Test
  pinMode(pin_push2test, INPUT_PULLUP); // pin D2
//Annunciator Suppress
  pinMode(pin_blankAnnunciator, INPUT_PULLUP); // pin D4
//SN74XX595 breakout board -- D6 D7 D8 -- serial in - parallel out shift register
  pinMode(RCLK, OUTPUT);  digitalWrite(RCLK, HIGH);  // pin D6 -- GREEN
  pinMode(SER, OUTPUT);   digitalWrite(SER , HIGH);  // pin D7 -- WHITE
  pinMode(SRCLK, OUTPUT); digitalWrite(SRCLK, HIGH); // pin D8 -- ORANGE
//WARN LED
  pinMode(pin_warnLED, OUTPUT); digitalWrite(pin_warnLED, HIGH); // pin D14 -- UNO and NANO boards analog A0
//OTHER PINS
//pin D3 PWM used by tone() -- optional audio warning
//pin A5 analog input from 10k pot wiper -- sets optional LED voltage
//SERIAL BUS SETUP
  Serial.begin(115200); 
//CLEAR XX595Register[]
  for (byte x=0; x<XX595_chipCount; x++) {XX595Register[x] = 0;}

}// end setup()



//*********
//LOOP()  **************************************
//*********
/*
There are only two routines:
ROUTINE 1 calls  XX595REG_OUT() and LED_DIMMer() functions THEN waits for XX595Reg_slot to time out.
  NOTE AFTER DOWNLOADING for first time XX595Reg_slot loops every 600 milliseconds TO ALLOW USER to read debugging
  messages. AFTER THE SKETCH RUNS SMOOTHLY uncomment all messages and CHANGE 'XX595Reg_slot += 600;' TO 
  XX595Reg_slot += 60; SAVE changes to the sketch
ROUTINE 2 Polls the serial bus buffer for data from Link2FS then directs individual packets to appropreate funtion.
*/

void loop() { //  #loop
  char serial_codeIn; int led_analog; static long XX595Reg_slot;

  if (millis() > XX595Reg_slot) {
    XX595REG_OUT();
    LED_DIMMer();
    XX595Reg_slot += loop_timer;
  }// close led controller

//MONITOR the serial bus
  if (Serial.available()){ // now lets check the serial buffer for any input data
    serial_codeIn = getChar();
    if (serial_codeIn == 'D') {KEYS();}     // The first identifier is "D"
    if (serial_codeIn == '=') {EQUALS();}   // The first identifier is "="
    if (serial_codeIn == '<') {LESSTHAN();} // The first identifier is "<"
    if (serial_codeIn == '?') {QUESTION();} // The first identifier is "?"
    if (serial_codeIn == '/') {SLASH();}    // The first identifier is "/"
    if (serial_codeIn == '#') {POUND();}    // The first identifier is "#"
  }// end route data
}// end loop()



//*********************************
// PARALLEL SHIFT OUT SN74HC595N  **************************************
//*********************************
/*
  SHIFT REGISTER 
  All shift registers retain the old programming in the output latch until the final operation. 
> DRIVE THE SHIFT REGISTER CLOCK (SRCLK) LOW
> REPEAT the following sequence once for every shift register output pin
    APPLY VOLTAGE to the shift register 'Serial In' pin -- HIGH closes the internal switch of the shift register output
    PULSE THE SERIAL REGISTER CLOCK (RCLK) LOW then HIGH to add bit to first register and push other bits downstream
> DRIVE THE SHIFT REGISTER CLOCK (SRCLK) HIGH replaces old stored voltage in each output pin with new voltage in the
  register latch. 
This function handles XX595Register[] as separate elements to simplify adding more '595 shift registers
*/

void XX595REG_OUT() { //  #595
  byte tempByte; byte on = 1; byte off = 0;

//SWITCH ALL LEDs
//turn all off (master battery and alternator switches off)
  if (system_busVoltage == false) {
    //Serial.println("power off");
    TURN_ALL_LEDs(off);}
//light all LEDs (push 2 test)
  if ((system_busVoltage == true) && (digitalRead(pin_push2test) == LOW)) {
      //Serial.println("push 2 test");
      TURN_ALL_LEDs(on); 
      annunciator_blankedState = false; }

//LIGHT SELECTED LEDs
  if ((system_busVoltage == true) && (digitalRead(pin_push2test) == !LOW)) {
//blank annuniator display
    if (digitalRead(pin_blankAnnunciator) == LOW) {  
      //Serial.println("blank annunciator display");
      annunciator_blankedState = true;
      BLANK_ANNUNCIATOR_ONLY(); }
//display warning LEDs
    else if (annunciator_blankedState == false) {
      digitalWrite(SRCLK, LOW);
/*display XX595Register[1]
  this is me getting fancy, also easier to explain asssociating XX595Register[] values with pin output state
line 1 copy the XX595Register to a new byte
line 3 the SER output pin is driven by boolean '&' of most significant bit of the new byte with the value 1
line 5 shift all bits of the new byte one bit to the left creating MSB pointing to then next LED
*/
//load register 2     
      byte tempByte = XX595Register[1];
      for (int counter=7; counter>-1; counter--) {
        digitalWrite(SER, !(tempByte & 0b10000000)); 
        digitalWrite(RCLK, LOW); digitalWrite(RCLK, HIGH);
        tempByte = tempByte << 1; }
//load register 1
      tempByte = XX595Register[0];
      for (int counter=7; counter>-1; counter--) {
        digitalWrite(SER, !(tempByte & 0b10000000)); 
        digitalWrite(RCLK, LOW); digitalWrite(RCLK, HIGH);
        tempByte = tempByte << 1; }
      digitalWrite(SRCLK, HIGH);
      DISPLAY_WARN_LED();
    }// close else
  }// close switch select leds
}// end xx595reg_out()



//*******************
//Serial Streaming  **************************************
//*******************
/*
Receives status of selected data from FSX via SimConnect and Link2FS via USB serial downloads. Repeated downloads
of old data are ignored. Format of data stream is one function assignment character and one case assignment character
followed by one to five or more data characters.

In most case statements there is only one data char in the downloaded packet.
line 1 tests the data character char. Result is true if char = '1', false if char = '0'
line 2 reads current status of bit of the register byte assigned to the download and passes the new data to the
       register overflow checking function if new and old values differ.
*/

char getChar(){ // Get a character from the serial buffer
  while(Serial.available() == 0);// wait for data
  return((char)Serial.read());// Thanks Doug
}// end getchar()


void KEYS(){  // The first identifier was "D"  #D
  byte CodeIn = getChar(); // Get another character
  switch(CodeIn) {// Now lets find what to do with it
  }// close switch
}// end keys()


void EQUALS(){  // The first identifier was "="  #=
  byte CodeIn = getChar(); // Get another character
  switch(CodeIn) {// Now lets find what to do with it
  }// close switch
}// end equals()


void LESSTHAN(){  // The first identifier was "<"  #<
  // NULL = 255, GEAR = 200, FLAPS = 205  
  byte CodeIn = getChar(); // Get another character
  switch(CodeIn) {// Now lets find what to do with it

  case 'b': { // Pitot Heat Warning -- '1' = warn  '0' = no warn  -- top line Extractions(2) check box <b
/* long form
Load the last character of the Serial bus data packet into tempChar.
Compare tempChar to the ASCII character '1' and store 'true' in tempBool if both characters match.
Extract stored result of last call to this case from XX595Register[1] bit #4 and store as variable tempByte.
tempBool and tempByte are variations on a theme of 8 bit variables. Any difference in variable structure is not flagged.
Compare tempByte and tempBool and if they do not match, call the function void ANNUN_STORE_BIT().
ANNUN_STORE_BIT() provides error checking to confine changed bit to only XX595Register[] array. 
Finally write latest data into designated bit in 'XX595Register[]'
'XX595Register[]' update does not appear on annunciator panel LEDs until void XX595REG_OUT() is called by void loop().
*/
    bool tempBool = false; byte tempByte = 0;
    char tempChar = getChar(); 
    if (tempChar == '1') tempBool = true;  
    tempByte = bitRead(XX595Register[1] , 4); 
    if (tempBool != tempByte) ANNUN_STORE_BIT(1, 4, tempBool); 
    //Serial.println("pitot heat warning   " + String(tempBool));
  }// close 'b'
  break;

// or short form
  
  case 'k': { // Start Power Warning -- '1' = warn  '0' = no warn  -- top line Extractions(2) check box <k
    bool  tempBool = (getChar() == '1');    
    if (bitRead(XX595Register[0] , 7) != tempBool) ANNUN_STORE_BIT(0, 7, tempBool);
    //Serial.println("start power warning   " + String(tempBool));
  }// close 'k'
  break;
  
  }// close switch
}// end LESSTHAN()


void QUESTION(){ // The first identifier was "?"  #?
  char codeIn = getChar(); // Get another character
  //Serial.println("Question  " + String(codeIn));
  switch(codeIn){ // Now lets find what to do with it - like convert it to an integer
    
  case 'G': { // Boost Pump Warning -- '1' = warn  '0' = no warn  -- top line Extractions(2) check box ?G
    bool  tempBool = (getChar() == '1');   
    if (bitRead(XX595Register[1] , 5) != tempBool) ANNUN_STORE_BIT(1, 5, tempBool);
    //Serial.println("boost pump warning   " + String(tempBool));
  }// close 'G'
  break;

/*
Electrical Bus Voltage
This case outputs the aircraft voltage status. Individual LEDs are not directly affected.
*/
  case 'K': { // Electrical Bus Voltage -- eg ?K24.6 -- top line Extractions(2) check box ?K
    bool tempBool = (getChar() != '0');
    for (byte x=0; x<3; x++) getChar(); // strip off unnecessary characters
    if (tempBool != system_busVoltage) {system_busVoltage = tempBool;}
    //Serial.println(system_busVoltage);
  }// close 'K'
  break;
  
/*
GEAR POSITION -- this is the exception to the single data character download. The three characters of data define the
                 three gear positions '000' = gear up and locked, '222' gear down and locked, anything in between means
                 gear is in transition.
line 1 load three characters into a temporary string
line 2 convert the temporary string to a temporary integer
*/
  case 'Y':{ //Gear Position  Nose, L, R -- eg. ?Y222 -- sequence nose left right -- top line Extractions(2) check box ?G
    //gear_status - gear down = bit 0,  gear in transit bit= 1, gear up = no bits
    byte tempByte; String tempString = "";
//clear landing gear warning bits
    for (byte x=0; x<3; x++) tempString += getChar();
    tempByte = tempString.toInt();
/*
For each position:
line 1 compare the temporary byte to first and second bits in the gear status register
line 2 update XX595Register[0] annunciator gear LEDs
line 3 update XX595Register[1] gear status LEDs
line 4 updates current gear status register
*/
//gear up
    if ((tempByte == 0) && ((gear_status & 3) != 0)) {
      ANNUN_STORE_BIT(0, 0, false); ANNUN_STORE_BIT(0, 2, false); // annunciator gear LEDs
      ANNUN_STORE_BIT(1, 6, false); ANNUN_STORE_BIT(1, 7, false); // landing gear status LEDs
      bitClear(gear_status,0); bitClear(gear_status, 1); }
// in transition
    if ((tempByte > 0) && (tempByte < 222) && (bitRead(gear_status,2) != 1)) {
      ANNUN_STORE_BIT(0, 0, false); ANNUN_STORE_BIT(0, 2, true);
      ANNUN_STORE_BIT(1, 6, false); ANNUN_STORE_BIT(1, 7, true);
      bitSet(gear_status,1); bitClear(gear_status,0); }
// gear down
    if ((tempByte == 222) && (bitRead(gear_status,0) != 1)) {
      ANNUN_STORE_BIT(0, 0, true); ANNUN_STORE_BIT(0, 2, false);
      ANNUN_STORE_BIT(1, 7, false); ANNUN_STORE_BIT(1, 6, true);
      bitSet(gear_status,0); bitClear(gear_status,1); }
    //Serial.println("gear_status   " + String(gear_status));
  }// close 'Y':
  break ;

  }// close switch
}// end QUESTION()


void SLASH(){ // The first identifier was "/"` -- open Link2FS tab 'Annunciators'   #/
  char codeIn = getChar(); // Get another character
  //Serial.println("Slash  ");
  switch(codeIn){ // Now lets find what to do with it - like convert it to an integer

  case 'J': { // Left Fuel Quantity Warning -- '1' = warn  '0' = no warn  -- check box /J
    bool tempBool = (getChar() == '1');
    if (bitRead(XX595Register[0] , 4) != tempBool) ANNUN_STORE_BIT(0, 4, tempBool);
    //Serial.println("left fuel warning  " + String(tempBool));
  }// close 'J'
  break;

  case 'K': { // Right Fuel Quantity Warning -- '1' = warn  '0' = no warn  -- check box /K
    bool tempBool = (getChar() == '1');
    if (bitRead(XX595Register[0] , 6) != tempBool) ANNUN_STORE_BIT(0, 6, tempBool);
    //Serial.println("right fuel warning  " + String(tempBool));
  }// close 'K'
  break;
  
  case 'N': { // Vacuum Suction Warning -- '1' = warn  '0' = no warn  --- check box /N
    bool  tempBool = (getChar() == '1');    
    if (bitRead(XX595Register[0] , 1) != tempBool) ANNUN_STORE_BIT(0, 1, tempBool);
    //Serial.println("hi/lo vac  " + String(tempBool));
  }// close 'N'
  break;
    
  case 'V': { // Left Alternator Voltage Warning -- '1' = warn  '0' = no warn  -- check box /V
    bool  tempBool = (getChar() == '1');
    if (bitRead(XX595Register[0] , 3) != tempBool) ANNUN_STORE_BIT(0, 3, tempBool);    
    //Serial.println("left alternator voltage warning   " + String(tempBool));
  }// close 'V'
  break;
  
  case 'W': { // Right Alternator Voltage Warning -- '1' = warn  '0' = no warn  -- check box /W
    bool  tempBool = (getChar() == '1');    
    if (bitRead(XX595Register[0] , 5) != tempBool) ANNUN_STORE_BIT(0, 5, tempBool);
    //Serial.println("right alternator voltage warning   " + String(tempBool));
  }// close 'W'
  break;
  
  }// close switch(case)
}// end slash()


void POUND(){ // The first identifier was "#" Link2FS tab Experts then tab Simconnect Extractions  #pound
//Enter the capitalized text for each case in EXPERTS SimConnect Extractions tab
  char serial_codeIn = getChar(); // Get another character
  //Serial.println("serial_codeIn  " + String(serial_codeIn));
  switch(serial_codeIn) { // Now lets find what to do with it - like convert it to an integer

  case 'A': { // Prop Deice Warning -- '1' = warn  '0' = no warn  -- code PROP DEICE SWITCH:1
    bool  tempBool = (getChar() == '1');    
    if (bitRead(XX595Register[1] , 3) != tempBool) ANNUN_STORE_BIT(1, 3, tempBool);
    //Serial.println("prop deice warning   " + String(tempBool));
  }// close 'F'
  break;

  case 'B': { // Alternate Air Warning -- '1' = warn  '0' = no warn  -- code RECIP ENG ALTERNATE AIR POSITION:1
    bool  tempBool = (getChar() == '1');    
    if (bitRead(XX595Register[1] , 2) != tempBool) ANNUN_STORE_BIT(1, 2, tempBool);
    //Serial.println("alternate air warning   " + String(tempBool));
  }// close 'H'
  break ;
  
  case 'C': { // Speed Brake Warning -- '1' = warn  '0' = no warn  code SPOILERS HANDLE POSITION
    bool  tempBool = (getChar() == '1');
    if (bitRead(XX595Register[1] , 0) != tempBool) ANNUN_STORE_BIT(1, 0, tempBool);    
    //Serial.println("speed brake warning   " + String(tempBool));
  }// close 'H'
  break ;

  case 'D': { // Standby Vacuum Warning -- '1' = warn  '0' = no warn  code CIRCUIT STANDY VACUUM ON
    bool  tempBool = (getChar() == '1');    
    if (bitRead(XX595Register[1] , 1) != tempBool) ANNUN_STORE_BIT(1, 1, tempBool);
    //Serial.println("alternate vacuum warning   " + String(tempBool));
  }// close 'M'
  break ;
  
  }// close switch
}// end POUND()

//********************
//ANNUNCIATOR FUNCTIONS  **************************************
//********************
/*
ANNUN_STORE_BIT
Updates a bit within the register array and prevents register overflow.
line 1 confines updated byte to one of the array elements
line 2 confines updated bit to the above byte
line 3 updates the bit - set or reset
line 4 wakes annunciator after blanking first 14 LEDs
optional generates pulse to audio device and prints register status to serial monitor
*/

void ANNUN_STORE_BIT(byte element, byte posn, bool switchThrown) {
//Stop register overflow by limiting bit manipulation to only LED registers
  if (element > XX595_chipCount - 1) return;
  if (posn > 7) return;
  bitWrite(XX595Register[element], posn, switchThrown);
  annunciator_blankedState = false;
  //tone(3,2000, 150); // audible warning
  //Serial.println(String(XX595Register[0]) + "   " + String(XX595Register[1]));
}// end annun_store_bit()


/*
ANNUNCIATOR ALERT LED
Turns the annunciator warning LED on or off
line 1 combines the annunciator register bytes into one unsigned integer
line 2 removes any bits from the above not intended to activate warning
line 3 any bits remaining will pull output to active low LED down
line 4 failure of above turns LED off
*/
void DISPLAY_WARN_LED() {
  unsigned int warn_integer = word(XX595Register[1], XX595Register[0]);
  unsigned int warn_masked = warn_integer & 0b0000001111011110;
  if (warn_masked > 0) {digitalWrite(pin_warnLED, LOW);}
  else digitalWrite(pin_warnLED, HIGH);
// Serial.print ("warn_screened  "); Serial.println(warn_screened, BIN);
}// end display_warn_led()


/*
TURN_ALL_LEDs(on or off) (use reverse polarity)
lines 1, 3, 4, & 5  sequence shift register
line  2 sets state of all LEDs 
line  6 sets state of 'WARN' LED
*/
void TURN_ALL_LEDs(byte onOff) {
  digitalWrite(SRCLK, LOW);
  digitalWrite(SER, !onOff);
  for (int counter=0; counter<(XX595_bitCount); counter++){
    digitalWrite(RCLK, LOW); digitalWrite(RCLK, HIGH); 
  }// close for loop
  digitalWrite(SRCLK, HIGH);
  digitalWrite(pin_warnLED, !onOff);
}// end test_annunciator()


/*
BLANK_ANNUNCIATOR
Turns off all annunciator LEDs while allowing landing gear LEDs to remain on
line  1 combines the annunciator register bytes into one unsigned integer
line  2 resets any bits from the above except bits 14 & 15
line  5 any setbits remaining will pull output to active low LED down
lines 3, 4, 6, & 8 sequence shift register
line  5 line 5 applies value of the most significant bit - as blank_masked is sequenced - to the shift register input pin
line  9 shifts blank_masked one bit to the left
*/

void BLANK_ANNUNCIATOR_ONLY() {
  unsigned int blank_integer = word(XX595Register[1], XX595Register[0]);
  unsigned int blank_masked = blank_integer & 0b1100000000000000;
  //Serial.println("blank_integer   " + String(blank_integer , BIN));
  digitalWrite(SRCLK, LOW);
  for (int counter=XX595_bitCount -1; counter>-1; counter--){
    digitalWrite(SER, !(blank_masked & 0b1000000000000000)); 
    digitalWrite(RCLK, LOW); digitalWrite(RCLK, HIGH);
    blank_masked = blank_masked << 1;  //Serial.println(blank_masked, BIN);
  }// close for loop
  digitalWrite(SRCLK, HIGH);
}// end blank_annunciator_only()


/*
DIM_LEDS Optional
line 2 read voltage of dimmer potentiometer
line 3 convert the above to a scaled output that will pulse the output of P-channel MOSFET
line 4 set PWM output of Arduino 'led_bright' pin
*/

void LED_DIMMer() {
  int led_analog; static int led_bright;
  led_analog = analogRead(pin_readPot);
  led_bright = map (led_analog, 0, 1023, dimmer_max, 255);
  analogWrite(9, led_bright);
  //Serial.println(String(analogRead(0)) + "   " + String(led_bright));
}// end dim_leds()

// end of sketch



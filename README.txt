ANNUNCIATOR PANEL FOR THE FSX DEFAULT AIRCRAFT 'MOONEY ORION' USING AN ARDUINO BOARD AND JIM'S LINK2FS INTERFACE PROGRAM.

Displays 14 light annunciator panel
Displays Master Warning light
Displays 2 Landing Gear Status lights
Any time electrical power is available to the virtual aircraft:
- Pushing 'blank annunciator' switch blanks the annunciator panel. 'Master Warning' and 'Landing Gear'     	lights remain on.
- Pushing 'push2test' switch turns on all indicator lights.
- Releasing 'push2test' switch or changing status of annunciator input, lights triggered indicators.
- Master Alarm indicator is switched on by selected annunciator inputs.
Turning virtual power off, switches off all indicator lights.
Any change to FSX controls is downloaded through Link2FS, and processed in real time.
Shift registers controlling the lights are updated every 60 milliseconds.

PROGRAMMING
- FSX of course. FSX included default aircraft, 'Mooney Orion' may not be available on all editions.
- Link2fs  Multi for FSX for experts  http://www.jimspage.co.nz/intro.htm
- This sketch

MATERIAL REQUIRED
> ANNUNCIATOR
- 1 Arduino board UNO, NANO or MEGA
- 2 75HC595 shift registers or pre assembled breakout boards
- 2 pushbutton switch, momentary contact, normally open
- 17 LED -- 8 red, 4 green, 4 yellow, & 1 blue -- your final annunciator panel may use what ever it 	takes to look real
- 1/8 watt resistor - axial leads or SM0805 surface mount - size requred to match brightness of four 	different colour LEDs
- hookup wire or jumpers
- circuit mounting surface - solderless breadboard or double side perforated project board
> OPTIONAL dimming circuit
- MOSFET P-channel switching transistor - most type will work that allow 5 volt control input - demo 	circuit sinks 65mA
- 5 volt power supply - wall wart or small 'buck' type switching regulator with external 12V supply
- 1 10k ohm potentiometer. 1 each 1k & 10k ohm resistors 1/8 watt
 
This project is presented in DIY form. There are 9 Arduino input/output pins, 24 kilo bytes program storage, and 1.8 kilo bytes 
of dynamic (RAM) remaining, to expand it into a Do It Your Way Project.

My thanks to github.com, the Arduino.cc community, Mark1980 @ My Cockpit.org, and especially Jim - www.jimspage.co.nz. Doing this
without their help would be much, much, more difficult.

This code is provided in the spirit of Open Source. Please copy and distribute for personal use. Poke holes in my logic and 
improve to your heart’s content.

BushPilotSimmer
May 2019

This sketch and circuit design will work 'out of the box' for all Arduino boards. Connect the input
  and output pins as directed. Complete the device wiring as outlined below.

ARDUINO IDE 
> https://www.arduino.cc/en/Main/Software Download an installer or .zip file, install the Arduino IDE 	into a folder on your root
directory (C:\). Create a shortcut and place on the desktop. Find a good YouTube video on running the 'Blink' sketch to get 
comfortable with the Arduino environment.

THIS .zip file
> Download, unzip to a folder, and load Test_xx595_register.ino sketch onto your Arduino board. Get 	this working as directed 
then download Annunciator_Board.ino to your Arduino board. Shut down 	the Arduino Terminal Monitor window.
LINK2FS 
> Download and unzip Jim's ‘Link2FSMulti for FSX for experts’. http://www.jimspage.co.nz/intro.htm 	to a new folder 'Link2FS0' in
your C:\ directory. Make a shortcut of Link2FS.exe and place it on your 	desktop. Rename to the shortcut to “Mooney Annunciator 
Panel”. Read any Instructions and hints on 	the webpage and in the ‘README.txt’ file in the zip folder.
> Start FSX and THEN start Link2Fs 'Annunciator Panel'. Assuming you are using Link2FS for the 	first time set the parameters for
'card 1'. Set 'Serial Port' to the Com port you used to load this sketch 	onto the Arduino board. Keep the default 'Baud Rate' as
115200. Set 'Cycle time (M/s)' to 10. Set 	'Refresh time (Sec)' to 30. Punch the yellow 'Save all settings for all Cards and 
Start-up' box to save changes. 
> Punch the top line tab 'SimConnect Extractions(2)' check box <b, <k, ?G, ?K, ?Y. Punch yellow box. 
NOTE with FSX running and the Arduino card 'Connected', live output is shown beside all check 	boxes. Only boxes with check 
marks pass output to the Arduino board.
> Punch the tab 'Annunciators’ check boxes: Annunciator Active, /N, /J, /K & /V . Later adjust the 	dialog boxes to match those 
used by FSX Mooney aircraft. 
> Punch the second line tab 'Experts'. Punch the new line tab 'Simconnects Assignments'
  NOTE Punch the purple box after each change. Enter one line at a time and RESET. With the plane 	in the air change the 
  monitored switch to force output. Watch for change in the 'Result' box to 	confirm that the entry is working. Then go on to 
  the next box.
Alternate Air and Right Alternator are not programmed into this aircraft and I could not find a SimConnect Event or Extraction 
  to operate or monitor an alternate air source control. SimConnect 	Input and Extractions are available in PDF format within 
  the FSX program folder

Line	Variable 	                                         Units 	Format	Check box
#A	  PROP DEICE SWITCH:1	                                        0 	    #A
#B	  RECIP ENG ALTERNATE AIR POSITION:1		                      0	      #B
#C	  SPOILERS HANDLE POSITION		                                0	      #C
#D	  CIRCUIT STANDY VACUUM ON -- spelling is correct -- 	        0       #D

CONSTRUCTION
> ARDUINO BOARD
Connect an Arduino MEGA or NANO board as follows. Device wiring can be found at the appropriate 	user functions below.
  digital pin # 2	to 'push2test' pushbutton switch then other side to ground.
  digital pin # 3 (PWM)	to +ve terminal of piezo buzzer then other terminal to ground 	OPTIONAL
  digital pin # 4 	to 'Suppress' pushbutton switch then other side to ground.
  digital pin # 6 	to 74XX595 chip RCLK  pin # 12   breakout board pin /Clock	GREEN 
  digital pin # 7 	to 74XX595 chip SER   pin # 14   breakout board pin /Serial_IN	WHITE
  digital pin # 8 	to 74XX595 chip SRCLK pin # 11   breakout board pin /L_Clock	ORANGE
  digital pin # 9 	to P-channel MOSFET 'gate' through 1k ohm resistor	OPTIONAL
  analog pin   D14 (A0)	to 'WARN' LED cathode though current limiting resistor then to 5V bus
  analog pin   A5 	to wiper of 10k ohm potentiometer - LED intensity adjust   OPTIONAL
  NOTE I have had no trouble with analog pin A0 working with the other unused analog pins floating 	(not pulled to 5V or GND). 
    Ground unused analog pins through high value resistors if desired.

> 74HC595N 16 pin dip chips or breakout boards
- I have used the Texas Industries datasheet for pin names. Download datasheet provided for your 	chips. All dip and smd chips 
  from all suppliers should have the same pin numbers.
- Common practice for xx595 registers is to daisy chain 5V, GND, RCK & SRCK wires from the 	Arduino board, to chip0, then to 
  chip1, and on to the following chips.
- Only the SER jumper is required to go from the Arduino board to chip0 SER input. Then from chip0 	serial out (QH1 aka QH 
  prime) to the following xx595 register SER input and so on.
- Alternately, all chip 5V pins may connect individually to the same Arduino 5V pin. So too for the 	GND, RCK, and SRCK pins. 
  Use what is easiest to connect and follow when trouble shooting.

> Right now the offshore supply of xx595 breakout boards is quite limited. A two row 'dip' chip will 	easily fit onto a piece of 
  double side perforated 8 X 9 pad PCB board. Position the chip with two pads 	outboard of the chip terminal pins and one row of 
  pads above the VCC pin end. I permanently 	disabled the onboard output clear function and output enable pins of the xx595 to 
  reduce the number of jumpers and Arduino pins.
- 30AWG bare copper wire or one strand taken from 16AWG stranded cable is heavy enough. On the 	top side of the project board, 
  lay down a trace from the VCC pin, down the closest line of pads to the 	SRCLR pin. Pass the wire through the board then up 
  through the next pad in one location so that this 	trace is accessible later, in case you want break the trace and control the
  state of the SRCLR pin. On 	the underside run a wire from the GND pin up the near line of pads and across to the OA pin. Run a 
  third wire from the QA pin across to the second line of pads and then up to the top line of pads. 	Turn left and cross over to
  the pad above and outboard of register pin ‘QB’.
- The top wire and the bottom wires will cross in two places. Use a small drill to remove the thru hole 	plating of these two 
  pads and isolate the wires. Carefully tack the top wire in place pulling the ends
   through the pads occupied by VCC and SRCLR pins. On the VCC side of the board, tack two rows of 	header pins outboard of the 
   chip with the pins facing up. The header pins beside the SRCLR pin may 	be removed. Remove one header pin from the pads beside
   the SER and QH prime pins.
- On the other side tack one header strip in the pads outboard of the register ‘Q’ and GND pins. Tack 	one pin into the empty 
    pad outboard of the GND pin. Connect each register control and power pins to 	the header pins outboard of the chip. On the ‘Q’ 
    side, you have a gap between the chip terminal pins 	and the header pins just long enough to fit a sm0805 10mW LED current 
    limiting resistor. If you 	locate the resistors elsewhere, push the chip's terminal pins sideways against the header pins and 
    solder. 
- You have output pins A thru H down one side and one or two header pins for control and power. Eight empty pads outboard of the
  ‘Q’ header pins could be used as power buses. There are never enough +5V and GND access points when building an instrument panel.
- 170 point solderless breadboards make good mounting pads. If you have 20mm header pins, replace 	one normal pin at the four 
  corners of the project board in a pattern so that the 5 pin conductors of the breadboard will not create a short. A standard 
  length pin, in an unconnected pad of the project board, 	facing downward, can be inserted anywhere on the solderless 
  breadboard.
- Connect anodes of all LEDs to Arduino 5V, external 5V power supply, or the drain of a P-channel 	MOSFET. Connect cathodes
  through a current limiting resistor to the shift register, or Arduino output 	pin.

Arranged in order assuming shift register 0 is connected to the left most 8 LEDs of the annuciator. 	

  Pin 	  Input				                    Type		  String	 Colour
  pin A0 	to Annunciator Gear Down	      position	?Y222	   green
  pin B0 	to HILO Vacuum		              warning	  /N	     red
  pin C0 	to Annunciator Gear In Transit	position	?Y111	   red
  pin D0 	to Left Alternator		          warning	  /V	     red
  pin E0 	to Left Fuel			              warning	  /J	     red
  pin F0 	to Right Alternator		          warning	  /W     	 red
  pin G0 	to Right Fuel			              warning	  /K	     red 
  pin H0 	to Starter Engaged		          warning	  <k	     red
  pin A1 	to Speed Brake			            position	#C	     amber
  pin B1 	to Standby Vacuum		            warning   #D	     amber 
  pin C1 	to Alternate Air			          position	#B	     amber
  pin D1 	to Prop Deice			              position	#A	     blue 
  pin E1 	to Pitot Heat			              position	<b	     green   
  pin F1 	to Fuel Boost Pump		          position	?G	     green   
  pin G1 	to Indicator Gear Down  		    duplicate		       green
  pin H1 	to Indicator Gear In Transit    duplicate		       red

> LEDs 
- The FSX default Mooney Orion has a backlit annunciator panel. Plain LEDs and legends are kinda 	cheesy. I'm starting small. My 
  demo board uses 5mm LEDs from a grab bag of green, red, and 	yellow. Surprisingly the greens are dimmer than the other two. 
  Blue LED uses blue pin and anode of 	RGB LED. Try to source ultra-bright units. They require larger value resistors which 
  reduces the 	current draw, desirable when running without an external voltage supply. Connect a sample of each 	colour to a 
  5V supply through resistors and vary the size of the resistors until all LEDs are bright with 	the same intensity. Seventeen 
  5mm LEDs draw too much current to connect directly to a PWM pin 	as a dimmer.
- Connect anodes of all LEDs to Arduino 5V bus or drain of optional P-channel MOSFET.
- Connect cathodes of annunciator LED to appropriate pins of the shift registers through resistors as 	determined above.
- Connect Master Warning LED cathode to Arduino pin D19 (UNO and NANO aka pin A5) through 	resistor as above. This LED can be 
  moved to a third shift register when adding to the sim. The sketch 	is expandable using this code as template. 

> P-channel MOSFET (Optional) tested with BS250 (120 mA rated) or IRFD9110 (500 mA rated). 	Anything rated for 5V (-5volt) or 
  less gate to source 'ON' drop should work.
- Drain connect to anodes of all LEDs
- Source, connect to Arduino 5V bus or external 5V supply
- Gate, connect to Arduino pin D9 through 1k ohm resistor AND to (Arduino or external) +5V volt 	source through 10k resistor.

> Dimming potentiometer 10k ohm linear taper pot
- Outside pins to VCC and GND
  Wiper pin to Arduino analog pin A0
  swap 5V and GND connections when intensity varies in the wrong direction.

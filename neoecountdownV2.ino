
#include "pitches.h"

#define PIN_BEEP 2

#define PIN_DC  5
#define PIN_DIN  4
#define PIN_CLK  3
#define PIN_BL  6

#define PIN_BTN_UP     7
#define PIN_BTN_DOWN   8

#define PIN_SCE   9
#define PIN_RESET 10

//power save after 10 sec
#define PowerSave 10

int status = 0;
int btn_key= 0;
long cd , // count down
acc ;// key accelarate

// BEEP
// notes in the melody:
int melody[] = {
	NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
	4, 8, 8, 4, 4, 4, 4, 4
};

unsigned long t1, t2, tk/*for power saving*/;

boolean light;


// LCD
#define CNT_CHAR   96

//The DC pin tells the LCD if we are sending a command or data
#define LCD_COMMAND 0
#define LCD_DATA  1

//You may find a different size screen, but this one is 84 by 48 pixels
#define LCD_X     84
#define LCD_Y     48

//This table contains the hex values that represent pixels
//for a font that is 5 pixels wide and 8 pixels high
static const byte ASCII[][5] = {
	{0x00, 0x00, 0x00, 0x00, 0x00} // 20
	,{0x00, 0x00, 0x5f, 0x00, 0x00} // 21 !
	,{0x00, 0x07, 0x00, 0x07, 0x00} // 22 "
	,{0x14, 0x7f, 0x14, 0x7f, 0x14} // 23 #
	,{0x24, 0x2a, 0x7f, 0x2a, 0x12} // 24 $
	,{0x23, 0x13, 0x08, 0x64, 0x62} // 25 %
	,{0x36, 0x49, 0x55, 0x22, 0x50} // 26 &
	,{0x00, 0x05, 0x03, 0x00, 0x00} // 27 '
	,{0x00, 0x1c, 0x22, 0x41, 0x00} // 28 (
	,{0x00, 0x41, 0x22, 0x1c, 0x00} // 29 )
	,{0x14, 0x08, 0x3e, 0x08, 0x14} // 2a *
	,{0x08, 0x08, 0x3e, 0x08, 0x08} // 2b +
	,{0x00, 0x50, 0x30, 0x00, 0x00} // 2c ,
	,{0x08, 0x08, 0x08, 0x08, 0x08} // 2d -
	,{0x00, 0x60, 0x60, 0x00, 0x00} // 2e .
	,{0x20, 0x10, 0x08, 0x04, 0x02} // 2f /
	,{0x3e, 0x51, 0x49, 0x45, 0x3e} // 30 0
	,{0x00, 0x42, 0x7f, 0x40, 0x00} // 31 1
	,{0x42, 0x61, 0x51, 0x49, 0x46} // 32 2
	,{0x21, 0x41, 0x45, 0x4b, 0x31} // 33 3
	,{0x18, 0x14, 0x12, 0x7f, 0x10} // 34 4
	,{0x27, 0x45, 0x45, 0x45, 0x39} // 35 5
	,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 36 6
	,{0x01, 0x71, 0x09, 0x05, 0x03} // 37 7
	,{0x36, 0x49, 0x49, 0x49, 0x36} // 38 8
	,{0x06, 0x49, 0x49, 0x29, 0x1e} // 39 9
	,{0x00, 0x36, 0x36, 0x00, 0x00} // 3a :
	,{0x00, 0x56, 0x36, 0x00, 0x00} // 3b ;
	,{0x08, 0x14, 0x22, 0x41, 0x00} // 3c <
	,{0x14, 0x14, 0x14, 0x14, 0x14} // 3d =
	,{0x00, 0x41, 0x22, 0x14, 0x08} // 3e >
	,{0x02, 0x01, 0x51, 0x09, 0x06} // 3f ?
	,{0x32, 0x49, 0x79, 0x41, 0x3e} // 40 @
	,{0x7e, 0x11, 0x11, 0x11, 0x7e} // 41 A
	,{0x7f, 0x49, 0x49, 0x49, 0x36} // 42 B
	,{0x3e, 0x41, 0x41, 0x41, 0x22} // 43 C
	,{0x7f, 0x41, 0x41, 0x22, 0x1c} // 44 D
	,{0x7f, 0x49, 0x49, 0x49, 0x41} // 45 E
	,{0x7f, 0x09, 0x09, 0x09, 0x01} // 46 F
	,{0x3e, 0x41, 0x49, 0x49, 0x7a} // 47 G
	,{0x7f, 0x08, 0x08, 0x08, 0x7f} // 48 H
	,{0x00, 0x41, 0x7f, 0x41, 0x00} // 49 I
	,{0x20, 0x40, 0x41, 0x3f, 0x01} // 4a J
	,{0x7f, 0x08, 0x14, 0x22, 0x41} // 4b K
	,{0x7f, 0x40, 0x40, 0x40, 0x40} // 4c L
	,{0x7f, 0x02, 0x0c, 0x02, 0x7f} // 4d M
	,{0x7f, 0x04, 0x08, 0x10, 0x7f} // 4e N
	,{0x3e, 0x41, 0x41, 0x41, 0x3e} // 4f O
	,{0x7f, 0x09, 0x09, 0x09, 0x06} // 50 P
	,{0x3e, 0x41, 0x51, 0x21, 0x5e} // 51 Q
	,{0x7f, 0x09, 0x19, 0x29, 0x46} // 52 R
	,{0x46, 0x49, 0x49, 0x49, 0x31} // 53 S
	,{0x01, 0x01, 0x7f, 0x01, 0x01} // 54 T
	,{0x3f, 0x40, 0x40, 0x40, 0x3f} // 55 U
	,{0x1f, 0x20, 0x40, 0x20, 0x1f} // 56 V
	,{0x3f, 0x40, 0x38, 0x40, 0x3f} // 57 W
	,{0x63, 0x14, 0x08, 0x14, 0x63} // 58 X
	,{0x07, 0x08, 0x70, 0x08, 0x07} // 59 Y
	,{0x61, 0x51, 0x49, 0x45, 0x43} // 5a Z
	,{0x00, 0x7f, 0x41, 0x41, 0x00} // 5b [
	,{0x02, 0x04, 0x08, 0x10, 0x20} // 5c \
	,{0x00, 0x41, 0x41, 0x7f, 0x00} // 5d ]
	,{0x04, 0x02, 0x01, 0x02, 0x04} // 5e ^
	,{0x40, 0x40, 0x40, 0x40, 0x40} // 5f _
	,{0x00, 0x01, 0x02, 0x04, 0x00} // 60 `
	,{0x20, 0x54, 0x54, 0x54, 0x78} // 61 a
	,{0x7f, 0x48, 0x44, 0x44, 0x38} // 62 b
	,{0x38, 0x44, 0x44, 0x44, 0x20} // 63 c
	,{0x38, 0x44, 0x44, 0x48, 0x7f} // 64 d
	,{0x38, 0x54, 0x54, 0x54, 0x18} // 65 e
	,{0x08, 0x7e, 0x09, 0x01, 0x02} // 66 f
	,{0x0c, 0x52, 0x52, 0x52, 0x3e} // 67 g
	,{0x7f, 0x08, 0x04, 0x04, 0x78} // 68 h
	,{0x00, 0x44, 0x7d, 0x40, 0x00} // 69 i
	,{0x20, 0x40, 0x44, 0x3d, 0x00} // 6a j
	,{0x7f, 0x10, 0x28, 0x44, 0x00} // 6b k
	,{0x00, 0x41, 0x7f, 0x40, 0x00} // 6c l
	,{0x7c, 0x04, 0x18, 0x04, 0x78} // 6d m
	,{0x7c, 0x08, 0x04, 0x04, 0x78} // 6e n
	,{0x38, 0x44, 0x44, 0x44, 0x38} // 6f o
	,{0x7c, 0x14, 0x14, 0x14, 0x08} // 70 p
	,{0x08, 0x14, 0x14, 0x18, 0x7c} // 71 q
	,{0x7c, 0x08, 0x04, 0x04, 0x08} // 72 r
	,{0x48, 0x54, 0x54, 0x54, 0x20} // 73 s
	,{0x04, 0x3f, 0x44, 0x40, 0x20} // 74 t
	,{0x3c, 0x40, 0x40, 0x20, 0x7c} // 75 u
	,{0x1c, 0x20, 0x40, 0x20, 0x1c} // 76 v
	,{0x3c, 0x40, 0x30, 0x40, 0x3c} // 77 w
	,{0x44, 0x28, 0x10, 0x28, 0x44} // 78 x
	,{0x0c, 0x50, 0x50, 0x50, 0x3c} // 79 y
	,{0x44, 0x64, 0x54, 0x4c, 0x44} // 7a z
	,{0x00, 0x08, 0x36, 0x41, 0x00} // 7b {
	,{0x00, 0x00, 0x7f, 0x00, 0x00} // 7c |
	,{0x00, 0x41, 0x36, 0x08, 0x00} // 7d }
	,{0x10, 0x08, 0x08, 0x10, 0x08} // 7e ~
	,{0x78, 0x46, 0x41, 0x46, 0x78} // 7f DEL
};

void gotoXY(int x, int y) {
	LCDWrite(0, 0x80 | x);  // Column.
	LCDWrite(0, 0x40 | y);  // Row.  ?
}

void lcd_home() { gotoXY(0, 0); }

//This function takes in a character, looks it up in the font table/array
//And writes it to the screen
//Each character is 8 bits tall and 5 bits wide. We pad one blank column of
//pixels on each side of the character for readability.
void lcd_char(char character) {
	int xi = character - 0x20;
	if (xi<0||xi>=CNT_CHAR) return;

	LCDWrite(LCD_DATA, 0x00); //Blank vertical line padding

	for (int index = 0 ; index < 5 ; index++)
	LCDWrite(LCD_DATA, ASCII[xi][index]);
	//0x20 is the ASCII character for Space (' '). The font table starts with this character

	LCDWrite(LCD_DATA, 0x00); //Blank vertical line padding
}

//Given a string of characters, one by one is passed to the LCD
void lcd_print(const char *characters) {
	while (*characters)
	lcd_char(*characters++);
}

//Clears the LCD by writing zeros to the entire screen
void lcd_clear(void) {
	for (int index = 0 ; index < (LCD_X * LCD_Y / 8) ; index++)
	LCDWrite(LCD_DATA, 0x00);

	lcd_home();
}

//This sends the magical commands to the PCD8544
void LCDInit(void) {

	//Configure control pins
	pinMode(PIN_SCE, OUTPUT);
	pinMode(PIN_RESET, OUTPUT);
	pinMode(PIN_DC, OUTPUT);
	pinMode(PIN_DIN, OUTPUT);
	pinMode(PIN_CLK, OUTPUT);

	//Reset the LCD to a known state
	digitalWrite(PIN_RESET, LOW);
	digitalWrite(PIN_RESET, HIGH);

	LCDWrite(LCD_COMMAND, 0x21); //Tell LCD that extended commands follow
	LCDWrite(LCD_COMMAND, 0xB0); //Set LCD Vop (Contrast): Try 0xB1(good @ 3.3V) or 0xBF if your display is too dark
	LCDWrite(LCD_COMMAND, 0x04); //Set Temp coefficent
	LCDWrite(LCD_COMMAND, 0x14); //LCD bias mode 1:48: Try 0x13 or 0x14

	LCDWrite(LCD_COMMAND, 0x20); //We must send 0x20 before modifying the display control mode
	LCDWrite(LCD_COMMAND, 0x0C); //Set display control, normal mode. 0x0D for inverse
}

//There are two memory banks in the LCD, data/RAM and commands. This
//function sets the DC pin high or low depending, and then sends
//the data byte
void LCDWrite(byte data_or_command, byte data) {
	digitalWrite(PIN_DC, data_or_command); //Tell the LCD that we are writing either to data or a command

	//Send the data
	digitalWrite(PIN_SCE, LOW);
	shiftOut(PIN_DIN, PIN_CLK, MSBFIRST, data);
	digitalWrite(PIN_SCE, HIGH);
}



void setup() {

	pinMode(PIN_BEEP, OUTPUT);
	pinMode(PIN_DC, OUTPUT);
	pinMode(PIN_DIN, OUTPUT);
	pinMode(PIN_CLK, OUTPUT);
	pinMode(PIN_BL, OUTPUT);

	pinMode(PIN_BTN_UP, INPUT_PULLUP);
	pinMode(PIN_BTN_DOWN, INPUT_PULLUP);

	LCDInit();
	
	lightON();

	lcd_clear();
	lcd_print("Neoe 20151226 count down konia5110lcd  :)");
	delay(500);

	resetKey();
	playTone(PIN_BEEP);
	lcd_clear();

	go1();
}

void lightON() {
	digitalWrite(PIN_BL, HIGH);
	light=true;
}
void lightOFF() {
	digitalWrite(PIN_BL, LOW);
	light=false;
}

void go1(){ //count down
	status=1;
	lcd_clear();
	cd = 5*60+10; //,default 5min10sec
	t1 = millis();
	lightON();
	tk = t1;
	resetKey();
}
void go2(){ //sleep
	status=2;
	lcd_clear();
	lightOFF();
	delay(1000);
	resetKey();
}

void go0() {
	resetKey();
	status=0;
}

void loop() {


	if (status==0) { // play
		resetKey();
		playTone(PIN_BEEP);
		if (readKey()) {
			go2();
		}
	} else if (status==2) { // sleep
		if (readKey()){
			go1();
			return;
		}
		delay(2000);
	} else if (status==1) { // count down
		t2 =  millis();

		long cd2 = cd -(t2-t1)/1000;
		if (cd2<=0) {
			go0();
			return;
		} else {
			long tk2 = (t2 - tk)/1000;
			if (tk2>PowerSave) {
				if (light) {
					lightOFF();
				}
			}else{
				if(!light){
					lightON();
				}
			}

			lcd_clear();
			lcd_print("CD ");
			int s = cd2;
			int m = s/60;
			int h = m /60;
			s=s%60;
			m=m%60;
			if (h>0) {
				lcd_print(h);
				lcd_print(":");
			}
			if (h>0||m>0) {
				lcd_print(m);
				lcd_print(":");
			}
			lcd_print(s);
			// lcd_print("      ");

			if (readKey()) {
				tk= millis();
				if (btn_key==PIN_BTN_DOWN) {
					cd-=10+(acc++);
					acc+=4;
				}else if (btn_key==PIN_BTN_UP) {
					cd+=10+(acc++);
					acc+=4;
				}
				resetKey();
			}else{
				acc=0;
			}
			delay(100);
		}
	}

}

void resetKey() {
	delay(30);
	btn_key = 0;
}

boolean readKey() {
	if (btn_key==0) readAKey(PIN_BTN_UP);
	if (btn_key==0) readAKey(PIN_BTN_DOWN);
	return 0!=btn_key;
}
void readAKey(int pin) {
	if (digitalRead(pin)==0) btn_key=pin;
}


void playTone(byte pin){
#ifdef silent
	return;
#endif

	for (int thisNote = 0; thisNote < 8; thisNote++) {

		// to calculate the note duration, take one second
		// divided by the note type.
		//e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
		int noteDuration = 1000 / noteDurations[thisNote];
		tone(pin, melody[thisNote], noteDuration);

		if (readKey()) break;

		// to distinguish the notes, set a minimum time between them.
		// the note's duration + 30% seems to work well:
		int pauseBetweenNotes = noteDuration * 1.30;
		delay(pauseBetweenNotes);

		// stop the tone playing:
		noTone(pin);
		if (readKey()) break;
	}
	noTone(pin);
}

void lcd_print(long n){
	lcd_print(n,10);
}
void lcd_print(long n, int base) {
	char buf[8 * sizeof(long) + 1];
	int p1 =0;
	if (base < 2) {
		return;
	}
	if (n<0) {
		lcd_char('-');
		n=-n;
	}
	while(1) {
		char c = n % base;
		buf[p1++] =  c < 10 ? c + '0' : c + 'A' - 10;
		n /= base;
		if (n ==0) break;
	}

	for (int i=p1-1;i>=0;i--){
		lcd_char(buf[i]);
	}
}



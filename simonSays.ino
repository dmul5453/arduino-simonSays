

/*
   Simon says game with sound
   @author Dustin Mullins
   last updated 9/13/2022
*/

#define c5 523 // green sound
#define e5 654 // red sound
#define a5 880 // yellow sound
#define b5 988 // blue sound
#define c6 1760 // fail noise
#define buzz 13
#define duration 500

#define greenLED 2
#define greenButt 3

#define redLED 7
#define redButt 6

#define yellowLED 8
#define yellowButt 9

#define blueLED 12
#define blueButt 11

#define buzzPin 13

#define ledCount 4

#define totalRounds 35
#define increaseSpeedEveryNth 5

int ledArray[ledCount] = {greenLED, redLED, yellowLED, blueLED};
int btnArray[ledCount] = {greenButt, redButt, yellowButt, blueButt};
int noteArray[ledCount] = {c5, e5, a5, b5};

int gameState = 1;
int gameSpeed = 800;
int count = 0;
int btnPress = 0;
int sequence[totalRounds];
int btnPressCount = 0;
int sequenceState = 0;
// readVcc provided by https://xod.io/libs/vegunes/read-vcc/read-vcc/
long readVcc() {

  //delay(3); // Wait for Vref to settle

  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
  ADMUX = _BV(MUX5) | _BV(MUX0);
#elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  ADMUX = _BV(MUX3) | _BV(MUX2);
#else
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#endif

  delay(3); // Wait for Vref to settle

  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA, ADSC)); // measuring

  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH
  uint8_t high = ADCH; // unlocks both

  long result = (high << 8) | low;

  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}

void setup() {
  // put your setup code here, to run once:
  for (int i = 0; i < ledCount; i++) {
    pinMode(ledArray[i], OUTPUT);
    pinMode(btnArray[i], INPUT_PULLUP);
  }

  // next 3 lines provided by u/dedokta on reddit
  int v = readVcc();
  while ( readVcc() == v);
  randomSeed(micros());
  
}

// the functions ledOn and onward are provided by https://www.duinobit.com/projects/my-bootleg-simon-game-arduino-mini-project-and-tutorial/
// only addition was the buzzer stuff
void ledOn (int led){
  digitalWrite(ledArray[led-1], HIGH);
  tone(buzzPin, noteArray[led-1], duration);
}

void ledOff (int led){
  digitalWrite(ledArray[led-1], LOW);
}

void processBtnPress(int btn) {
  if(sequence[btnPress] == btn){
    ledOn(btn);
    delay(gameSpeed/4);
    ledOff(btn);
  }
  else {
    lose();
  }
  btnPress++;
}

void win(){
  for(int i = 0; i < 5; i++) {
    for(int i = 0; i < sizeof(ledArray)/sizeof(ledArray[0]); i++) {
      ledOn(i+1);
      delay(50);
      ledOff(i+1);
      delay(50);
    }
  }
  reset();
  delay(2000);
}

void lose() {
  for(int i = 0; i < 3; i++) {
    tone(buzzPin, e5, duration);
    tone(buzzPin, e5, duration);
    for(int i = 0; i < sizeof(ledArray)/sizeof(ledArray[0]); i++) {
      ledOn(i+1);
    }
    delay(250);
    for(int i = 0; i < sizeof(ledArray)/sizeof(ledArray[0]); i++) {
      ledOff(i+1);
    }
    delay(250);
  }
  reset();
  delay(2000);
}

void clearSequence() {
  for(int i = 0; i < sizeof(sequence)/sizeof(sequence[0]); i++) {
    sequence[i] = ' ';
  }
}

void reset() {
  clearSequence();
  count = 0;
  btnPress = 0;
  gameSpeed = 800;
  gameState = 0;
}


void loop() {
  // put your main code here, to run repeatedly:
  if(gameState == 0) {
    if(count == sizeof(sequence)/sizeof(sequence[0])){
      win();
    }
    else if(count % increaseSpeedEveryNth == 0){
      gameSpeed-=200;
    }
    sequence[count] = random(1, 5);
    
    for(int i = 0; i <= count; i++)
    {
      
    }
    
    
    for(int i = 0; i <= count; i++) {
      ledOn(sequence[i]);
      delay(gameSpeed);
      ledOff(sequence[i]);
      delay(gameSpeed);
    }
    count++;
    gameState = 1;
  }
  else if(gameState == 1){
    if(btnPress == count){
      btnPress = 0;
      gameState = 0;
      delay(2000);
    }
    else {
      if(digitalRead(btnArray[0]) == LOW || digitalRead(btnArray[1]) == LOW || digitalRead(btnArray[2]) == LOW || digitalRead(btnArray[3]) == LOW) {
        if(digitalRead(btnArray[0]) == LOW){
          processBtnPress(1);
        }
        else if(digitalRead(btnArray[1]) == LOW){
          processBtnPress(2);
        }
        else if(digitalRead(btnArray[2]) == LOW){
          processBtnPress(3);
        }
        else if(digitalRead(btnArray[3]) == LOW){
          processBtnPress(4);
        }
        delay(250);
      }
    }
  }
}

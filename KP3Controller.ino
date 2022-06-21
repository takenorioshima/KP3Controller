#include <MIDI.h> // https://playground.arduino.cc/Main/MIDILibrary/
#include <JC_Button.h> // https://github.com/JChristensen/JC_Button
#include <jled.h> // https://github.com/jandelgado/jled

const int BUTTON_PIN_A = 3;
const int BUTTON_PIN_B = 4;
const int SWITCH_PIN = 7;
const int LED_PIN = 13;

// CC, program numbers of KP3+ 
const int CC_X = 12;
const int CC_Y = 13;
const int CC_TOUCH = 92;
const int PGM_A = 103; // LoP.6 - ShuttleLop
const int PGM_B = 110; // LP.13 - Looper&HPF
const int PGM_C = 105; // LoP.8 - BreakLoop2

const int MIDI_CH = 1;

static int activeProgram;
static int xPos;
static int yPos;
static byte btnAState;
static byte btnBState;
static byte btnCState;
static byte swState;

Button btnA(BUTTON_PIN_A, 30);
Button btnB(BUTTON_PIN_B, 30);
Button sw(SWITCH_PIN, 30);

MIDI_CREATE_DEFAULT_INSTANCE();

// LED 13 as a status lamp
auto blinkLED = JLed(LED_PIN).Blink(300, 100).Forever();
auto staticLED = JLed(LED_PIN).On();

void setup() {
  btnA.begin();
  btnB.begin();
  sw.begin();
  MIDI.begin();
}

void loop() {
  btnA.read();
  btnB.read();
  if( sw.read() ){ swState = 1; }

  // Check switch state
  if( sw.wasPressed() ){
    swState = 1; // Program Change + Control Change Mode
  }
  if( sw.wasReleased() ){
    swState = 0; // Control Change Only Mode (Do not send program change)
  }
  if( swState ){
    staticLED.Update();
  } else {
    blinkLED.Update();
  }
  
  // Check button state - btnA + btnB
  if( btnAState && btnBState ){
    if( activeProgram != PGM_C && swState ){
      MIDI.sendProgramChange( PGM_C, MIDI_CH );
      activeProgram = PGM_C;
    }
    if( !btnCState ){
      if( swState ){
        xPos = random(80, 80);
        yPos = random(64, 127);
      }else{
        xPos = random(43, 85);
        yPos = random(0, 127);
      }
      touchPad(xPos, yPos);
      btnCState = 1;
    }
  }
  
  // Check button state - btnA
  if( btnA.wasPressed() ){
    btnAState = 1;
    if( activeProgram != PGM_A && swState ){
      MIDI.sendProgramChange(PGM_A, MIDI_CH);
      activeProgram = PGM_A;
    }
    if( swState ){
      xPos = random(24, 74);
      yPos = random(54, 74);
    }else{
      xPos = random(0, 42);
      yPos = random(0, 127);
    }
    touchPad( xPos, yPos );
  }
  if( btnA.wasReleased() ){
    btnAState = 0;
    btnCState = 0;
    untouchPad();
  }

  // Check button state - btnB
  if( btnB.wasPressed() ){
    btnBState = 1;
    if( activeProgram != PGM_B && swState ){
      MIDI.sendProgramChange(PGM_B, MIDI_CH);
      activeProgram = PGM_B;
    }
    if( swState ){
      xPos = random(76, 120);
      yPos = random(22, 84);
    }else{
      xPos = random(86, 127);
      yPos = random(0, 127);
    }
    touchPad(xPos, yPos);
  }
  if( btnB.wasReleased() ){
    btnBState = 0;
    btnCState = 0;
    untouchPad();
  }

}

void touchPad( int xPos, int yPos ){
  MIDI.sendControlChange(CC_X, xPos, MIDI_CH);
  MIDI.sendControlChange(CC_Y, yPos, MIDI_CH);
  MIDI.sendControlChange(CC_TOUCH, 127, MIDI_CH);
}

void untouchPad(){
  MIDI.sendControlChange(CC_TOUCH, 0, MIDI_CH);
}

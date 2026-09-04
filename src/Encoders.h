#ifndef encoders_h
#define encoders_h

// Modifiers, "||" the ones you want together and pass as third argument of constructor
#define PULLUPS 0x1     // Enable internal pull-ups on input pins
#define HIGH_STEP 0x2   // If the encoder's detents correspond with 11 instead of 00
#define REVERSE_DIR 0x4 // Reverses the two pins, swapping clockwise and counter-clockwise steps

//
// Common functionality, instantiate either FSRotary or HSRotary instead
//
class Rotary
{
public:
  Rotary(char _pin1, char _pin2);
  Rotary(char, char, char options);
  void process();        // turn pin state into sum of encoder steps. May be called from ISR.
  bool hasPosChanged();  // flag for presence of new steps collected
  signed char readPos(); // returns and zeros out step count
  void resetPos();       // just throws away any steps so far

  const unsigned char pin1;
  const unsigned char pin2;
  const char options;

protected:

  unsigned char state; // state machine internal state, only ever accessed from method process()

  // These are written to by process() (which may be called from ISR) and other methods as well
  volatile bool posChanged;     // has encoder stepped since last read/reset
  volatile signed char posDiff; // relative position of encoder since last read/reset, clockwise is positive

  volatile unsigned int faultCounter; // nothing uses the functionality yet

private:
  virtual const unsigned char (*getTable())[4] = 0;
};

class HSRotary : public Rotary // Half-stepping rotary encoder, emits codes at 00 and 11.
{
public:
using Rotary::Rotary;
protected:
  const unsigned char (*getTable())[4];
  static const unsigned char ttable[][4];
};

class FSRotary : public Rotary // Full-stepping rotary encoder, emits codes at 00 only.
{
public:
using Rotary::Rotary;
protected:
  const unsigned char (*getTable())[4];
  static const unsigned char ttable[][4];
};

#endif
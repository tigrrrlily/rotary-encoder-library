/* Rotary encoder handler for arduino. v1.1
 *
 * Copyright 2011 Ben Buxton. Licenced under the GNU GPL Version 3.
 * Contact: bb@cactii.net
 *
 * Re-structured and modified 2026 Summer Kline
 * Contact: tigrrrlily@gmail.com
 *
 * A typical mechanical rotary encoder emits a two bit gray code
 * on 3 output pins. Every step in the output (often accompanied
 * by a physical 'click') generates a specific sequence of output
 * codes on the pins.
 *
 * There are 3 pins used for the rotary encoding - one common and
 * two 'bit' pins.
 *
 * The following is the typical sequence of code on the output when
 * moving from one step to the next:
 *
 *   Position   Bit1   Bit2
 *   ----------------------
 *     Step1     0      0
 *      1/4      1      0
 *      1/2      1      1
 *      3/4      0      1
 *     Step2     0      0
 *
 * From this table, we can see that when moving from one 'click' to
 * the next, there are 4 changes in the output code.
 *
 * - From an initial 0 - 0, Bit1 goes high, Bit0 stays low.
 * - Then both bits are high, halfway through the step.
 * - Then Bit1 goes low, but Bit2 stays high.
 * - Finally at the end of the step, both bits return to 0.
 *
 * Detecting the direction is easy - the table simply goes in the other
 * direction (read up instead of down).
 *
 * To decode this, we use a simple state machine. Every time the output
 * code changes, it follows state, until finally a full steps worth of
 * code is received (in the correct order). At the final 0-0, it returns
 * a value indicating a step in one direction or the other.
 *
 * It's also possible to use 'half-step' mode. This just emits an event
 * at both the 0-0 and 1-1 positions. This might be useful for some
 * encoders where you want to detect all positions.
 *
 * If an invalid state happens (for example we go from '0-1' straight
 * to '1-0'), the state machine resets to the start until 0-0 and the
 * next valid codes occur.
 *
 * The biggest advantage of using a state machine over other algorithms
 * is that this has inherent debounce built in. Other algorithms emit spurious
 * output with switch bounce, but this one will simply flip between
 * sub-states until the bounce settles, then continue along the state
 * machine.
 * A side effect of debounce is that fast rotations can cause steps to
 * be skipped. By not requiring debounce, fast rotations can be accurately
 * measured.
 * Another advantage is the ability to properly handle bad state, such
 * as due to EMI, etc.
 * It is also a lot simpler than others - a static state table and less
 * than 10 lines of logic. (haha, well that's no longer true --Summer)
 */

#include "Arduino.h"
#include "Encoders.h"
#include "Steps.h"

#define NO_OPTIONS 0x0


// Constructor. Only called from sub-classes.
Rotary::Rotary(char _pin1, char _pin2, char _options) : pin1(_pin1), pin2(_pin2), options(_options)
{
  // Set pins to input.
  pinMode(pin1, (options && PULLUPS) ? (INPUT_PULLUP) : (INPUT));
  pinMode(pin2, (options && PULLUPS) ? (INPUT_PULLUP) : (INPUT));
  // Initialise state.
  state = R_START;
  resetPos();
  faultCounter = 0;
}

Rotary::Rotary(char _pin1, char _pin2) : Rotary(_pin1, _pin2, NO_OPTIONS) {} // no reverse, no step on high, no pullups

void Rotary::process()
{
  // Grab state of input pins.
  unsigned char pinState;
  switch (options && (REVERSE_DIR || HIGH_STEP))
  {
  case (REVERSE_DIR || HIGH_STEP):
    pinState = ((!digitalRead(pin1)) << 1) | (!digitalRead(pin2));
    break;
  case (REVERSE_DIR):
    pinState = (digitalRead(pin2) << 1) | digitalRead(pin1);
    break;
  case (HIGH_STEP):
    pinState = ((!digitalRead(pin2)) << 1) | (!digitalRead(pin1));
    break;
  default:
    pinState = (digitalRead(pin1) << 1) | digitalRead(pin2);
  }
  
  // State machine step! Determine new state from the current state, the pins and the state table.
  state = ttable[state & 0xf][pinState];

  switch (state & 0x30) // only want the direction message that's been stored in the higher bits
  {
  case DIR_CCW:
    if (posDiff != INT8_MIN)
    {
      posDiff--;
      posChanged = true;
    }
    break;
  case DIR_CW:
    if (posDiff != INT8_MAX)
    {
      posDiff++;
      posChanged = true;
    }
    break;
  case DIR_FAULT:
    if (faultCounter != UINT16_MAX)
      faultCounter++;
  }
}

bool Rotary::hasPosChanged()
{
  return posChanged;
}
void Rotary::resetPos()
{
  noInterrupts();
  {
    posDiff = 0;
    posChanged = false;
  }
  interrupts();
}

signed char Rotary::readPos()
{
  signed char copy;
  noInterrupts();
  {
    copy = posDiff;
    posDiff = 0;
    posChanged = false;
  }
  interrupts();
  return copy;
}

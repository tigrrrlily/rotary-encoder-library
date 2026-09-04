#ifndef steps_h
#define steps_h

// State table direction indicators used in 'process()'
#define DIR_NONE 0x0   // No complete step yet.
#define DIR_CW 0x10    // Clockwise step.
#define DIR_CCW 0x20   // Anti-clockwise step.
#define DIR_FAULT 0x30 // Fault detected

#define R_START 0 // state tables start in row 0.

#endif
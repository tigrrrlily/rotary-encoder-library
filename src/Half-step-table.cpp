// In here we populate the state table for the half-stepping rotary encoder handler.

#include "Encoders.h"
#include "Steps.h"

#define R_START_H R_START
#define R_CCW_BEGIN_H 0x1
#define R_CW_BEGIN_H 0x2
#define R_START_M_H 0x3
#define R_CW_BEGIN_M_H 0x4
#define R_CCW_BEGIN_M_H 0x5
#define R_FAULT_H 0x6

const unsigned char HSRotary::ttable[][4] = {
    // R_START_H
    {R_START_H,           R_CW_BEGIN_H,    R_CCW_BEGIN_H,  R_START_M_H},
    // R_CCW_BEGIN_H
    {R_START_H,           R_FAULT_H,       R_CCW_BEGIN_H,  R_START_M_H | DIR_CCW},
    // R_CW_BEGIN_H
    {R_START_H,           R_CW_BEGIN_H,    R_FAULT_H,      R_START_M_H | DIR_CW},
    // R_START_M_H (11)
    {R_START_H,           R_CCW_BEGIN_M_H, R_CW_BEGIN_M_H, R_START_M_H},
    // R_CW_BEGIN_M_H
    {R_START_H | DIR_CW,  R_FAULT_H,       R_CW_BEGIN_M_H, R_START_M_H},
    // R_CCW_BEGIN_M_H
    {R_START_H | DIR_CCW, R_CCW_BEGIN_M_H, R_FAULT_H,      R_START_M_H},
    // R_FAULT_H
    {R_START_H,           R_FAULT_H,       R_FAULT_H,      R_START_M_H}};
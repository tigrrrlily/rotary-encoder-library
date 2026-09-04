// In here we populate the state table for the full-stepping rotary encoder handler.

#include "Encoders.h"
#include "Steps.h"

#define R_START_F R_START
#define R_CW_FINAL_F 0x1
#define R_CW_BEGIN_F 0x2
#define R_CW_NEXT_F 0x3
#define R_CCW_BEGIN_F 0x4
#define R_CCW_FINAL_F 0x5
#define R_CCW_NEXT_F 0x6
#define R_FAULT_F 0x7

const unsigned char FSRotary::ttable[][4] = {
    // R_START_F
    {R_START_F,             R_CW_BEGIN_F,          R_CCW_BEGIN_F,         R_FAULT_F | DIR_FAULT},
    // R_CW_FINAL_F
    {R_START_F | DIR_CW,    R_FAULT_F | DIR_FAULT, R_CW_FINAL_F,          R_CW_NEXT_F},
    // R_CW_BEGIN_F
    {R_START_F,             R_CW_BEGIN_F,          R_FAULT_F | DIR_FAULT, R_CW_NEXT_F},
    // R_CW_NEXT_F
    {R_START_F | DIR_FAULT, R_CW_BEGIN_F,          R_CW_FINAL_F,          R_CW_NEXT_F},
    // R_CCW_BEGIN_F
    {R_START_F,             R_FAULT_F | DIR_FAULT, R_CCW_BEGIN_F,         R_CCW_NEXT_F},
    // R_CCW_FINAL_F
    {R_START_F | DIR_CCW,   R_CCW_FINAL_F,         R_FAULT_F | DIR_FAULT, R_CCW_NEXT_F},
    // R_CCW_NEXT_F
    {R_START_F | DIR_FAULT, R_CCW_FINAL_F,         R_CCW_BEGIN_F,         R_CCW_NEXT_F},
    // R_FAULT_F
    {R_START_F,             R_FAULT_F,             R_FAULT_F,             R_FAULT_F}};


const unsigned char (*FSRotary::getTable())[4]{
    return ttable;
}
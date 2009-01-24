#include "conf.h"

int *video_quality = &conf.video_quality;
int *video_mode    = &conf.video_mode;

long def_table[68]={
  0x2000,  0x38D,   0x788,   0xBE1,   0x10CB,  0x1642,  0x1C48,  0x22F9,  0x2A79,  0x32AA, 0x3C01, 0x4637,
  0x5190,  0x5E0E,  0x5800,  0x9C5,   0x14B8,  0x20C9,  0x2E31,  0x3D39,  0x4DC9,  0x6030, 0x74D1, 0x8B8D,
  0xA509,  0xC160,  0xE054,  0x102AF, 0x10000, 0x1C6A,  0x3C45,  0x5F60,  0x8661,  0xB21A, 0xE249, 0x117D2,
  0x153D5, 0x195F8, 0x1E01C, 0x2328E, 0x28C99, 0x2F08B, 0x8000,  0xE35,   0x1E23,  0x2FB0, 0x4331, 0x590D, 
  0x7125,  0x8BE9,  0xA9EB,  0xCAFC,  0xF00E,  0x11947, 0x1464D, 0x17846, 0x1CCD, -0x2E1,  -0x579, 0x4F33,
  -0x7EB, -0xF0C,   0xE666,  -0x170A, -0x2BC6, 0x7333,  -0xB85, -0x15E3};

long table[68];

void change_video_tables(int a, int b){
 int i;
 for (i=0;i<68;i++) table[i]=(def_table[i]*a)/b; 
}

long CompressionRateTable[]={0x60, 0x5D, 0x5A, 0x57, 0x54, 0x51, 0x4D, 0x48, 0x42, 0x3B, 0x32, 0x29, 0x22, 0x1D, 0x17, 0x14, 0x10, 0xE, 0xB, 9, 7, 6, 5, 4, 3, 2, 1};


void __attribute__((naked,noinline)) movie_record_task(){ 
 asm volatile(
                "STMFD   SP!, {R3-R7,LR}\n"
                "LDR     R4, =0x5868\n"
                "MOV     R6, #0\n"
                "MOV     R5, #1\n"
"loc_FF862E58:\n"
                "LDR     R0, [R4,#0x18]\n"
                "MOV     R2, #0\n"
                "MOV     R1, SP\n"
                "BL      sub_FF82A4F8\n"
                "LDR     R0, [R4,#0xAC]\n"
                "MOV     R1, #0\n"
                "BL      sub_FF81B798\n"
                "LDR     R0, [R4,#0x20]\n"
                "CMP     R0, #0\n"
                "BNE     loc_FF862F34\n"
                "LDR     R0, [SP]\n"
                "LDR     R1, [R0]\n"
                "SUB     R1, R1, #2\n"
                "CMP     R1, #9\n"
                "ADDLS   PC, PC, R1,LSL#2\n"
                "B       loc_FF862F34\n"
"loc_FF862E98:\n"
                "B       loc_FF862EF0\n"
"loc_FF862E9C:\n"
                "B       loc_FF862F08\n"
"loc_FF862EA0:\n"
                "B       loc_FF862F18\n"
"loc_FF862EA4:\n"
                "B       loc_FF862F20\n"
"loc_FF862EA8:\n"
                "B       loc_FF862EF8\n"
"loc_FF862EAC:\n"
                "B       loc_FF862F28\n"
"loc_FF862EB0:\n"
                "B       loc_FF862F00\n"
"loc_FF862EB4:\n"
                "B       loc_FF862F34\n"
"loc_FF862EB8:\n"
                "B       loc_FF862F30\n"
"loc_FF862EBC:\n"
                "B       loc_FF862EC0\n"
"loc_FF862EC0:\n"
                "LDR     R0, =0xFF862AC8\n"
                "STR     R6, [R4,#0x34]\n"
                "STR     R0, [R4,#0xA0]\n"
                "LDR     R0, =0xFF86201C\n"
                "LDR     R2, =0xFF862020\n"
                "STR     R0, [R4,#0xA4]\n"
                "LDR     R1, =0x1A990\n"
                "LDR     R0, =0xFF862124\n"
                "STR     R6, [R4,#0x24]\n"
                "BL      sub_FF84082C\n"
                "STR     R5, [R4,#0x38]\n"
                "B       loc_FF862F34\n"
"loc_FF862EF0:\n"
                "BL      sub_FF862BF0\n"
                "B       loc_FF862F34\n"
"loc_FF862EF8:\n"
                "BL      sub_FF862904_my\n"   //------------->
                "B       loc_FF862F34\n"
"loc_FF862F00:\n"
                "BL      sub_FF86328C\n"
                "B       loc_FF862F34\n"
"loc_FF862F08:\n"
                "LDR     R0, [R4,#0x38]\n"
                "CMP     R0, #5\n"
                "STRNE   R5, [R4,#0x28]\n"
                "B       loc_FF862F44\n"
"loc_FF862F18:\n"
                "BL      sub_FF86274C\n"
                "B       loc_FF862F34\n"
"loc_FF862F20:\n"
                "BL      sub_FF862608\n"
                "B       loc_FF862F34\n"
"loc_FF862F28:\n"
                "BL      sub_FF861FA8\n"
                "B       loc_FF862F34\n"
"loc_FF862F30:\n"
                "BL      sub_FF8631B0\n"
"loc_FF862F34:\n"
                "LDR     R0, [SP]\n"
                "LDR     R1, [R0]\n"
                "CMP     R1, #9\n"
                "BLEQ    sub_FF862498\n"
"loc_FF862F44:\n"
                "LDR     R1, [SP]\n"
                "MOV     R2, #0\n"
                "STR     R6, [R1]\n"
                "LDR     R0, [R4,#0x1C]\n"
                "BL      sub_FF82A620\n"
                "LDR     R0, [R4,#0xAC]\n"
                "BL      sub_FF81B868\n"
                "B       loc_FF862E58\n"
 );
} 


void __attribute__((naked,noinline)) sub_FF862904_my(){ 
 asm volatile(                                   	
                "STMFD   SP!, {R4-R7,LR}\n"
                "SUB     SP, SP, #0x3C\n"
                "MOV     R6, #0\n"
                "LDR     R5, =0x5868\n"
                "MOV     R4, R0\n"
                "STR     R6, [SP,#0x2C]\n"
                "STR     R6, [SP,#0x24]\n"
                "LDR     R0, [R5,#0x38]\n"
                "CMP     R0, #3\n"
                "MOVEQ   R0, #4\n"
                "STREQ   R0, [R5,#0x38]\n"
                "LDR     R0, [R5,#0xA0]\n"
                "BLX     R0\n"
                "LDR     R0, [R5,#0x38]\n"
                "CMP     R0, #4\n"
                "BNE     loc_FF86298C\n"
                "ADD     R3, SP, #0x24\n"
                "ADD     R2, SP, #0x28\n"
                "ADD     R1, SP, #0x2C\n"
                "ADD     R0, SP, #0x30\n"
                "BL      sub_FF95D950\n"
                "CMP     R0, #0\n"
                "BNE     loc_FF86297C\n"
                "LDR     R1, [R5,#0x28]\n"
                "CMP     R1, #1\n"
                "BNE     loc_FF862994\n"
                "LDR     R1, [R5,#0x50]\n"
                "LDR     R2, [R5,#0x3C]\n"
                "CMP     R1, R2\n"
                "BCC     loc_FF862994\n"
"loc_FF86297C:\n"
                "BL      sub_FF862164\n"
                "BL      sub_FF8630A0\n"
                "MOV     R0, #5\n"
                "STR     R0, [R5,#0x38]\n"
"loc_FF86298C:\n"
                "ADD     SP, SP, #0x3C\n"
                "LDMFD   SP!, {R4-R7,PC}\n"
"loc_FF862994:\n"
                "LDR     LR, [SP,#0x2C]\n"
                "CMP     LR, #0\n"
                "BEQ     loc_FF862A70\n"
                "MOV     R7, #1\n"
                "STR     R7, [R5,#0x2C]\n"
                "LDR     R1, [R4,#0x14]\n"
                "LDR     R2, [R4,#0x18]\n"
                "LDR     R12, [R4,#0xC]\n"
                "LDR     R0, [R5,#0x6C]\n"
                "ADD     R3, SP, #0x34\n"
                "STR     R0, [SP,#0x14]\n"
                "STR     R2, [SP,#0x1C]\n"
                "STR     R1, [SP,#0x18]\n"
                "STR     R3, [SP,#0x20]\n"
                "LDR     R3, [R5,#0x58]\n"
                "LDR     R0, [SP,#0x24]\n"
                "LDR     R1, [SP,#0x28]\n"
                "ADD     R2, SP, #0x38\n"
                "STR     R2, [SP,#0xC]\n"
                "STR     R3, [SP,#0x10]\n"
                "STR     R0, [SP,#8]\n"
                "STR     LR, [SP]\n"
                "STR     R1, [SP,#4]\n"
                "LDMIB   R4, {R0,R1}\n"
                "LDR     R3, [SP,#0x30]\n"
                "MOV     R2, R12\n"
                "BL      sub_FF918360\n"
                "LDR     R0, [R5,#0x10]\n"
                "MOV     R1, #0x3E8\n"
                "BL      sub_FF81B798\n"
                "CMP     R0, #9\n"
                "BNE     loc_FF862A24\n"
                "BL      sub_FF95DEEC\n"
                "LDR     R0, =0xFF8625D4\n"
                "STR     R7, [R5,#0x38]\n"
                "B       loc_FF862A3C\n"
"loc_FF862A24:\n"
                "LDR     R0, [SP,#0x34]\n"
                "CMP     R0, #0\n"
                "BEQ     loc_FF862A44\n"
                "BL      sub_FF95DEEC\n"
                "LDR     R0, =0xFF8625E0\n"
                "STR     R7, [R5,#0x38]\n"
"loc_FF862A3C:\n"
                "BL      sub_FF878128\n"
                "B       loc_FF86298C\n"
"loc_FF862A44:\n"
                "BL      sub_FF918424\n"
                "LDR     R0, [SP,#0x30]\n"
                "LDR     R1, [SP,#0x38]\n"
                "BL      sub_FF95DC94\n"
                "LDR     R0, [R5,#0x4C]\n"
                "LDR     R1, =0x58D4\n"
                "ADD     R0, R0, #1\n"
                "STR     R0, [R5,#0x4C]\n"
                "LDR     R0, [SP,#0x38]\n"
                "MOV     R2, #0\n"
                "BL      sub_FF95BA54_my\n"  //---------->
"loc_FF862A70:\n"
                "LDR     R0, [R5,#0x50]\n"
                "ADD     R0, R0, #1\n"
                "STR     R0, [R5,#0x50]\n"
                "LDR     R1, [R5,#0x78]\n"
                "MUL     R0, R1, R0\n"
                "LDR     R1, [R5,#0x74]\n"
                "BL      sub_FFA944C0\n"
                "MOV     R4, R0\n"
                "BL      sub_FF95DF1C\n"
                "LDR     R1, [R5,#0x70]\n"
                "CMP     R1, R4\n"
                "BNE     loc_FF862AAC\n"
                "LDR     R0, [R5,#0x30]\n"
                "CMP     R0, #1\n"
                "BNE     loc_FF862AC0\n"
"loc_FF862AAC:\n"
                "LDR     R1, [R5,#0x84]\n"
                "MOV     R0, R4\n"
                "BLX     R1\n"
                "STR     R4, [R5,#0x70]\n"
                "STR     R6, [R5,#0x30]\n"
"loc_FF862AC0:\n"
                "STR     R6, [R5,#0x2C]\n"
                "B       loc_FF86298C\n"
 );
} 


void __attribute__((naked,noinline)) sub_FF95BA54_my(){ 
 asm volatile(
                "STMFD   SP!, {R4-R9,LR}\n"
                "LDR     R4, =0xB50C\n"
                "LDR     R12, [R4]\n"
                "LDR     R2, [R4,#8]\n"
                "CMP     R12, #0\n"
                "LDRNE   R3, [R4,#0xC]\n"
                "MOV     R5, R2\n"
                "CMPNE   R3, #1\n"
                "MOVEQ   R2, #0\n"
                "STREQ   R0, [R4]\n"
                "STREQ   R2, [R4,#0xC]\n"
                "BEQ     loc_FF95BB24\n"
                "LDR     LR, [R4,#4]\n"
                "LDR     R8, =table\n"         //*
                "RSB     R6, LR, LR,LSL#3\n"
                "LDR     R3, [R8,R6,LSL#3]\n"
                "ADD     LR, LR, LR,LSL#1\n"
                "ADD     R7, R8, #0xE0\n"
                "LDR     R9, [R7,LR,LSL#2]\n"
                "SUB     R3, R12, R3\n"
                "CMP     R3, #0\n"
                "SUB     R12, R12, R9\n"
                "BLE     loc_FF95BAE0\n"
                "ADD     R12, R8, R6,LSL#3\n"
                "LDR     LR, [R12,#4]\n"
                "CMP     LR, R3\n"
                "ADDGE   R2, R2, #1\n"
                "BGE     loc_FF95BAD4\n"
                "LDR     R12, [R12,#8]\n"
                "CMP     R12, R3\n"
                "ADDLT   R2, R2, #3\n"
                "ADDGE   R2, R2, #2\n"
"loc_FF95BAD4:\n"
          //    "CMP     R2, #0x17\n"   // -
          //    "MOVGE   R2, #0x16\n"   // -
                "CMP     R2, #0x1A\n"   // +
                "MOVGE   R2, #0x19\n"   // +
                "B       loc_FF95BB14\n"
"loc_FF95BAE0:\n"
                "CMP     R12, #0\n"
                "BGE     loc_FF95BB14\n"
                "ADD     R3, R7, LR,LSL#2\n"
                "LDR     LR, [R3,#4]\n"
                "CMP     LR, R12\n"
                "SUBLE   R2, R2, #1\n"
                "BLE     loc_FF95BB0C\n"
                "LDR     R3, [R3,#8]\n"
                "CMP     R3, R12\n"
                "SUBGT   R2, R2, #3\n"
                "SUBLE   R2, R2, #2\n"
"loc_FF95BB0C:\n"
                "CMP     R2, #0\n"
                "MOVLT   R2, #0\n"
"loc_FF95BB14:\n"
                "CMP     R2, R5\n"
                "STRNE   R2, [R4,#8]\n"
                "MOVNE   R2, #1\n"
                "STRNE   R2, [R4,#0xC]\n"
"loc_FF95BB24:\n"
                "LDR     R2, =CompressionRateTable\n"   // *
                "LDR     R3, [R4,#8]\n"
                "LDR     R2, [R2,R3,LSL#2]\n"

                "LDR     R3, =video_mode\n"      // +
                "LDR     R3, [R3]\n"             // +
                "LDR     R3, [R3]\n"             // +
                "CMP     R3, #1\n"               // +
                "LDREQ   R3, =video_quality\n"   // +     
                "LDREQ   R3, [R3]\n"             // +     
                "LDREQ   R2, [R3]\n"             // +     

                "STR     R2, [R1]\n"
                "STR     R0, [R4]\n"
                "LDMFD   SP!, {R4-R9,PC}\n"
 );
} 

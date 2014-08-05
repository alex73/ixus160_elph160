/*
 * movie_rec.c - auto-generated by CHDK code_gen.
 */
#include "conf.h"

int *video_quality = &conf.video_quality;
int *video_mode    = &conf.video_mode;

long def_table[24]={0x2000, 0x38D, 0x788, 0x5800, 0x9C5, 0x14B8, 0x10000, 0x1C6A, 0x3C45, 0x8000, 0xE35, 0x1E23,
           0x1CCD, -0x2E1, -0x579, 0x4F33, -0x7EB, -0xF0C, 0xE666, -0x170A, -0x2BC6, 0x7333, -0xB85, -0x15E3};

long table[24];

void change_video_tables(int a, int b){
 int i;
 for (i=0;i<24;i++) table[i]=(def_table[i]*a)/b; 
}

long CompressionRateTable[]={0x60, 0x5D, 0x5A, 0x57, 0x54, 0x51, 0x4D, 0x48, 0x42, 0x3B, 0x32, 0x29, 0x22, 0x1D, 0x17, 0x14, 0x10, 0xE, 0xB, 9, 7, 6, 5, 4, 3, 2, 1};

/*************************************************************/
//** movie_record_task @ 0xFF85821C - 0xFF858320, length=66
void __attribute__((naked,noinline)) movie_record_task() {
asm volatile (
"    STMFD   SP!, {R2-R8,LR} \n"
"    LDR     R8, =0x32B \n"
"    LDR     R7, =0x2710 \n"
"    LDR     R4, =0x54E0 \n"
"    MOV     R6, #0 \n"
"    MOV     R5, #1 \n"

"loc_FF858234:\n"
"    LDR     R0, [R4, #0x1C] \n"
"    MOV     R2, #0 \n"
"    ADD     R1, SP, #4 \n"
"    BL      sub_FF829130 /*_ReceiveMessageQueue*/ \n"
"    LDR     R0, [R4, #0x24] \n"
"    CMP     R0, #0 \n"
"    BNE     loc_FF858304 \n"
"    LDR     R0, [SP, #4] \n"
"    LDR     R1, [R0] \n"
"    SUB     R1, R1, #2 \n"
"    CMP     R1, #9 \n"
"    ADDLS   PC, PC, R1, LSL#2 \n"
"    B       loc_FF858304 \n"
"    B       loc_FF8582B8 \n"
"    B       loc_FF8582D8 \n"
"    B       loc_FF8582E8 \n"
"    B       loc_FF8582F0 \n"
"    B       loc_FF8582C0 \n"
"    B       loc_FF8582F8 \n"
"    B       loc_FF8582C8 \n"
"    B       loc_FF858304 \n"
"    B       loc_FF858300 \n"
"    B       loc_FF858290 \n"

"loc_FF858290:\n"
"    STR     R6, [R4, #0x38] \n"
"    LDR     R0, =0xFF857EF0 \n"
"    LDR     R2, =0xFF857820 \n"
"    LDR     R1, =0x19D84 \n"
"    STR     R0, [R4, #0xA4] \n"
"    LDR     R0, =0xFF857904 \n"
"    STR     R6, [R4, #0x28] \n"
"    BL      sub_FF8C0804 \n"
"    STR     R5, [R4, #0x3C] \n"
"    B       loc_FF858304 \n"

"loc_FF8582B8:\n"
"    BL      unlock_optical_zoom\n"
"    BL      sub_FF857FE8 \n"
"    B       loc_FF858304 \n"

"loc_FF8582C0:\n"
"    BL      sub_FF857CB4_my \n"  // --> Patched. Old value = 0xFF857CB4.
"    B       loc_FF858304 \n"

"loc_FF8582C8:\n"
"    LDR     R1, [R0, #0x10] \n"
"    LDR     R0, [R0, #4] \n"
"    BL      sub_FF92E71C \n"
"    B       loc_FF858304 \n"

"loc_FF8582D8:\n"
"    LDR     R0, [R4, #0x3C] \n"
"    CMP     R0, #5 \n"
"    STRNE   R5, [R4, #0x2C] \n"
"    B       loc_FF858304 \n"

"loc_FF8582E8:\n"
"    BL      sub_FF857AB8 \n"
"    B       loc_FF858304 \n"

"loc_FF8582F0:\n"
"    BL      sub_FF857950 \n"
"    B       loc_FF858304 \n"

"loc_FF8582F8:\n"
"    BL      sub_FF8577AC \n"
"    B       loc_FF858304 \n"

"loc_FF858300:\n"
"    BL      sub_FF85846C \n"

"loc_FF858304:\n"
"    LDR     R1, [SP, #4] \n"
"    LDR     R3, =0xFF8575B0 \n"
"    STR     R6, [R1] \n"
"    STR     R8, [SP] \n"
"    LDR     R0, [R4, #0x20] \n"
"    MOV     R2, R7 \n"
"    BL      sub_FF81BFD0 /*_PostMessageQueueStrictly*/ \n"
"    B       loc_FF858234 \n"
);
}

/*************************************************************/
//** sub_FF857CB4_my @ 0xFF857CB4 - 0xFF857EEC, length=143
void __attribute__((naked,noinline)) sub_FF857CB4_my() {
asm volatile (
"    STMFD   SP!, {R4-R9,LR} \n"
"    SUB     SP, SP, #0x3C \n"
"    MOV     R7, #0 \n"
"    LDR     R5, =0x54E0 \n"
"    MOV     R4, R0 \n"
"    STR     R7, [SP, #0x2C] \n"
"    STR     R7, [SP, #0x24] \n"
"    LDR     R0, [R5, #0x3C] \n"
"    MOV     R8, #4 \n"
"    CMP     R0, #3 \n"
"    STREQ   R8, [R5, #0x3C] \n"
"    LDR     R0, [R5, #0xA4] \n"
"    MOV     R6, #0 \n"
"    BLX     R0 \n"
"    LDR     R0, [R5, #0x3C] \n"
"    CMP     R0, #4 \n"
"    BNE     loc_FF857DC4 \n"
"    LDRH    R0, [R5, #2] \n"
"    MOV     R9, #1 \n"
"    CMP     R0, #1 \n"
"    BNE     loc_FF857D30 \n"
"    LDRH    R1, [R5, #4] \n"
"    LDR     R0, [R5, #0x4C] \n"
"    MUL     R0, R1, R0 \n"
"    MOV     R1, #0x3E8 \n"
"    BL      sub_FFA8D90C /*__divmod_unsigned_int*/ \n"
"    MOV     R1, R0 \n"
"    LDR     R0, [R5, #0x54] \n"
"    BL      sub_FFA8D90C /*__divmod_unsigned_int*/ \n"
"    CMP     R1, #0 \n"
"    BNE     loc_FF857D4C \n"

"loc_FF857D30:\n"
"    ADD     R3, SP, #0x24 \n"
"    ADD     R2, SP, #0x28 \n"
"    ADD     R1, SP, #0x2C \n"
"    ADD     R0, SP, #0x30 \n"
"    BL      sub_FF92E8B0 \n"
"    MOVS    R6, R0 \n"
"    BNE     loc_FF857D68 \n"

"loc_FF857D4C:\n"
"    LDR     R0, [R5, #0x2C] \n"
"    CMP     R0, #1 \n"
"    BNE     loc_FF857DCC \n"
"    LDR     R0, [R5, #0x54] \n"
"    LDR     R1, [R5, #0x40] \n"
"    CMP     R0, R1 \n"
"    BCC     loc_FF857DCC \n"

"loc_FF857D68:\n"
"    CMP     R6, #0x80000001 \n"
"    STREQ   R8, [R5, #0x58] \n"
"    BEQ     loc_FF857DA0 \n"
"    CMP     R6, #0x80000003 \n"
"    STREQ   R9, [R5, #0x58] \n"
"    BEQ     loc_FF857DA0 \n"
"    CMP     R6, #0x80000005 \n"
"    MOVEQ   R0, #2 \n"
"    BEQ     loc_FF857D9C \n"
"    CMP     R6, #0x80000007 \n"
"    STRNE   R7, [R5, #0x58] \n"
"    BNE     loc_FF857DA0 \n"
"    MOV     R0, #3 \n"

"loc_FF857D9C:\n"
"    STR     R0, [R5, #0x58] \n"

"loc_FF857DA0:\n"
"    LDR     R0, =0x19DB4 \n"
"    LDR     R0, [R0, #8] \n"
"    CMP     R0, #0 \n"
"    BEQ     loc_FF857DB8 \n"
"    BL      sub_FF8459F8 \n"
"    B       loc_FF857DBC \n"

"loc_FF857DB8:\n"
"    BL      sub_FF8577AC \n"

"loc_FF857DBC:\n"
"    MOV     R0, #5 \n"
"    STR     R0, [R5, #0x3C] \n"

"loc_FF857DC4:\n"
"    ADD     SP, SP, #0x3C \n"
"    LDMFD   SP!, {R4-R9,PC} \n"

"loc_FF857DCC:\n"
"    LDR     R12, [SP, #0x2C] \n"
"    CMP     R12, #0 \n"
"    BEQ     loc_FF857E98 \n"
"    STR     R9, [R5, #0x30] \n"
"    LDR     R0, [R5, #0x70] \n"
"    LDR     R1, [R4, #0x14] \n"
"    LDR     R2, [R4, #0x18] \n"
"    LDR     LR, [R4, #0xC] \n"
"    ADD     R3, SP, #0x34 \n"
"    ADD     R6, SP, #0x14 \n"
"    STMIA   R6, {R0-R3} \n"
"    LDR     R0, [SP, #0x24] \n"
"    LDR     R3, [R5, #0x5C] \n"
"    LDR     R1, [SP, #0x28] \n"
"    ADD     R2, SP, #0x38 \n"
"    ADD     R6, SP, #8 \n"
"    STMIA   R6, {R0,R2,R3} \n"
"    STR     R1, [SP, #4] \n"
"    STR     R12, [SP] \n"
"    LDMIB   R4, {R0,R1} \n"
"    LDR     R3, [SP, #0x30] \n"
"    MOV     R2, LR \n"
"    BL      sub_FF8EBADC \n"
"    LDR     R0, [R5, #0x14] \n"
"    MOV     R1, #0x3E8 \n"
"    BL      _TakeSemaphore \n"
"    CMP     R0, #9 \n"
"    BNE     loc_FF857E4C \n"
"    BL      sub_FF92EE8C \n"
"    MOV     R0, #0x90000 \n"
"    STR     R9, [R5, #0x3C] \n"
"    B       loc_FF857E64 \n"

"loc_FF857E4C:\n"
"    LDR     R0, [SP, #0x34] \n"
"    CMP     R0, #0 \n"
"    BEQ     loc_FF857E6C \n"
"    BL      sub_FF92EE8C \n"
"    MOV     R0, #0xA0000 \n"
"    STR     R9, [R5, #0x3C] \n"

"loc_FF857E64:\n"
"    BL      _HardwareDefect \n"
"    B       loc_FF857DC4 \n"

"loc_FF857E6C:\n"
"    BL      sub_FF8EBBA0 \n"
"    LDR     R0, [SP, #0x30] \n"
"    LDR     R1, [SP, #0x38] \n"
"    BL      sub_FF92EC34 \n"
"    LDR     R0, [R5, #0x50] \n"
"    LDR     R1, =0x5550 \n"
"    ADD     R0, R0, #1 \n"
"    STR     R0, [R5, #0x50] \n"
"    LDR     R0, [SP, #0x38] \n"
"    MOV     R2, #0 \n"
"    BL      sub_FF92CAB4_my \n"  // --> Patched. Old value = 0xFF92CAB4.

"loc_FF857E98:\n"
"    LDR     R0, [R5, #0x54] \n"
"    ADD     R0, R0, #1 \n"
"    STR     R0, [R5, #0x54] \n"
"    LDR     R1, [R5, #0x7C] \n"
"    MUL     R0, R1, R0 \n"
"    LDR     R1, [R5, #0x78] \n"
"    BL      sub_FFA8D90C /*__divmod_unsigned_int*/ \n"
"    MOV     R4, R0 \n"
"    BL      sub_FF92EEC4 \n"
"    LDR     R1, [R5, #0x74] \n"
"    CMP     R1, R4 \n"
"    BNE     loc_FF857ED4 \n"
"    LDR     R0, [R5, #0x34] \n"
"    CMP     R0, #1 \n"
"    BNE     loc_FF857EE8 \n"

"loc_FF857ED4:\n"
"    LDR     R1, [R5, #0x88] \n"
"    MOV     R0, R4 \n"
"    BLX     R1 \n"
"    STR     R4, [R5, #0x74] \n"
"    STR     R7, [R5, #0x34] \n"

"loc_FF857EE8:\n"
"    STR     R7, [R5, #0x30] \n"
"    B       loc_FF857DC4 \n"
);
}

/*************************************************************/
//** sub_FF92CAB4_my @ 0xFF92CAB4 - 0xFF92CB94, length=57
void __attribute__((naked,noinline)) sub_FF92CAB4_my() {
asm volatile (
"    STMFD   SP!, {R4-R8,LR} \n"
"    LDR     R4, =0xA8EC \n"
"    LDR     LR, [R4] \n"
"    LDR     R2, [R4, #8] \n"
"    CMP     LR, #0 \n"
"    LDRNE   R3, [R4, #0xC] \n"
"    MOV     R5, R2 \n"
"    CMPNE   R3, #1 \n"
"    MOVEQ   R2, #0 \n"
"    STREQ   R0, [R4] \n"
"    STREQ   R2, [R4, #0xC] \n"
"    BEQ     loc_FF92CB80 \n"
"    LDR     R3, [R4, #4] \n"
"    LDR     R7, =table\n"          // +
"    LDR     R7, =0xFFAA8CCC \n"
"    ADD     R12, R3, R3, LSL#1 \n"
"    LDR     R3, [R7, R12, LSL#2] \n"
"    ADD     R6, R7, #0x30 \n"
"    LDR     R8, [R6, R12, LSL#2] \n"
"    SUB     R3, LR, R3 \n"
"    CMP     R3, #0 \n"
"    SUB     LR, LR, R8 \n"
"    BLE     loc_FF92CB3C \n"
"    ADD     R12, R7, R12, LSL#2 \n"
"    LDR     LR, [R12, #4] \n"
"    CMP     LR, R3 \n"
"    ADDGE   R2, R2, #1 \n"
"    BGE     loc_FF92CB30 \n"
"    LDR     R12, [R12, #8] \n"
"    CMP     R12, R3 \n"
"    ADDLT   R2, R2, #3 \n"
"    ADDGE   R2, R2, #2 \n"

"loc_FF92CB30:\n"
"    CMP     R2, #0x1A \n"  // --> Patched. Old value = 0x17.
"    MOVGE   R2, #0x19 \n"  // --> Patched. Old value = 0x16.
"    B       loc_FF92CB70 \n"

"loc_FF92CB3C:\n"
"    CMP     LR, #0 \n"
"    BGE     loc_FF92CB70 \n"
"    ADD     R3, R6, R12, LSL#2 \n"
"    LDR     R12, [R3, #4] \n"
"    CMP     R12, LR \n"
"    SUBLE   R2, R2, #1 \n"
"    BLE     loc_FF92CB68 \n"
"    LDR     R3, [R3, #8] \n"
"    CMP     R3, LR \n"
"    SUBGT   R2, R2, #3 \n"
"    SUBLE   R2, R2, #2 \n"

"loc_FF92CB68:\n"
"    CMP     R2, #0 \n"
"    MOVLT   R2, #0 \n"

"loc_FF92CB70:\n"
"    CMP     R2, R5 \n"
"    STRNE   R2, [R4, #8] \n"
"    MOVNE   R2, #1 \n"
"    STRNE   R2, [R4, #0xC] \n"

"loc_FF92CB80:\n"
"    LDR     R2, =CompressionRateTable \n"  // --> Patched. Old value = 0xFFAA8C70.
"    LDR     R3, [R4, #8] \n"
"    LDR     R2, [R2, R3, LSL#2] \n"
//mod start
"    LDR     R3, =video_mode\n"         // +
"    LDR     R3, [R3]\n"                // +
"    LDR     R3, [R3]\n"                // +
"    CMP     R3, #1\n"                  // +
"    LDREQ   R3, =video_quality\n"      // +
"    LDREQ   R3, [R3]\n"                // +
"    LDREQ   R2, [R3]\n"                // +
//mod end
"    STR     R2, [R1] \n"
"    STR     R0, [R4] \n"
//"    BL      mute_on_zoom\n"            // +
"    LDMFD   SP!, {R4-R8,PC} \n"
);
}

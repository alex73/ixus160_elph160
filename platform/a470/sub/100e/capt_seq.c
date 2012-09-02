#include "lolevel.h"
#include "platform.h"
#include "core.h"

static long *nrflag = (long*)0x8888; //in sub_ffd0cae8, like in 101b

typedef struct {
    unsigned int address;
    unsigned int length;
} cam_ptp_data_chunk; //camera specific structure

#define MAX_CHUNKS_FOR_JPEG 3 //can be model specific (it's either for JPEG or native RAW)

#include "../../../generic/capt_seq.c"

void __attribute__((naked,noinline)) sub_FFD0CAE8_my(){ 
 asm volatile(
                "STMFD   SP!, {R0-R10,LR}\n"
                "MOV     R6, #0\n"
                "MOV     R4, R0\n"
                "BL      sub_FFD0D5E8\n"
                "MVN     R1, #0\n"
                "BL      sub_FFC173EC\n"
                "MOV     R2, #4\n"
                "ADD     R1, SP, #0x30-0x28\n"
                "MOV     R0, #0x8A\n"
                "BL      sub_FFC56F08\n"
                "TST     R0, #1\n"
                "MOVNE   R1, #0x218\n"
                "LDRNE   R0, =0xFFD0CCFC\n"
                "BLNE    sub_FFC0BDB8\n"
                "LDR     R8, =0x1973C\n"
                "LDR     R5, =0x19690\n"
                "LDRSH   R1, [R8,#0xE]\n"
                "LDR     R0, [R5,#0x74]\n"
                "BL      sub_FFCCB1F0\n"
                "BL      sub_FFC32B68\n"
                "LDR     R2, =0x888C\n"
                "ADD     R3, R4, #0x8C\n"
                "STRH    R0, [R4,#0x88]\n"
                "STR     R2, [SP]\n"  //Was "STRD    R2, [SP,#0x30-0x30]\n"
                "STR     R3, [SP,#4]\n" //
                "MOV     R1, R0\n"
                "LDRH    R0, [R5,#0x4C]\n"
                "LDRSH   R2, [R8,#0xC]\n"
                "LDR     R3, =0x8888\n"
                "BL      sub_FFD0DAD4\n"
                "BL      wait_until_remote_button_is_released\n"
                "BL      capt_seq_hook_set_nr\n"
				"BL      sub_FFD0CB5C\n"
 );
}


void __attribute__((naked,noinline)) task_CaptSeqTask_my() //#fs
{
	asm volatile (
                "STMFD   SP!, {R3-R7,LR}\n"
                "LDR     R6, =0x52D0\n"
"loc_FFC488AC:\n"
                "LDR     R0, [R6,#8]\n"
                "MOV     R2, #0\n"
                "MOV     R1, SP\n"
                "BL      sub_FFC1763C\n"
                "TST     R0, #1\n"
                "BEQ     loc_FFC488D8\n"
                "LDR     R1, =0x48E\n"
                "LDR     R0, =0xFFC485CC\n"
                "BL      sub_FFC0BDB8\n"
                "BL      sub_FFC0BB70\n"
                "LDMFD   SP!, {R3-R7,PC}\n"
"loc_FFC488D8:\n"
                //"LDR     R0, [SP,#0x18-0x18]\n"
				"LDR     R0, [SP]\n" //+
                "LDR     R1, [R0]\n"
				
                "CMP     R1, #0x19\n"
                "ADDLS   PC, PC, R1,LSL#2\n"
                "B       loc_FFC48AEC\n"
"loc_FFC488EC:\n"
                "B       loc_FFC48954\n"
"loc_FFC488F0:\n"
                "B       loc_FFC4895C\n"
"loc_FFC488F4:\n"
                "B       loc_FFC489DC\n"
"loc_FFC488F8:\n"
                "B       loc_FFC489F0\n"
"loc_FFC488FC:\n"
                "B       loc_FFC489E8\n"
"loc_FFC48900:\n"
                "B       loc_FFC489F8\n"
"loc_FFC48904:\n"
                "B       loc_FFC48A00\n"
"loc_FFC48908:\n"
                "B       loc_FFC48A0C\n"
"loc_FFC4890C:\n"
                "B       loc_FFC48A64\n"
"loc_FFC48910:\n"
                "B       loc_FFC489F0\n"
"loc_FFC48914:\n"
                "B       loc_FFC48A6C\n"
"loc_FFC48918:\n"
                "B       loc_FFC48A74\n"
"loc_FFC4891C:\n"
                "B       loc_FFC48A7C\n"
"loc_FFC48920:\n"
                "B       loc_FFC48A84\n"
"loc_FFC48924:\n"
                "B       loc_FFC48A8C\n"
"loc_FFC48928:\n"
                "B       loc_FFC48A98\n"
"loc_FFC4892C:\n"
                "B       loc_FFC48AA0\n"
"loc_FFC48930:\n"
                "B       loc_FFC48AA8\n"
"loc_FFC48934:\n"
                "B       loc_FFC48AB0\n"
"loc_FFC48938:\n"
                "B       loc_FFC48ABC\n"
"loc_FFC4893C:\n"
                "B       loc_FFC48AC4\n"
"loc_FFC48940:\n"
                "B       loc_FFC48ACC\n"
"loc_FFC48944:\n"
                "B       loc_FFC48AD4\n"
"loc_FFC48948:\n"
                "B       loc_FFC48ADC\n"
"loc_FFC4894C:\n"
                "B       loc_FFC48AE4\n"
"loc_FFC48950:\n"
                "B       loc_FFC48AF8\n"
"loc_FFC48954:\n" //1
                "BL      sub_FFD0B650\n"
				"BL      shooting_expo_param_override\n"  // +
                "B       loc_FFC48A04\n"
"loc_FFC4895C:\n" //2
                "LDR     R4, [R0,#0xC]\n"
                "LDR     R0, [R4,#8]\n"
                "ORR     R0, R0, #1\n"
                "STR     R0, [R4,#8]\n"
                "BL      sub_FFD0B640\n"
                "MOV     R0, R4\n"
                "BL      sub_FFD0BA28\n"
                "TST     R0, #1\n"
                "MOVNE   R2, R4\n"
                "MOVNE   R1, #1\n"
                "BNE     loc_FFC48A5C\n"
                "BL      sub_FFD2A16C\n"
                "BL      sub_FFC57098\n"
                "STR     R0, [R4,#0x14]\n"
                "MOV     R0, R4\n"
                "BL      sub_FFD0CA20\n"
                "BL      sub_FFD0D484\n"
                "MOV     R0, R4\n"
                "BL      sub_FFD0CAE8_my\n"  //-------------->
				"BL      capt_seq_hook_raw_here\n"  //-------------->
                "MOV     R5, R0\n"
                "BL      sub_FFD0E464\n"
                "BL      sub_FFD0E4A0\n"
                "MOV     R2, R4\n"
                "MOV     R1, #1\n"
                "MOV     R0, R5\n"
                "BL      sub_FFC47080\n"
                "BL      sub_FFD0CE98\n"
                "CMP     R0, #0\n"
                "LDRNE   R0, [R4,#8]\n"
                "ORRNE   R0, R0, #0x2000\n"
                "STRNE   R0, [R4,#8]\n"
                "B       loc_FFC48AF8\n"
"loc_FFC489DC:\n"
                "MOV     R0, #1\n"
                "BL      sub_FFD0B7E4\n"
                "B       loc_FFC48AF8\n"
"loc_FFC489E8:\n"
                "BL      sub_FFD0B2B0\n"
                "B       loc_FFC48AF8\n"
"loc_FFC489F0:\n"
                "BL      sub_FFD0B630\n"
                "B       loc_FFC48AF8\n"
"loc_FFC489F8:\n"
                "BL      sub_FFD0B638\n"
                "B       loc_FFC48AF8\n"
"loc_FFC48A00:\n"
                "BL      sub_FFD0B704\n"
"loc_FFC48A04:\n"
                "BL      sub_FFC46D00\n"
                "B       loc_FFC48AF8\n"
"loc_FFC48A0C:\n"
                "LDR     R4, [R0,#0xC]\n"
                "BL      sub_FFD0B640\n"
                "MOV     R0, R4\n"
                "BL      sub_FFD0BDA8\n"
                "TST     R0, #1\n"
                "MOV     R5, R0\n"
                "BNE     loc_FFC48A4C\n"
                "BL      sub_FFC57098\n"
                "STR     R0, [R4,#0x14]\n"
                "MOV     R0, R4\n"
                "BL      sub_FFD0CA20\n"
                "MOV     R0, R4\n"
                "BL      sub_FFD0CEF8\n"
                "MOV     R5, R0\n"
                "LDR     R0, [R4,#0x14]\n"
                "BL      sub_FFC572A4\n"
"loc_FFC48A4C:\n"
                "BL      sub_FFD0B630\n"
                "MOV     R2, R4\n"
                "MOV     R1, #9\n"
                "MOV     R0, R5\n"
"loc_FFC48A5C:\n"
                "BL      sub_FFC47080\n"
                "B       loc_FFC48AF8\n"
"loc_FFC48A64:\n"
                "BL      sub_FFD0B764\n"
                "B       loc_FFC48A04\n"
"loc_FFC48A6C:\n"
                "BL      sub_FFD0C024\n"
                "B       loc_FFC48AF8\n"
"loc_FFC48A74:\n"
                "BL      sub_FFD0C20C\n"
                "B       loc_FFC48AF8\n"
"loc_FFC48A7C:\n"
                "BL      sub_FFD0C29C\n"
                "B       loc_FFC48AF8\n"
"loc_FFC48A84:\n"
                "BL      sub_FFD0C350\n"
                "B       loc_FFC48AF8\n"
"loc_FFC48A8C:\n"
                "MOV     R0, #0\n"
                "BL      sub_FFD0C4F4\n"
                "B       loc_FFC48AF8\n"
"loc_FFC48A98:\n"
                "BL      sub_FFD0C644\n"
                "B       loc_FFC48AF8\n"
"loc_FFC48AA0:\n"
                "BL      sub_FFD0C6D8\n"
                "B       loc_FFC48AF8\n"
"loc_FFC48AA8:\n"
                "BL      sub_FFD0C7A0\n"
                "B       loc_FFC48AF8\n"
"loc_FFC48AB0:\n"
                "BL      sub_FFD0B900\n"
                "BL      sub_FFC149A0\n"
                "B       loc_FFC48AF8\n"
"loc_FFC48ABC:\n"
                "BL      sub_FFD0C40C\n"
                "B       loc_FFC48AF8\n"
"loc_FFC48AC4:\n"
                "BL      sub_FFD0C450\n"
                "B       loc_FFC48AF8\n"
"loc_FFC48ACC:\n"
                "BL      sub_FFD0E448\n"
                "B       loc_FFC48AF8\n"
"loc_FFC48AD4:\n"
                "BL      sub_FFD0E464\n"
                "B       loc_FFC48AF8\n"
"loc_FFC48ADC:\n"
                "BL      sub_FFD0E474\n"
                "B       loc_FFC48AF8\n"
"loc_FFC48AE4:\n"
                "BL      sub_FFD0E4A0\n"
                "B       loc_FFC48AF8\n"
"loc_FFC48AEC:\n"
                "LDR     R1, =0x58E\n"
                "LDR     R0, =0xFFC485CC\n"
                "BL      sub_FFC0BDB8\n"
"loc_FFC48AF8:\n"
                //"LDR     R0, [SP,#0x18-0x18]\n"
				"LDR     R0, [SP]\n" //+
                "LDR     R1, [R0,#4]\n"
                "LDR     R0, [R6,#4]\n"
                "BL      sub_FFC173B8\n"
                //"LDR     R4, [SP,#0x18-0x18]\n"
				"LDR     R4, [SP]\n" //+
                "LDR     R0, [R4,#8]\n"
                "CMP     R0, #0\n"
                "LDREQ   R1, =0x10D\n"
                "LDREQ   R0, =0xFFC485CC\n"
                "BLEQ    sub_FFC0BDB8\n"
                "MOV     R0, #0\n"
                "STR     R0, [R4,#8]\n"
                "B       loc_FFC488AC\n"
    );
} //#fe


void __attribute__((naked,noinline)) exp_drv_task()
{
        asm volatile (
"       STMFD   SP!, {R4-R8,LR}\n"
"       SUB     SP, SP, #0x20\n"
"       LDR     R8, =0xBB8\n"
"       LDR     R7, =0x64D4\n"
"       LDR     R5, =0x3D3DC\n"
"       MOV     R0, #0\n"
"       ADD     R6, SP, #0x10\n"
"       STR     R0, [SP,#0xC]\n"
"loc_FFC900D8:\n"
"       LDR     R0, [R7,#0x20]\n"
"       MOV     R2, #0\n"
"       ADD     R1, SP, #0x1C\n"
"       BL      sub_FFC1763C\n"
"       LDR     R0, [SP,#0xC]\n"
"       CMP     R0, #1\n"
"       BNE     loc_FFC90120\n"
"       LDR     R0, [SP,#0x1C]\n"
"       LDR     R0, [R0]\n"
"       CMP     R0, #0x13\n"
"       CMPNE   R0, #0x14\n"
"       CMPNE   R0, #0x15\n"
"       BEQ     loc_FFC9023C\n"
"       CMP     R0, #0x27\n"
"       BEQ     loc_FFC90214\n"
"       ADD     R1, SP, #0xC\n"
"       MOV     R0, #0\n"
"       BL      sub_FFC90068\n"
"loc_FFC90120:\n"
"       LDR     R0, [SP,#0x1C]\n"
"       LDR     R1, [R0]\n"
"       CMP     R1, #0x2C\n"
"       BNE     loc_FFC90150\n"
"       LDR     R0, [SP,#0x1C]\n"
"       BL      sub_FFC9131C\n"
"       LDR     R0, [R7,#0x1C]\n"
"       MOV     R1, #1\n"
"       BL      sub_FFC173B8\n"
"       BL      sub_FFC0BB70\n"
"       ADD     SP, SP, #0x20\n"
"       LDMFD   SP!, {R4-R8,PC}\n"
"loc_FFC90150:\n"
"       CMP     R1, #0x2B\n"
"       BNE     loc_FFC9016C\n"
"       LDR     R2, [R0,#0x88]!\n"
"       LDR     R1, [R0,#4]\n"
"       MOV     R0, R1\n"
"       BLX     R2\n"
"       B       loc_FFC906A0\n"
"loc_FFC9016C:\n"
"       CMP     R1, #0x25\n"
"       BNE     loc_FFC901BC\n"
"       LDR     R0, [R7,#0x1C]\n"
"       MOV     R1, #0x80\n"
"       BL      sub_FFC173EC\n"
"       LDR     R0, =0xFFC8CB5C\n"
"       MOV     R1, #0x80\n"
"       BL      sub_FFD02FE0\n"
"       LDR     R0, [R7,#0x1C]\n"
"       MOV     R2, R8\n"
"       MOV     R1, #0x80\n"
"       BL      sub_FFC172F0\n"
"       TST     R0, #1\n"
"       LDRNE   R1, =0xD1B\n"
"       BNE     loc_FFC90200\n"
"loc_FFC901A8:\n"
"       LDR     R1, [SP,#0x1C]\n"
"       LDR     R0, [R1,#0x8C]\n"
"       LDR     R1, [R1,#0x88]\n"
"       BLX     R1\n"
"       B       loc_FFC906A0\n"
"loc_FFC901BC:\n"
"       CMP     R1, #0x26\n"
"       BNE     loc_FFC9020C\n"
"       ADD     R1, SP, #0xC\n"
"       BL      sub_FFC90068\n"
"       LDR     R0, [R7,#0x1C]\n"
"       MOV     R1, #0x100\n"
"       BL      sub_FFC173EC\n"
"       LDR     R0, =0xFFC8CB6C\n"
"       MOV     R1, #0x100\n"
"       BL      sub_FFD03268\n"
"       LDR     R0, [R7,#0x1C]\n"
"       MOV     R2, R8\n"
"       MOV     R1, #0x100\n"
"       BL      sub_FFC172F0\n"
"       TST     R0, #1\n"
"       BEQ     loc_FFC901A8\n"
"       LDR     R1, =0xD25\n"
"loc_FFC90200:\n"
"       LDR     R0, =0xFFC8D25C\n"
"       BL      sub_FFC0BDB8\n"
"       B       loc_FFC901A8\n"
"loc_FFC9020C:\n"
"       CMP     R1, #0x27\n"
"       BNE     loc_FFC90224\n"
"loc_FFC90214:\n"
"       LDR     R0, [SP,#0x1C]\n"
"       ADD     R1, SP, #0xC\n"
"       BL      sub_FFC90068\n"
"       B       loc_FFC901A8\n"
"loc_FFC90224:\n"
"       CMP     R1, #0x2A\n"
"       BNE     loc_FFC9023C\n"
"       BL      sub_FFC6FF10\n"
"       BL      sub_FFC70CDC\n"
"       BL      sub_FFC70760\n"
"       B       loc_FFC901A8\n"
"loc_FFC9023C:\n"
"       LDR     R0, [SP,#0x1C]\n"
"       MOV     R4, #1\n"
"       LDR     R1, [R0]\n"
"       CMP     R1, #0x11\n"
"       CMPNE   R1, #0x12\n"
"       BNE     loc_FFC902AC\n"
"       LDR     R1, [R0,#0x7C]\n"
"       ADD     R1, R1, R1,LSL#1\n"
"       ADD     R1, R0, R1,LSL#2\n"
"       SUB     R1, R1, #8\n"
"       LDMIA   R1, {R2-R4}\n"
"       STMIA   R6, {R2-R4}\n"
"       BL      sub_FFC8EBE8\n"
"       LDR     R0, [SP,#0x1C]\n"
"       LDR     R1, [R0,#0x7C]\n"
"       LDR     R3, [R0,#0x88]\n"
"       LDR     R2, [R0,#0x8C]\n"
"       ADD     R0, R0, #4\n"
"       BLX     R3\n"
"       LDR     R0, [SP,#0x1C]\n"
"       BL      sub_FFC916F4\n"
"       LDR     R0, [SP,#0x1C]\n"
"       LDR     R1, [R0,#0x7C]\n"
"       LDR     R3, [R0,#0x90]\n"
"       LDR     R2, [R0,#0x94]\n"
"       ADD     R0, R0, #4\n"
"       BLX     R3\n"
"       B       loc_FFC905E0\n"
"loc_FFC902AC:\n"
"       CMP     R1, #0x13\n"
"       CMPNE   R1, #0x14\n"
"       CMPNE   R1, #0x15\n"
"       BNE     loc_FFC90360\n"
"       ADD     R3, SP, #0xC\n"
"       MOV     R2, SP\n"
"       ADD     R1, SP, #0x10\n"
"       BL      sub_FFC8EE30\n"
"       CMP     R0, #1\n"
"       MOV     R4, R0\n"
"       CMPNE   R4, #5\n"
"       BNE     loc_FFC902FC\n"
"       LDR     R0, [SP,#0x1C]\n"
"       MOV     R2, R4\n"
"       LDR     R1, [R0,#0x7C]!\n"
"       LDR     R12, [R0,#0xC]!\n"
"       LDR     R3, [R0,#4]\n"
"       MOV     R0, SP\n"
"       BLX     R12\n"
"       B       loc_FFC90334\n"
"loc_FFC902FC:\n"
"       LDR     R0, [SP,#0x1C]\n"
"       CMP     R4, #2\n"
"       LDR     R3, [R0,#0x8C]\n"
"       CMPNE   R4, #6\n"
"       BNE     loc_FFC90348\n"
"       LDR     R12, [R0,#0x88]\n"
"       MOV     R0, SP\n"
"       MOV     R2, R4\n"
"       MOV     R1, #1\n"
"       BLX     R12\n"
"       LDR     R0, [SP,#0x1C]\n"
"       MOV     R2, SP\n"
"       ADD     R1, SP, #0x10\n"
"       BL      sub_FFC8FDB4\n"
"loc_FFC90334:\n"
"       LDR     R0, [SP,#0x1C]\n"
"       LDR     R2, [SP,#0xC]\n"
"       MOV     R1, R4\n"
"       BL      sub_FFC90008\n"
"       B       loc_FFC905E0\n"
"loc_FFC90348:\n"
"       LDR     R1, [R0,#0x7C]\n"
"       LDR     R12, [R0,#0x88]\n"
"       ADD     R0, R0, #4\n"
"       MOV     R2, R4\n"
"       BLX     R12\n"
"       B       loc_FFC905E0\n"
"loc_FFC90360:\n"
"       CMP     R1, #0x21\n"
"       CMPNE   R1, #0x22\n"
"       BNE     loc_FFC903AC\n"
"       LDR     R1, [R0,#0x7C]\n"
"       ADD     R1, R1, R1,LSL#1\n"
"       ADD     R1, R0, R1,LSL#2\n"
"       SUB     R1, R1, #8\n"
"       LDMIA   R1, {R2-R4}\n"
"       STMIA   R6, {R2-R4}\n"
"       BL      sub_FFC8E174\n"
"       LDR     R0, [SP,#0x1C]\n"
"       LDR     R1, [R0,#0x7C]\n"
"       LDR     R3, [R0,#0x88]\n"
"       LDR     R2, [R0,#0x8C]\n"
"       ADD     R0, R0, #4\n"
"       BLX     R3\n"
"       LDR     R0, [SP,#0x1C]\n"
"       BL      sub_FFC8E464\n"
"       B       loc_FFC905E0\n"
"loc_FFC903AC:\n"
"       ADD     R1, R0, #4\n"
"       LDMIA   R1, {R2,R3,R12}\n"
"       STMIA   R6, {R2,R3,R12}\n"
"       LDR     R1, [R0]\n"
"       CMP     R1, #0x24\n"
"       ADDLS   PC, PC, R1,LSL#2\n"
"       B       loc_FFC905C0\n"
"loc_FFC903C8:\n"
"       B       loc_FFC9045C\n"
"loc_FFC903CC:\n"
"       B       loc_FFC9045C\n"
"loc_FFC903D0:\n"
"       B       loc_FFC90464\n"
"loc_FFC903D4:\n"
"       B       loc_FFC9046C\n"
"loc_FFC903D8:\n"
"       B       loc_FFC9046C\n"
"loc_FFC903DC:\n"
"       B       loc_FFC9046C\n"
"loc_FFC903E0:\n"
"       B       loc_FFC9045C\n"
"loc_FFC903E4:\n"
"       B       loc_FFC90464\n"
"loc_FFC903E8:\n"
"       B       loc_FFC9046C\n"
"loc_FFC903EC:\n"
"       B       loc_FFC9046C\n"
"loc_FFC903F0:\n"
"       B       loc_FFC904C8\n"
"loc_FFC903F4:\n"
"       B       loc_FFC904C8\n"
"loc_FFC903F8:\n"
"       B       loc_FFC905B4\n"
"loc_FFC903FC:\n"
"       B       loc_FFC905BC\n"
"loc_FFC90400:\n"
"       B       loc_FFC905BC\n"
"loc_FFC90404:\n"
"       B       loc_FFC905BC\n"
"loc_FFC90408:\n"
"       B       loc_FFC905BC\n"
"loc_FFC9040C:\n"
"       B       loc_FFC905C0\n"
"loc_FFC90410:\n"
"       B       loc_FFC905C0\n"
"loc_FFC90414:\n"
"       B       loc_FFC905C0\n"
"loc_FFC90418:\n"
"       B       loc_FFC905C0\n"
"loc_FFC9041C:\n"
"       B       loc_FFC905C0\n"
"loc_FFC90420:\n"
"       B       loc_FFC90474\n"
"loc_FFC90424:\n"
"       B       loc_FFC9047C\n"
"loc_FFC90428:\n"
"       B       loc_FFC9047C\n"
"loc_FFC9042C:\n"
"       B       loc_FFC904D4\n"
"loc_FFC90430:\n"
"       B       loc_FFC904D4\n"
"loc_FFC90434:\n"
"       B       loc_FFC904DC\n"
"loc_FFC90438:\n"
"       B       loc_FFC9050C\n"
"loc_FFC9043C:\n"
"       B       loc_FFC9053C\n"
"loc_FFC90440:\n"
"       B       loc_FFC9056C\n"
"loc_FFC90444:\n"
"       B       loc_FFC9059C\n"
"loc_FFC90448:\n"
"       B       loc_FFC9059C\n"
"loc_FFC9044C:\n"
"       B       loc_FFC905C0\n"
"loc_FFC90450:\n"
"       B       loc_FFC905C0\n"
"loc_FFC90454:\n"
"       B       loc_FFC905A4\n"
"loc_FFC90458:\n"
"       B       loc_FFC905AC\n"
"loc_FFC9045C:\n"
"       BL      sub_FFC8D044\n"
"       B       loc_FFC905C0\n"
"loc_FFC90464:\n"
"       BL      sub_FFC8D2D4\n"
"       B       loc_FFC905C0\n"
"loc_FFC9046C:\n"
"       BL      sub_FFC8D4D8\n"
"       B       loc_FFC905C0\n"
"loc_FFC90474:\n"
"       BL      sub_FFC8D740\n"
"       B       loc_FFC905C0\n"
"loc_FFC9047C:\n"
"       BL      sub_FFC8D934\n"
"       B       loc_FFC905C0\n"
"loc_FFC904C8:\n"
"       BL      sub_FFC8DB9C_my\n" // ->
"       MOV     R4, #0\n"
"       B       loc_FFC905C0\n"
"loc_FFC904D4:\n"
"       BL      sub_FFC8DCD8\n"
"       B       loc_FFC905C0\n"
"loc_FFC904DC:\n"
"       LDRH    R1, [R0,#4]\n"
"       STRH    R1, [SP,#0x10]\n"
"       LDRH    R1, [R5,#2]\n"
"       STRH    R1, [SP,#0x12]\n"
"       LDRH    R1, [R5,#4]\n"
"       STRH    R1, [SP,#0x14]\n"
"       LDRH    R1, [R5,#6]\n"
"       STRH    R1, [SP,#0x16]\n"
"       LDRH    R1, [R0,#0xC]\n"
"       STRH    R1, [SP,#0x18]\n"
"       BL      sub_FFC91390\n"
"       B       loc_FFC905C0\n"
"loc_FFC9050C:\n"
"       LDRH    R1, [R0,#4]\n"
"       STRH    R1, [SP,#0x10]\n"
"       LDRH    R1, [R5,#2]\n"
"       STRH    R1, [SP,#0x12]\n"
"       LDRH    R1, [R5,#4]\n"
"       STRH    R1, [SP,#0x14]\n"
"       LDRH    R1, [R5,#6]\n"
"       STRH    R1, [SP,#0x16]\n"
"       LDRH    R1, [R5,#8]\n"
"       STRH    R1, [SP,#0x18]\n"
"       BL      sub_FFC91510\n"
"       B       loc_FFC905C0\n"
"loc_FFC9053C:\n"
"       LDRH    R1, [R5]\n"
"       STRH    R1, [SP,#0x10]\n"
"       LDRH    R1, [R0,#6]\n"
"       STRH    R1, [SP,#0x12]\n"
"       LDRH    R1, [R5,#4]\n"
"       STRH    R1, [SP,#0x14]\n"
"       LDRH    R1, [R5,#6]\n"
"       STRH    R1, [SP,#0x16]\n"
"       LDRH    R1, [R5,#8]\n"
"       STRH    R1, [SP,#0x18]\n"
"       BL      sub_FFC915BC\n"
"       B       loc_FFC905C0\n"
"loc_FFC9056C:\n"
"       LDRH    R1, [R5]\n"
"       STRH    R1, [SP,#0x10]\n"
"       LDRH    R1, [R5,#2]\n"
"       STRH    R1, [SP,#0x12]\n"
"       LDRH    R1, [R5,#4]\n"
"       STRH    R1, [SP,#0x14]\n"
"       LDRH    R1, [R5,#6]\n"
"       STRH    R1, [SP,#0x16]\n"
"       LDRH    R1, [R0,#0xC]\n"
"       STRH    R1, [SP,#0x18]\n"
"       BL      sub_FFC9165C\n"
"       B       loc_FFC905C0\n"
"loc_FFC9059C:\n"
"       BL      sub_FFC8DF4C\n"
"       B       loc_FFC905C0\n"
"loc_FFC905A4:\n"
"       BL      sub_FFC8E568\n"
"       B       loc_FFC905C0\n"
"loc_FFC905AC:\n"
"       BL      sub_FFC8E7A0\n"
"       B       loc_FFC905C0\n"
"loc_FFC905B4:\n"
"       BL      sub_FFC8E918\n"
"       B       loc_FFC905C0\n"
"loc_FFC905BC:\n"
"       BL      sub_FFC8EAB0\n"
"loc_FFC905C0:\n"
"       LDR     R0, [SP,#0x1C]\n"
"       LDR     R1, [R0,#0x7C]\n"
"       LDR     R3, [R0,#0x88]\n"
"       LDR     R2, [R0,#0x8C]\n"
"       ADD     R0, R0, #4\n"
"       BLX     R3\n"
"       CMP     R4, #1\n"
"       BNE     loc_FFC90628\n"
"loc_FFC905E0:\n"
"       LDR     R0, [SP,#0x1C]\n"
"       MOV     R2, #0xC\n"
"       LDR     R1, [R0,#0x7C]\n"
"       ADD     R1, R1, R1,LSL#1\n"
"       ADD     R0, R0, R1,LSL#2\n"
"       SUB     R4, R0, #8\n"
"       LDR     R0, =0x3D3DC\n"
"       ADD     R1, SP, #0x10\n"
"       BL      sub_FFE4E008\n"
"       LDR     R0, =0x3D3E8\n"
"       MOV     R2, #0xC\n"
"       ADD     R1, SP, #0x10\n"
"       BL      sub_FFE4E008\n"
"       LDR     R0, =0x3D3F4\n"
"       MOV     R2, #0xC\n"
"       MOV     R1, R4\n"
"       BL      sub_FFE4E008\n"
"       B       loc_FFC906A0\n"
"loc_FFC90628:\n"
"       LDR     R0, [SP,#0x1C]\n"
"       LDR     R0, [R0]\n"
"       CMP     R0, #0xB\n"
"       BNE     loc_FFC90670\n"
"       MOV     R3, #0\n"
"       STR     R3, [SP]\n"
"       MOV     R3, #1\n"
"       MOV     R2, #1\n"
"       MOV     R1, #1\n"
"       MOV     R0, #0\n"
"       BL      sub_FFC8CE4C\n"
"       MOV     R3, #0\n"
"       STR     R3, [SP]\n"
"       MOV     R3, #1\n"
"       MOV     R2, #1\n"
"       MOV     R1, #1\n"
"       MOV     R0, #0\n"
"       B       loc_FFC9069C\n"
"loc_FFC90670:\n"
"       MOV     R3, #1\n"
"       MOV     R2, #1\n"
"       MOV     R1, #1\n"
"       MOV     R0, #1\n"
"       STR     R3, [SP]\n"
"       BL      sub_FFC8CE4C\n"
"       MOV     R3, #1\n"
"       MOV     R2, #1\n"
"       MOV     R1, #1\n"
"       MOV     R0, #1\n"
"       STR     R3, [SP]\n"
"loc_FFC9069C:\n"
"       BL      sub_FFC8CF8C\n"
"loc_FFC906A0:\n"
"       LDR     R0, [SP,#0x1C]\n"
"       BL      sub_FFC9131C\n"
"       B       loc_FFC900D8\n"
        );
}


void __attribute__((naked,noinline)) sub_FFC8DB9C_my()
{
        asm volatile (
"       STMFD   SP!, {R4-R8,LR}\n"
"       LDR     R7, =0x64D4\n"
"       MOV     R4, R0\n"
"       LDR     R0, [R7,#0x1C]\n"
"       MOV     R1, #0x3E\n"
"       BL      sub_FFC173EC\n"
"       LDRSH   R0, [R4,#4]\n"
"       MOV     R2, #0\n"
"       MOV     R1, #0\n"
"       BL      sub_FFC8CBE0\n"
"       MOV     R6, R0\n"
"       LDRSH   R0, [R4,#6]\n"
"       BL      sub_FFC8CCEC\n"
"       LDRSH   R0, [R4,#8]\n"
"       BL      sub_FFC8CD44\n"
"       LDRSH   R0, [R4,#0xA]\n"
"       BL      sub_FFC8CD9C\n"
"       LDRSH   R0, [R4,#0xC]\n"
"       BL      sub_FFC8CDF4\n"
"       MOV     R5, R0\n"
"       LDR     R0, [R4]\n"
"       LDR     R8, =0x3D3F4\n"
"       CMP     R0, #0xB\n"
"       MOVEQ   R6, #0\n"
"       MOVEQ   R5, #0\n"
"       BEQ     loc_FFC8DC2C\n"
"       CMP     R6, #1\n"
"       BNE     loc_FFC8DC2C\n"
"       LDRSH   R0, [R4,#4]\n"
"       LDR     R1, =0xFFC8CB4C\n"
"       MOV     R2, #2\n"
"       BL      sub_FFD03134\n"
"       STRH    R0, [R4,#4]\n"
"       MOV     R0, #0\n"
"       STR     R0, [R7,#0x28]\n"
"       B       loc_FFC8DC34\n"
"loc_FFC8DC2C:\n"
"       LDRH    R0, [R8]\n"
"       STRH    R0, [R4,#4]\n"
"loc_FFC8DC34:\n"
"       CMP     R5, #1\n"
"       LDRNEH  R0, [R8,#8]\n"
"       BNE     loc_FFC8DC50\n"
"       LDRSH   R0, [R4,#0xC]\n"
"       MOV     R2, #0x20\n"
"       LDR     R1, =0xFFC8CBD0\n"
"       BL      sub_FFC9134C\n"
"loc_FFC8DC50:\n"
"       STRH    R0, [R4,#0xC]\n"
"       LDRSH   R0, [R4,#6]\n"
"       BL      sub_FFC6FC5C_my\n" // ->
"       LDR     PC, =0xFFC8DC5C\n" // + continue in ROM
/* removed to spare some bytes -
"       LDRSH   R0, [R4,#8]\n"
"       MOV     R1, #1\n"
"       BL      sub_FFC70458\n"
"       MOV     R1, #0\n"
"       ADD     R0, R4, #8\n"
"       BL      sub_FFC704E0\n"
"       LDRSH   R0, [R4,#0xE]\n"
"       BL      sub_FFC83B1C\n"
"       LDR     R4, =0xBB8\n"
"       CMP     R6, #1\n"
"       BNE     loc_FFC8DCA8\n"
"       LDR     R0, [R7,#0x1C]\n"
"       MOV     R2, R4\n"
"       MOV     R1, #2\n"
"       BL      sub_FFC172F0\n"
"       TST     R0, #1\n"
"       LDRNE   R1, =0x532\n"
"       LDRNE   R0, =0xffc8d25c\n" // "ExpDrv.c"
"       BLNE    sub_FFC0BDB8\n"
"loc_FFC8DCA8:\n"
"       CMP     R5, #1\n"
"       LDMNEFD SP!, {R4-R8,PC}\n"
"       LDR     R0, [R7,#0x1C]\n"
"       MOV     R2, R4\n"
"       MOV     R1, #0x20\n"
"       BL      sub_FFC172F0\n"
"       TST     R0, #1\n"
"       LDRNE   R1, =0x537\n"
"       LDRNE   R0, =0xffc8d25c\n" // "ExpDrv.c"
"       LDMNEFD SP!, {R4-R8,LR}\n"
"       BNE     sub_FFC0BDB8\n"
"       LDMFD   SP!, {R4-R8,PC}\n"
*/
        );
}


void __attribute__((naked,noinline)) sub_FFC6FC5C_my()
{
        asm volatile (
"       STMFD   SP!, {R4-R6,LR}\n"
"       LDR     R5, =0x5FAC\n"
"       MOV     R4, R0\n"
"       LDR     R0, [R5,#4]\n"
"       CMP     R0, #1\n"
"       LDRNE   R1, =0x16D\n"
"       LDRNE   R0, =0xffc6f9f4\n" // "Shutter.c"
"       BLNE    sub_FFC0BDB8\n"
"       CMN     R4, #0xC00\n"
"       LDREQSH R4, [R5,#2]\n"
"       CMN     R4, #0xC00\n"
"       LDREQ   R1, =0x173\n"
"       LDREQ   R0, =0xffc6f9f4\n" // "Shutter.c"
"       STRH    R4, [R5,#2]\n"
"       BLEQ    sub_FFC0BDB8\n"
"       MOV     R0, R4\n"
//"       BL      sub_FFD68398\n" //-
"       BL      apex2us\n" //+
"       MOV     R4, R0\n"
//"       BL      nullsub\n" //-
//"       MOV     R0, R4\n" //-
"       BL      sub_FFCA25DC\n"
"       TST     R0, #1\n"
"       LDMNEFD SP!, {R4-R6,LR}\n"
"       MOVNE   R1, #0x178\n"
"       LDRNE   R0, =0xffc6f9f4\n" // "Shutter.c"
"       BNE     sub_FFC0BDB8\n"
"       LDMFD   SP!, {R4-R6,PC}\n"
        );
}

//filewritetask


void __attribute__((naked,noinline)) filewritetask(  ) { //0xFFDC0EA8
asm volatile (
      "    STMFD   SP!, {R1-R5,LR} \n" 
      "    LDR     R4, =0xC078 \n" 
"loc_FFDC0EB0:\n"
      "    LDR     R0, [R4, #0x10] \n" 
      "    MOV     R2, #0 \n" 
      "    ADD     R1, SP, #8 \n" 
      "    BL      sub_FFC1763C \n" 
      "    CMP     R0, #0 \n" 
      "    BNE     loc_FFDC0EE0 \n" 
      "    LDR     R0, [SP, #8] \n" 
      "    LDR     R1, [R0] \n" 
      "    CMP     R1, #1 \n" 
      "    BNE     loc_FFDC0EE8 \n" 
      "    LDR     R0, [R4, #8] \n" 
      "    BL      sub_FFC0B844 \n" 
"loc_FFDC0EE0:\n"
      "    BL      sub_FFC0BB70 \n" 
      "    LDMFD   SP!, {R1-R5,PC} \n" 
"loc_FFDC0EE8:\n"
      "    SUB     R1, R1, #2 \n" 
      "    CMP     R1, #5 \n" 
      "    ADDLS   PC, PC, R1, LSL #2 \n" 
      "    B       loc_FFDC0EB0 \n" 
      "    B       loc_FFDC0F10 \n" 
      "    B       loc_FFDC0F74 \n" 
      "    B       loc_FFDC0F7C \n" 
      "    B       loc_FFDC0F7C \n" 
      "    B       loc_FFDC0F7C \n" 
      "    B       loc_FFDC0F84 \n" 
"loc_FFDC0F10:\n"
      "    MOV     R0, #0 \n" 
      "    STR     R0, [SP] \n" 
"loc_FFDC0F18:\n"
      "    LDR     R0, [R4, #0x10] \n" 
      "    MOV     R1, SP \n" 
      "    BL      sub_FFC17840 \n" 
      "    LDR     R0, [SP] \n" 
      "    CMP     R0, #0 \n" 
      "    BEQ     loc_FFDC0F44 \n" 
      "    LDR     R0, [R4, #0x10] \n" 
      "    MOV     R2, #0 \n" 
      "    ADD     R1, SP, #4 \n" 
      "    BL      sub_FFC1763C \n" 
      "    B       loc_FFDC0F18 \n" 
"loc_FFDC0F44:\n"
      "    LDR     R0, [R4] \n" 
      "    CMN     R0, #1 \n" 
      "    BEQ     loc_FFDC0F68 \n" 
      "    BL      sub_FFC13B5C \n" 
      "    MVN     R0, #0 \n" 
      "    STR     R0, [R4] \n" 
      "    LDR     R0, =0x7BA14 \n" 
      "    BL      sub_FFC3C9D4 \n" 
      "    BL      sub_FFC3E288 \n" 
"loc_FFDC0F68:\n"
      "    LDR     R0, [R4, #0xC] \n" 
      "    BL      sub_FFC0B844 \n" 
      "    B       loc_FFDC0EB0 \n" 
"loc_FFDC0F74:\n"
      "    BL      sub_FFDC1148_my \n" // -> open file
      "    B       loc_FFDC0EB0 \n" 
"loc_FFDC0F7C:\n"
      "    BL      sub_FFDC1288_my \n" // -> write file
      "    B       loc_FFDC0EB0 \n" 
"loc_FFDC0F84:\n"
      "    BL      sub_FFDC1384_my \n" // -> close file
      "    B       loc_FFDC0EB0 \n"
    );
}

void __attribute__((naked,noinline)) sub_FFDC1148_my(  ) {
asm volatile (
      "    STMFD   SP!, {R4-R8,LR} \n" 
      "    MOV     R4, R0 \n" 
      "    ADD     R0, R0, #0x2C \n" 
      "    SUB     SP, SP, #0x38 \n" 
      "    BL      sub_FFC3C9D4 \n" 
      "    MOV     R1, #0 \n" 
      "    BL      sub_FFC3E238 \n" 
      "    LDR     R0, [R4, #0xC] \n" 
      "    BL      sub_FFC3EB28 \n" 
      "    LDR     R7, [R4, #8] \n" 
      "    LDR     R8, =0x1B6 \n" 
      "    ADD     R6, R4, #0x2C \n" 
      "    LDR     R5, [R4, #0xC] \n" 
//place hook here
//the task's data block is at [r4] at this point, filename starts at [r4+0x2c]
//the block can be captured here for a (new) camera with unknown data block structure
//for ptp remote capture, return fake file handle (255) when done and jump to 0xFFDC11EC
//if writing does not need to be prevented, just continue
      "STMFD SP!, {R4-R12,LR}\n"
      "ADD R1, R4, #0x14\n" //data chunk definitions start here
      "ADD R0, R4, #0x2c\n" //name starts here
      "BL filewrite_main_hook\n"
      "LDMFD SP!, {R4-R12,LR}\n"
      "LDR R0, =ignore_current_write\n"
      "LDR R0, [R0]\n"
      "CMP R0, #0\n"
      "MOVNE R0, #0xff\n"
      "LDRNE PC, =0xFFDC11EC\n"
//hook end
      "    MOV     R0, R6 \n" 
      "    MOV     R1, R7 \n" 
      "    MOV     R2, R8 \n" 
      "    BL      sub_FFC13B34 \n" 
//
      "LDR PC, =0xffdc118c\n" //+ continue in ROM
//      "    CMN     R0, #1 \n" // rom:ffdc118c
//      "    BNE     loc_FFDC11EC \n" 
    );
}

void __attribute__((naked,noinline)) sub_FFDC1288_my(  ) {
asm volatile (
      "    STMFD   SP!, {R4-R10,LR} \n" 
      "    MOV     R4, R0 \n" 
      "    LDR     R0, [R0] \n" 
      "    CMP     R0, #4 \n" 
      "    LDREQ   R6, [R4, #0x18] \n" 
      "    LDREQ   R7, [R4, #0x14] \n" 
      "    BEQ     loc_FFDC12C4 \n" 
      "    CMP     R0, #5 \n" 
      "    LDREQ   R6, [R4, #0x20] \n" 
      "    LDREQ   R7, [R4, #0x1C] \n" 
      "    BEQ     loc_FFDC12C4 \n" 
      "    CMP     R0, #6 \n" 
      "    BNE     loc_FFDC12D8 \n" 
      "    LDR     R6, [R4, #0x28] \n" 
      "    LDR     R7, [R4, #0x24] \n" 
"loc_FFDC12C4:\n"
      "    CMP     R6, #0 \n" 
      "    BNE     loc_FFDC12E8 \n" 
"loc_FFDC12CC:\n"
      "    MOV     R1, R4 \n" 
      "    MOV     R0, #7 \n" 
      "    B       loc_FFDC137C \n" 
"loc_FFDC12D8:\n"
      "    LDR     R1, =0x1E2 \n" 
      "    LDR     R0, =0xFFDC1268 \n" 
      "    BL      sub_FFC0BDB8 \n" 
      "    B       loc_FFDC12CC \n" 
"loc_FFDC12E8:\n"
      "    LDR     R9, =0xC078 \n" 
      "    MOV     R5, R6 \n" 
"loc_FFDC12F0:\n"
      "    LDR     R0, [R4, #4] \n" 
      "    CMP     R5, #0x1000000 \n" 
      "    MOVLS   R8, R5 \n" 
      "    MOVHI   R8, #0x1000000 \n" 
      "    BIC     R1, R0, #0xFF000000 \n" 
      "    CMP     R1, #0 \n" 
      "    BICNE   R0, R0, #0xFF000000 \n" 
      "    RSBNE   R0, R0, #0x1000000 \n" 
      "    CMPNE   R8, R0 \n" 
      "    MOVHI   R8, R0 \n" 
//mod start
      "LDR R0, =ignore_current_write\n"
      "LDR R0, [R0]\n"
      "CMP R0, #0\n"
      "MOVNE R0, R8\n" //"everything's written"
      "BNE loc_ffdc1328\n" //jump past Write (in RAM, because there are jump targets before this location)
//mod end
      "    LDR     R0, [R9] \n" 
      "    MOV     R2, R8 \n" // nr of bytes to be written
      "    MOV     R1, R7 \n" 
      "    BL      sub_FFC13C08 \n" // Write
"loc_ffdc1328:\n" //+
      "    LDR     R1, [R4, #4] \n" // rom:ffdc1328
      "    CMP     R8, R0 \n" 
      "    ADD     R1, R1, R0 \n" 
      "    STR     R1, [R4, #4] \n" 
      "    BEQ     loc_FFDC1350 \n" 
      "    LDR     R0, =0x10B1 \n" 
      "    BL      sub_FFC5B294 \n" 
      "    LDR     R1, =0x9200005 \n" 
      "    STR     R1, [R4, #0x10] \n" 
      "    B       loc_FFDC12CC \n" 
"loc_FFDC1350:\n"
      "    SUB     R5, R5, R0 \n" 
      "    CMP     R5, R6 \n" 
      "    ADD     R7, R7, R0 \n" 
      "    LDRCS   R1, =0x211 \n" 
      "    LDRCS   R0, =0xFFDC1268 \n" 
      "    BLCS    sub_FFC0BDB8 \n" 
      "    CMP     R5, #0 \n" 
      "    BNE     loc_FFDC12F0 \n" 
      "    LDR     R0, [R4] \n" 
      "    MOV     R1, R4 \n" 
      "    ADD     R0, R0, #1 \n" 
"loc_FFDC137C:\n"
      "    LDMFD   SP!, {R4-R10,LR} \n" 
      "    B       sub_FFDC0E00 \n"
    );
}

void __attribute__((naked,noinline)) sub_FFDC1384_my(  ) {
asm volatile (
      "    STMFD   SP!, {R4,R5,LR} \n" 
      "    LDR     R5, =0xC078 \n" 
      "    MOV     R4, R0 \n" 
      "    LDR     R0, [R5] \n" 
      "    SUB     SP, SP, #0x1C \n" 
      "    CMN     R0, #1 \n" 
      "    BEQ     loc_FFDC13B8 \n" 
//mod start
      "LDR R1, =ignore_current_write\n"
      "LDR R1, [R1]\n"
      "CMP R1, #0\n"
      "MOVNE R0, #0\n" //"close OK"
      "LDRNE R1, =ignore_current_write\n"
      "STRNE R0, [R1]\n" //also disarm flag
      "LDRNE PC, =0xffdc13a4\n" //jump past Close (in ROM)
//mod end
      "    BL      sub_FFC13B5C \n" // Close
      "    CMP     R0, #0 \n" // rom:ffdc13a4
      "    LDRNE   R0, =0x9200003 \n" 
      "    STRNE   R0, [R4, #0x10] \n" 
      "    MVN     R0, #0 \n" 
      "    STR     R0, [R5] \n" 
"loc_FFDC13B8:\n"
      "    LDR     R0, [R4, #0x10] \n" 
      "LDR PC, =0xffdc13bc\n" //+ continue in ROM
/*      "    TST     R0, #1 \n" // rom:ffdc13bc
      "    BNE     loc_FFDC1400 \n" 
      "    LDR     R0, =0x81FF \n" 
      "    ADD     R1, SP, #4 \n" 
      "    STR     R0, [SP, #4] \n" 
      "    MOV     R0, #0x20 \n" 
      "    STR     R0, [SP, #8] \n" 
      "    LDR     R0, [R4, #4] \n" 
      "    STR     R0, [SP, #0xC] \n" 
      "    LDR     R0, [R4, #0xC] \n" 
      "    STR     R0, [SP, #0x10] \n" 
      "    LDR     R0, [R4, #0xC] \n" 
      "    STR     R0, [SP, #0x14] \n" 
      "    LDR     R0, [R4, #0xC] \n" 
      "    STR     R0, [SP, #0x18] \n" 
      "    ADD     R0, R4, #0x2C \n" 
      "    BL      sub_FFC3DB4C \n" 
"loc_FFDC1400:\n"
      "    ADD     R0, R4, #0x2C \n" 
      "    BL      sub_FFC3C9D4 \n" 
      "    BL      sub_FFC3E288 \n" 
      "    LDR     R1, [R5, #0x14] \n" 
      "    CMP     R1, #0 \n" 
      "    LDRNE   R0, [R4, #0x10] \n" 
      "    BLXNE   R1 \n" 
      "    ADD     SP, SP, #0x1C \n" 
      "    LDMFD   SP!, {R4,R5,PC} \n"
*/
    );
}

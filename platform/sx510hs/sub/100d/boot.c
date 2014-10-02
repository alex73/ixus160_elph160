/*
 * boot.c - auto-generated by CHDK code_gen.
 */
#include "lolevel.h"
#include "platform.h"
#include "core.h"
#include "dryos31.h"

#define offsetof(TYPE, MEMBER) ((int) &((TYPE *)0)->MEMBER)

const char * const new_sa = &_end;

/*----------------------------------------------------------------------
    CreateTask_spytask
-----------------------------------------------------------------------*/
void CreateTask_spytask()
{
    _CreateTask("SpyTask", 0x19, 0x2000, core_spytask, 0);
}

///*----------------------------------------------------------------------
// Pointer to stack location where jogdial task records previous and current
// jogdial positions
short *jog_position;


#define GREEN_LED       0xC0220120
#define AF_LED          0xC0223030
//debug use only

int debug_blink(int save_R0) {
	int i;
	*((volatile int *) AF_LED) = 0x46; // Turn on LED
	for (i=0; i<800000; i++) // Wait a while
		{
    		asm volatile ( "nop\n" );
		}

	*((volatile int *) AF_LED) = 0x44; // Turn off LED
	for (i=0; i<800000; i++) // Wait a while
		{
    		asm volatile ( "nop\n" );
		}
	return save_R0;
};

void __attribute__((naked,noinline)) my_blinker(int n) {
	asm volatile (
      "            STMFD   SP!, {R0-R9,LR}\n"
);
	int i, j;
	for (j=0; j<n; j++)
	{
		*((volatile int *) GREEN_LED) = 0x46; // Turn on LED
		for (i=0; i<0x200000; i++) { asm volatile ( "nop \n" ); }

		*((volatile int *) GREEN_LED) = 0x44; // Turn off LED
		for (i=0; i<0x400000; i++) { asm volatile ( "nop \n" ); }
	}
	for (i=0; i<0x900000; i++) { asm volatile ( "nop \n" ); }
	asm volatile (
      "            LDMFD   SP!, {R0-R9,PC}\n"
	);
}

/*----------------------------------------------------------------------
    boot()

    Main entry point for the CHDK code
-----------------------------------------------------------------------*/

/*************************************************************/
//** boot @ 0xFF00000C - 0xFF00017C, length=93
void __attribute__((naked,noinline)) boot() {
asm volatile (
"    LDR     R1, =0xC0410000 \n"
"    MOV     R0, #0 \n"
"    STR     R0, [R1] \n"
"    MOV     R1, #0x78 \n"
"    MCR     p15, 0, R1, c1, c0 \n"
"    MOV     R1, #0 \n"
"    MCR     p15, 0, R1, c7, c10, 4 \n"
"    MCR     p15, 0, R1, c7, c5 \n"
"    MCR     p15, 0, R1, c7, c6 \n"
"    MOV     R0, #0x3D \n"
"    MCR     p15, 0, R0, c6, c0 \n"
"    MOV     R0, #0xC000002F \n"
"    MCR     p15, 0, R0, c6, c1 \n"
"    MOV     R0, #0x35 \n"
"    MCR     p15, 0, R0, c6, c2 \n"
"    MOV     R0, #0x40000035 \n"
"    MCR     p15, 0, R0, c6, c3 \n"
"    MOV     R0, #0x80000017 \n"
"    MCR     p15, 0, R0, c6, c4 \n"
"    LDR     R0, =0xFF00002F \n"
"    MCR     p15, 0, R0, c6, c5 \n"
"    MOV     R0, #0x34 \n"
"    MCR     p15, 0, R0, c2, c0 \n"
"    MOV     R0, #0x34 \n"
"    MCR     p15, 0, R0, c2, c0, 1 \n"
"    MOV     R0, #0x34 \n"
"    MCR     p15, 0, R0, c3, c0 \n"
"    LDR     R0, =0x3333330 \n"
"    MCR     p15, 0, R0, c5, c0, 2 \n"
"    LDR     R0, =0x3333330 \n"
"    MCR     p15, 0, R0, c5, c0, 3 \n"
"    MRC     p15, 0, R0, c1, c0 \n"
"    ORR     R0, R0, #0x1000 \n"
"    ORR     R0, R0, #4 \n"
"    ORR     R0, R0, #1 \n"
"    MCR     p15, 0, R0, c1, c0 \n"
"    MOV     R1, #0x80000006 \n"
"    MCR     p15, 0, R1, c9, c1 \n"
"    MOV     R1, #6 \n"
"    MCR     p15, 0, R1, c9, c1, 1 \n"
"    MRC     p15, 0, R1, c1, c0 \n"
"    ORR     R1, R1, #0x50000 \n"
"    MCR     p15, 0, R1, c1, c0 \n"
"    LDR     R2, =0xC0200000 \n"
"    MOV     R1, #1 \n"
"    STR     R1, [R2, #0x10C] \n"
"    MOV     R1, #0xFF \n"
"    STR     R1, [R2, #0xC] \n"
"    STR     R1, [R2, #0x1C] \n"
"    STR     R1, [R2, #0x2C] \n"
"    STR     R1, [R2, #0x3C] \n"
"    STR     R1, [R2, #0x4C] \n"
"    STR     R1, [R2, #0x5C] \n"
"    STR     R1, [R2, #0x6C] \n"
"    STR     R1, [R2, #0x7C] \n"
"    STR     R1, [R2, #0x8C] \n"
"    STR     R1, [R2, #0x9C] \n"
"    STR     R1, [R2, #0xAC] \n"
"    STR     R1, [R2, #0xBC] \n"
"    STR     R1, [R2, #0xCC] \n"
"    STR     R1, [R2, #0xDC] \n"
"    STR     R1, [R2, #0xEC] \n"
"    STR     R1, [R2, #0xFC] \n"
"    LDR     R1, =0xC0400008 \n"
"    LDR     R2, =0x430005 \n"
"    STR     R2, [R1] \n"
"    MOV     R1, #1 \n"
"    LDR     R2, =0xC0243100 \n"
"    STR     R2, [R1] \n"
"    LDR     R2, =0xC0242010 \n"
"    LDR     R1, [R2] \n"
"    ORR     R1, R1, #1 \n"
"    STR     R1, [R2] \n"
"    LDR     R0, =0xFF719234 \n"
"    LDR     R1, =0x6B1000 \n"
"    LDR     R3, =0x6DB5D0 \n"

"loc_FF00013C:\n"
"    CMP     R1, R3 \n"
"    LDRCC   R2, [R0], #4 \n"
"    STRCC   R2, [R1], #4 \n"
"    BCC     loc_FF00013C \n"
"    LDR     R0, =0xFF702F8C \n"
"    LDR     R1, =0x1900 \n"
"    LDR     R3, =0x17BA8 \n"

"loc_FF000158:\n"
"    CMP     R1, R3 \n"
"    LDRCC   R2, [R0], #4 \n"
"    STRCC   R2, [R1], #4 \n"
"    BCC     loc_FF000158 \n"
"    LDR     R1, =0x265A28 \n"
"    MOV     R2, #0 \n"

"loc_FF000170:\n"
"    CMP     R3, R1 \n"
"    STRCC   R2, [R3], #4 \n"
"    BCC     loc_FF000170 \n"
/* Install task hooks via 0x193x is not possible with this new dryos version
   So the below code patches the CreateTask function in RAM to install our
   hook -- ERR99
*/
// Install CreateTask patch
"    LDR     R0, =patch_CreateTask\n"   // Patch data
"    LDM     R0, {R1,R2}\n"             // Get two patch instructions
"    LDR     R0, =hook_CreateTask\n"    // Address to patch
"    STM     R0, {R1,R2}\n"             // Store patch instructions

"    B       sub_FF0003C4_my \n"  // --> Patched. Old value = 0xFF0003C4.

"patch_CreateTask:\n"
"    LDR     PC, [PC,#-0x4]\n"          // Do jump to absolute address CreateTask_my
"    .long   CreateTask_my\n"
);
}

/*************************************************************/
//** CreateTask_my @ 0x006B5778 - 0x006B577C, length=2
void __attribute__((naked,noinline)) CreateTask_my() {
asm volatile (
"    STMFD   SP!, {R0}\n"
//R3 = Pointer to task function to create

/*** INSTALL capt_seq_task() hook ***/
"    LDR     R0, =task_CaptSeq\n"       // DryOS original code function ptr.
"    CMP     R0, R3\n"                  // is the given taskptr equal to our searched function?
"    LDREQ   R3, =capt_seq_task\n"      // if so replace with our task function base ptr.
"    BEQ     exitHook\n"                // below compares not necessary if this check has found something.

/*** INSTALL exp_drv_task() hook ***/
"    LDR     R0, =task_ExpDrv\n"
"    CMP     R0, R3\n"
"    LDREQ   R3, =exp_drv_task\n"
"    BEQ     exitHook\n"

/*** INSTALL filewrite() hook ***/
"    LDR     R0, =task_FileWrite\n"
"    CMP     R0, R3\n"
"    LDREQ   R3, =filewritetask\n"
"    BEQ     exitHook\n"

/*** INSTALL JogDial() hook ***/
"    LDR     R0, =task_RotaryEncoder\n"
"    CMP     R0, R3\n"
"    LDREQ   R3, =JogDial_task_my\n"
"    BEQ     exitHook\n"

/*** INSTALL movie_record_task() hook ***/
//"    LDR     R0, =task_MovieRecord\n"
//"    CMP     R0, R3\n"
//"    LDREQ   R3, =movie_record_task\n"
//"    BEQ     exitHook\n"

/*** INSTALL init_file_modules_task() hook ***/
"    LDR     R0, =task_InitFileModules\n"
"    CMP     R0, R3\n"
"    LDREQ   R3, =init_file_modules_task\n"

"exitHook:\n" 
// restore overwritten registers
"    LDMFD   SP!, {R0}\n"
// Execute overwritten instructions from original code, then jump to firmware
"    STMFD   SP!, {R1-R9,LR} \n"
"    MOV     R4, R0 \n"
"    LDR     PC, =0x006B5780 \n"  // Continue in firmware
);
}

/*************************************************************/
//** sub_FF0003C4_my @ 0xFF0003C4 - 0xFF00042C, length=27
void __attribute__((naked,noinline)) sub_FF0003C4_my() {

    //Replacement of sub_ for correct power-on.
    //(short press = playback mode, long press = record mode)

    // look at power-on switch sub_FF00BD98
    // value and pointer from sub_FF04EAFC
    *(int*)(0x2b8c+0x4) = (*(int*)0xC0220104)&1 ? 0x200000 : 0x100000;

asm volatile (
"    LDR     R0, =0xFF00043C \n"
"    MOV     R1, #0 \n"
"    LDR     R3, =0xFF000474 \n"

"loc_FF0003D0:\n"
"    CMP     R0, R3 \n"
"    LDRCC   R2, [R0], #4 \n"
"    STRCC   R2, [R1], #4 \n"
"    BCC     loc_FF0003D0 \n"
"    LDR     R0, =0xFF000474 \n"
"    MOV     R1, #0x1B0 \n"
"    LDR     R3, =0xFF00065C \n"

"loc_FF0003EC:\n"
"    CMP     R0, R3 \n"
"    LDRCC   R2, [R0], #4 \n"
"    STRCC   R2, [R1], #4 \n"
"    BCC     loc_FF0003EC \n"
"    MOV     R0, #0xD2 \n"
"    MSR     CPSR_cxsf, R0 \n"
"    MOV     SP, #0x1000 \n"
"    MOV     R0, #0xD3 \n"
"    MSR     CPSR_cxsf, R0 \n"
"    MOV     SP, #0x1000 \n"
"    LDR     R0, =0x398 \n"
"    LDR     R2, =0xEEEEEEEE \n"
"    MOV     R3, #0x1000 \n"

"loc_FF000420:\n"
"    CMP     R0, R3 \n"
"    STRCC   R2, [R0], #4 \n"
"    BCC     loc_FF000420 \n"
"    B       sub_FF00291C_my \n"  // --> Patched. Old value = 0xFF00291C.
);
}

/*************************************************************/
//** sub_FF00291C_my @ 0xFF00291C - 0xFF002A9C, length=97
void __attribute__((naked,noinline)) sub_FF00291C_my() {
asm volatile (
"    LDR     R1, =0x1A54 \n"
"    STR     LR, [SP, #-4]! \n"
"    SUB     SP, SP, #0x74 \n"
"    MOV     R0, #0x80000 \n"
"    STR     R0, [R1] \n"
"    LDR     R0, =0x40BCE980 \n"
"    LDR     R1, =0x1A58 \n"
"    STR     R0, [R1] \n"
"    LDR     R1, =0x1A5C \n"
"    ADD     R0, R0, #0x2000 \n"
"    STR     R0, [R1] \n"
"    MOV     R1, #0x74 \n"
"    MOV     R0, SP \n"
"    BL      sub_006D4CD4 \n"
"    MOV     R0, #0x73000 \n"
"    STR     R0, [SP, #4] \n"

#if defined(CHDK_NOT_IN_CANON_HEAP) // use original heap offset if CHDK is loaded in high memory
"    LDR     R0, =0x265A28 \n"
#else
"    LDR     R0, =new_sa\n"   // otherwise use patched value
"    LDR     R0, [R0]\n"      //
#endif

"    LDR     R2, =0x5903AC \n"
"    STR     R0, [SP, #8] \n"
"    SUB     R0, R2, R0 \n"
"    STR     R0, [SP, #0xC] \n"
"    MOV     R0, #0x22 \n"
"    STR     R0, [SP, #0x18] \n"
"    MOV     R0, #0x98 \n"
"    STR     R0, [SP, #0x1C] \n"
"    MOV     R0, #0x1E8 \n"
"    STR     R2, [SP, #0x10] \n"
"    STR     R0, [SP, #0x20] \n"
"    LDR     R1, =0x59B000 \n"
"    SUB     R0, R0, #0xE6 \n"
"    STR     R0, [SP, #0x24] \n"
"    MOV     R0, #0xB6 \n"
"    STR     R1, [SP] \n"
"    STR     R0, [SP, #0x28] \n"
"    MOV     R0, #0x85 \n"
"    STR     R0, [SP, #0x2C] \n"
"    MOV     R0, #0x40 \n"
"    STR     R0, [SP, #0x30] \n"
"    MOV     R0, #4 \n"
"    STR     R0, [SP, #0x34] \n"
"    MOV     R0, #0 \n"
"    STR     R0, [SP, #0x38] \n"
"    MOV     R0, #0x10 \n"
"    STR     R0, [SP, #0x5C] \n"
"    MOV     R0, #0x1000 \n"
"    STR     R0, [SP, #0x60] \n"
"    MOV     R0, #0x100 \n"
"    STR     R0, [SP, #0x64] \n"
"    MOV     R0, #0x2000 \n"
"    STR     R1, [SP, #0x14] \n"
"    STR     R0, [SP, #0x68] \n"
"    LDR     R1, =sub_FF0049F4_my \n"  // --> Patched. Old value = 0xFF0049F4.
"    MOV     R2, #0 \n"
"    MOV     R0, SP \n"
"    BL      sub_006B1C50 \n"
"    ADD     SP, SP, #0x74 \n"
"    LDR     PC, [SP], #4 \n"
);
}

/*************************************************************/
//** sub_FF0049F4_my @ 0xFF0049F4 - 0xFF004A8C, length=39
void __attribute__((naked,noinline)) sub_FF0049F4_my() {
asm volatile (
"    STMFD   SP!, {R4,LR} \n"
"    LDR     R4, =0xFF004AB0 /*'/_term'*/ \n"
"    BL      sub_FF000858 \n"
"    LDR     R1, =0x1A54 \n"
"    LDR     R0, =0x19F0 \n"
"    LDR     R1, [R1] \n"
"    LDR     R0, [R0] \n"
"    ADD     R1, R1, #0x10 \n"
"    CMP     R0, R1 \n"
"    LDRCC   R0, =0xFF004AC0 /*'USER_MEM size checking'*/ \n"
"    BLCC    _err_init_task \n"
"    BL      sub_FF0037BC \n"
"    CMP     R0, #0 \n"
"    LDRLT   R0, =0xFF004AD8 /*'dmSetup'*/ \n"
"    BLLT    _err_init_task \n"
"    BL      sub_FF002528 \n"
"    CMP     R0, #0 \n"
"    LDRLT   R0, =0xFF004AE0 /*'termDriverInit'*/ \n"
"    BLLT    _err_init_task \n"
"    MOV     R0, R4 \n"
"    BL      sub_FF002610 \n"
"    CMP     R0, #0 \n"
"    LDRLT   R0, =0xFF004AF0 /*'termDeviceCreate'*/ \n"
"    BLLT    _err_init_task \n"
"    MOV     R0, R4 \n"
"    BL      sub_FF002140 \n"
"    CMP     R0, #0 \n"
"    LDRLT   R0, =0xFF004B04 /*'stdioSetup'*/ \n"
"    BLLT    _err_init_task \n"
"    BL      sub_FF00505C \n"
"    CMP     R0, #0 \n"
"    LDRLT   R0, =0xFF004B10 /*'stdlibSetup'*/ \n"
"    BLLT    _err_init_task \n"
"    BL      sub_FF0010E8 \n"
"    CMP     R0, #0 \n"
"    LDRLT   R0, =0xFF004B1C /*'extlib_setup'*/ \n"
"    BLLT    _err_init_task \n"
"    LDMFD   SP!, {R4,LR} \n"
"    B       sub_FF007D34_my \n"  // --> Patched. Old value = 0xFF007D34.
);
}

/*************************************************************/
//** sub_FF007D34_my @ 0xFF007D34 - 0xFF007DA4, length=29
void __attribute__((naked,noinline)) sub_FF007D34_my() {
asm volatile (
"    STMFD   SP!, {R3,LR} \n"
"    BL      sub_FF013B78 \n"
"    BL      sub_FF0178F8 \n"
"    CMP     R0, #0 \n"
"    BNE     loc_FF007D58 \n"
"    BL      sub_FF00D16C /*_IsNormalCameraMode_FW*/ \n"
"    CMP     R0, #0 \n"
"    MOVNE   R0, #1 \n"
"    BNE     loc_FF007D5C \n"

"loc_FF007D58:\n"
"    MOV     R0, #0 \n"

"loc_FF007D5C:\n"
"    BL      sub_FF00BD98_my \n"  // --> Patched. Old value = 0xFF00BD98.
"    CMP     R0, #0 \n"
"    BNE     loc_FF007D70 \n"
"    BL      sub_FF00B684 \n"

"loc_FF007D6C:\n"
"    B       loc_FF007D6C \n"

"loc_FF007D70:\n"
"    BL      sub_006B8A34 \n"
"    LDR     R1, =0x60E000 \n"
"    MOV     R0, #0 \n"
"    BL      sub_FF017E48 \n"
"    BL      sub_006B8D78 \n"
"    MOV     R3, #0 \n"
"    STR     R3, [SP] \n"
"    LDR     R3, =task_Startup_my \n"  // --> Patched. Old value = 0xFF007CCC.
"    MOV     R2, #0 \n"
"    MOV     R1, #0x19 \n"
"    LDR     R0, =0xFF007DB0 /*'Startup'*/ \n"
"    BL      _CreateTask \n"
"    MOV     R0, #0 \n"
"    LDMFD   SP!, {R3,PC} \n"
);
}

/*************************************************************/
//** sub_FF00BD98_my @ 0xFF00BD98 - 0xFF00BDFC, length=26
void __attribute__((naked,noinline)) sub_FF00BD98_my() {
asm volatile (
"    STMFD   SP!, {R2-R8,LR} \n"
"    MOV     R6, #0 \n"
"    MOV     R8, R0 \n"
"    MOV     R7, R6 \n"
//"  BL      _sub_FF04EAF4 \n"  // --> Nullsub call removed.
"    LDR     R0, =0xC0220104 \n"
"    BL      sub_FF04BDE8 \n"
"    MOV     R4, #1 \n"
"    BIC     R5, R4, R0 \n"
"    LDR     R0, =0xC022010C \n"
"    BL      sub_FF04BDE8 \n"
"    CMP     R8, #0 \n"
"    BIC     R4, R4, R0 \n"
"    BEQ     loc_FF00BDD8 \n"
"    ORRS    R0, R5, R4 \n"
"    BEQ     loc_FF00BDFC \n"

"loc_FF00BDD8:\n"
"    BL      sub_FF0178F8 \n"
"    MOV     R2, R0 \n"
"    MOV     R3, #0 \n"
"    MOV     R1, R4 \n"
"    MOV     R0, R5 \n"
"    STRD    R6, [SP] \n"
//"  BL      _sub_FF04EAFC \n"  // Disable StartUpChecks
//"  BL      _sub_FF04EAF8 \n"  // --> Nullsub call removed.
"    MOV     R0, #1 \n"

"loc_FF00BDFC:\n"
"    LDMFD   SP!, {R2-R8,PC} \n"
);
}

/*************************************************************/
//** task_Startup_my @ 0xFF007CCC - 0xFF007D30, length=26
void __attribute__((naked,noinline)) task_Startup_my() {
asm volatile (
"    STMFD   SP!, {R4,LR} \n"
"    BL      sub_FF002F00 \n"
"    BL      sub_FF00C1F4 \n"
"    BL      sub_FF00B630 \n"
//"  BL      _sub_FF04F600 \n"  // --> Nullsub call removed.
"    BL      sub_FF017B54 \n"
//"  BL      _sub_FF0179F0 \n"  // load DISKBOOT.BIN
"    BL      sub_FF017CE4 \n"
"    BL      sub_FF017F1C \n"
"    BL      sub_FF017CA8 \n"
"    BL      sub_FF017B94 \n"
"    BL      sub_FF013AB0 \n"
"    BL      sub_FF017F24 \n"
"    BL      CreateTask_spytask\n" // added
"    BL      taskcreatePhySw_my \n"  // --> Patched. Old value = 0xFF00BC40.
"    BL      sub_FF011598 \n"
"    BL      sub_FF09B6C0 \n"
"    BL      sub_FF00910C \n"
"    BL      sub_FF00AF94 \n"
"    BL      sub_FF017728 \n"
"    BL      sub_FF00B5E4 \n"
"    BL      sub_FF00AF30 \n"
"    BL      sub_FF039774 \n"
"    BL      sub_FF009E78 \n"
"    BL      sub_FF00AEF4 \n"
"    LDMFD   SP!, {R4,LR} \n"
"    B       sub_FF003028 \n"
);
}

/*************************************************************/
//** taskcreatePhySw_my @ 0xFF00BC40 - 0xFF00BC88, length=19
void __attribute__((naked,noinline)) taskcreatePhySw_my() {
asm volatile (
"    STMFD   SP!, {R3-R5,LR} \n"
"    BL      sub_FF016980 \n"
"    BL      sub_FF00D0BC /*_IsFactoryMode_FW*/ \n"
"    CMP     R0, #0 \n"
"    BLEQ    sub_FF0168E8 /*_OpLog.Start_FW*/ \n"
"    LDR     R4, =0x1CFC \n"
"    LDR     R0, [R4, #4] \n"
"    CMP     R0, #0 \n"
"    BNE     loc_FF00BC84 \n"
"    MOV     R3, #0 \n"
"    STR     R3, [SP] \n"
"    LDR     R3, =mykbd_task \n"  // --> Patched. Old value = 0xFF00BC0C.
"    MOV     R2, #0x2000 \n"  // --> Patched. Old value = 0x800. stack size for new task_PhySw so we don't have to do stack switch
"    MOV     R1, #0x17 \n"
"    LDR     R0, =0xFF00BEF4 /*'PhySw'*/ \n"
"    BL      sub_006B7C6C /*_CreateTaskStrictly*/ \n"
"    STR     R0, [R4, #4] \n"

"loc_FF00BC84:\n"
"    LDMFD   SP!, {R3-R5,LR} \n"
"    B       sub_FF04FAB0 \n"
);
}

/*************************************************************/
//** init_file_modules_task @ 0xFF03E99C - 0xFF03E9D0, length=14
void __attribute__((naked,noinline)) init_file_modules_task() {
asm volatile (
"    STMFD   SP!, {R4-R6,LR} \n"
"    BL      sub_FF0865CC \n"
"    LDR     R5, =0x5006 \n"
"    MOVS    R4, R0 \n"
"    MOVNE   R1, #0 \n"
"    MOVNE   R0, R5 \n"
"    BLNE    _PostLogicalEventToUI \n"
"    BL      sub_FF086600 \n"
"    BL      core_spytask_can_start\n"  // CHDK: Set "it's-safe-to-start" flag for spytask
"    CMP     R4, #0 \n"
"    LDMNEFD SP!, {R4-R6,PC} \n"
"    MOV     R0, R5 \n"
"    LDMFD   SP!, {R4-R6,LR} \n"
"    MOV     R1, #0 \n"
"    B       _PostLogicalEventToUI \n"
);
}

/*************************************************************/
//** JogDial_task_my @ 0xFF04F774 - 0xFF04FAAC, length=207
void __attribute__((naked,noinline)) JogDial_task_my() {
asm volatile (
"    STMFD   SP!, {R4-R11,LR} \n"
"    SUB     SP, SP, #0x1C \n"
"    BL      sub_FF04FB1C \n"
"    LDR     R12, =0x2BAC \n"
"    LDR     R6, =0xFF5687FC \n"
"    MOV     R0, #0 \n"
"    ADD     R10, SP, #8 \n"
"    ADD     R9, SP, #0xC \n"

// Save pointer for kbd.c routine
"    LDR     R2, =jog_position \n"
"    STR     R9, [R2] \n"


"loc_FF04F794:\n"
"    ADD     R2, SP, #0x14 \n"
"    MOV     R1, #0 \n"
"    ADD     R4, R2, R0, LSL#1 \n"
"    ADD     R3, SP, #0x10 \n"
"    STRH    R1, [R4] \n"
"    ADD     R4, R3, R0, LSL#1 \n"
"    STRH    R1, [R4] \n"
"    STR     R1, [R9, R0, LSL#2] \n"
"    STR     R1, [R10, R0, LSL#2] \n"
"    ADD     R0, R0, #1 \n"
"    CMP     R0, #1 \n"
"    BLT     loc_FF04F794 \n"

"loc_FF04F7C4:\n"
"    LDR     R0, =0x2BAC \n"
"    MOV     R2, #0 \n"
"    LDR     R0, [R0, #8] \n"
"    MOV     R1, SP \n"
"    BL      sub_006B84B8 \n"
"    CMP     R0, #0 \n"
"    LDRNE   R2, =0x20B \n"
"    LDRNE   R1, =0xFF04FA34 /*'RotaryEncoder.c'*/ \n"
"    MOVNE   R0, #0 \n"
"    BLNE    _DebugAssert \n"
"    LDR     R0, [SP] \n"
"    AND     R4, R0, #0xFF \n"
"    AND     R0, R0, #0xFF00 \n"
"    CMP     R0, #0x100 \n"
"    BEQ     loc_FF04F83C \n"
"    CMP     R0, #0x200 \n"
"    BEQ     loc_FF04F874 \n"
"    CMP     R0, #0x300 \n"
"    BEQ     loc_FF04FA80 \n"
"    CMP     R0, #0x400 \n"
"    BNE     loc_FF04F7C4 \n"
"    CMP     R4, #0 \n"
"    LDRNE   R2, =0x285 \n"
"    LDRNE   R1, =0xFF04FA34 /*'RotaryEncoder.c'*/ \n"
"    MOVNE   R0, #0 \n"
"    BLNE    _DebugAssert \n"
"    RSB     R0, R4, R4, LSL#3 \n"
"    LDR     R0, [R6, R0, LSL#2] \n"

"loc_FF04F834:\n"
"    BL      sub_FF04FB00 \n"
"    B       loc_FF04F7C4 \n"

"loc_FF04F83C:\n"
//------------------  added code ---------------------
"labelA:\n"
"    LDR     R0, =jogdial_stopped\n"
"    LDR     R0, [R0]\n"
"    CMP     R0, #1\n"
"    BNE     labelB\n"
"    MOV     R0, #40\n"
"    BL      _SleepTask\n"
"    B       labelA\n"
"labelB:\n"
//------------------  original code ------------------
"    LDR     R0, =0x2BB8 \n"
"    LDR     R0, [R0, R4, LSL#2] \n"
"    BL      sub_FF0062E0 \n"
"    LDR     R2, =0xFF04F6B8 \n"
"    ORR     R3, R4, #0x200 \n"
"    MOV     R1, R2 \n"
"    MOV     R0, #0x28 \n"
"    BL      sub_FF006210 /*_SetTimerAfter*/ \n"
"    TST     R0, #1 \n"
"    CMPNE   R0, #0x15 \n"
"    STR     R0, [R10, R4, LSL#2] \n"
"    BEQ     loc_FF04F7C4 \n"
"    LDR     R2, =0x21D \n"
"    B       loc_FF04FA1C \n"

"loc_FF04F874:\n"
"    RSB     R5, R4, R4, LSL#3 \n"
"    LDR     R0, [R6, R5, LSL#2] \n"
"    LDR     R1, =0xC0240000 \n"
"    ADD     R0, R1, R0, LSL#8 \n"
"    LDR     R0, [R0, #0x104] \n"
"    MOV     R1, R0, ASR#16 \n"
"    ADD     R0, SP, #0x14 \n"
"    ADD     R11, R0, R4, LSL#1 \n"
"    ADD     R0, SP, #0x10 \n"
"    ADD     R0, R0, R4, LSL#1 \n"
"    STRH    R1, [R11] \n"
"    STR     R0, [SP, #0x18] \n"
"    LDRSH   R3, [R0] \n"
"    SUB     R2, R1, R3 \n"
"    CMP     R2, #0 \n"
"    BNE     loc_FF04F8F8 \n"
"    LDR     R0, [R9, R4, LSL#2] \n"
"    CMP     R0, #0 \n"
"    BEQ     loc_FF04F9D8 \n"
"    LDR     R7, =0x2BB8 \n"
"    LDR     R0, [R7, R4, LSL#2] \n"
"    BL      sub_FF0062E0 \n"
"    LDR     R2, =0xFF04F6C4 \n"
"    ORR     R3, R4, #0x300 \n"
"    MOV     R1, R2 \n"
"    MOV     R0, #0x1F4 \n"
"    BL      sub_FF006210 /*_SetTimerAfter*/ \n"
"    TST     R0, #1 \n"
"    CMPNE   R0, #0x15 \n"
"    STR     R0, [R7, R4, LSL#2] \n"
"    BEQ     loc_FF04F9D8 \n"
"    LDR     R2, =0x236 \n"
"    B       loc_FF04F9CC \n"

"loc_FF04F8F8:\n"
"    MOV     R0, R2 \n"
"    RSBLT   R0, R0, #0 \n"
"    MOVLE   R7, #0 \n"
"    MOVGT   R7, #1 \n"
"    CMP     R0, #0xFF \n"
"    BLS     loc_FF04F934 \n"
"    LDR     R0, =0x7FFF \n"
"    CMP     R2, #0 \n"
"    SUBLE   R0, R0, R3 \n"
"    ADDLE   R0, R0, R1 \n"
"    SUBGT   R0, R0, R1 \n"
"    ADDGT   R0, R0, R3 \n"
"    MVN     R1, #0x8000 \n"
"    SUB     R0, R0, R1 \n"
"    EOR     R7, R7, #1 \n"

"loc_FF04F934:\n"
"    STR     R0, [SP, #4] \n"
"    LDR     R0, [R9, R4, LSL#2] \n"
"    CMP     R0, #0 \n"
"    ADDEQ   R0, R6, R5, LSL#2 \n"
"    LDREQ   R0, [R0, #8] \n"
"    BEQ     loc_FF04F96C \n"
"    ADD     R8, R6, R5, LSL#2 \n"
"    ADD     R1, R8, R7, LSL#2 \n"
"    LDR     R1, [R1, #0x10] \n"
"    CMP     R1, R0 \n"
"    BEQ     loc_FF04F970 \n"
"    LDR     R0, [R8, #0xC] \n"
"    BL      sub_FF00BC8C \n"
"    LDR     R0, [R8, #8] \n"

"loc_FF04F96C:\n"
"    BL      sub_FF00BC8C \n"

"loc_FF04F970:\n"
"    ADD     R0, R6, R5, LSL#2 \n"
"    ADD     R7, R0, R7, LSL#2 \n"
"    LDR     R0, [R7, #0x10] \n"
"    LDR     R1, [SP, #4] \n"
"    BL      sub_FF00BC9C \n"
"    LDR     R0, [R7, #0x10] \n"
"    LDR     R7, =0x2BB8 \n"
"    STR     R0, [R9, R4, LSL#2] \n"
"    LDRH    R1, [R11] \n"
"    LDR     R0, [SP, #0x18] \n"
"    STRH    R1, [R0] \n"
"    LDR     R0, [R7, R4, LSL#2] \n"
"    BL      sub_FF0062E0 \n"
"    LDR     R2, =0xFF04F6C4 \n"
"    ORR     R3, R4, #0x300 \n"
"    MOV     R1, R2 \n"
"    MOV     R0, #0x1F4 \n"
"    BL      sub_FF006210 /*_SetTimerAfter*/ \n"
"    TST     R0, #1 \n"
"    CMPNE   R0, #0x15 \n"
"    STR     R0, [R7, R4, LSL#2] \n"
"    BEQ     loc_FF04F9D8 \n"
"    LDR     R2, =0x267 \n"

"loc_FF04F9CC:\n"
"    LDR     R1, =0xFF04FA34 /*'RotaryEncoder.c'*/ \n"
"    MOV     R0, #0 \n"
"    BL      _DebugAssert \n"

"loc_FF04F9D8:\n"
"    ADD     R0, R6, R5, LSL#2 \n"
"    LDR     R0, [R0, #0x18] \n"
"    CMP     R0, #1 \n"
"    BNE     loc_FF04FA78 \n"
"    LDR     R0, =0x2BAC \n"
"    LDR     R0, [R0, #0x10] \n"
"    CMP     R0, #0 \n"
"    BEQ     loc_FF04FA78 \n"
"    LDR     R2, =0xFF04F6B8 \n"
"    ORR     R3, R4, #0x400 \n"
"    MOV     R1, R2 \n"
"    BL      sub_FF006210 /*_SetTimerAfter*/ \n"
"    TST     R0, #1 \n"
"    CMPNE   R0, #0x15 \n"
"    STR     R0, [R10, R4, LSL#2] \n"
"    BEQ     loc_FF04F7C4 \n"
"    LDR     R2, =0x26E \n"

"loc_FF04FA1C:\n"
"    LDR     R1, =0xFF04FA34 /*'RotaryEncoder.c'*/ \n"
"    MOV     R0, #0 \n"
"    BL      _DebugAssert \n"
"    B       loc_FF04F7C4 \n"

"loc_FF04FA78:\n"
"    LDR     R0, [R6, R5, LSL#2] \n"
"    B       loc_FF04F834 \n"

"loc_FF04FA80:\n"
"    LDR     R0, [R9, R4, LSL#2] \n"
"    CMP     R0, #0 \n"
"    MOVEQ   R2, #0x278 \n"
"    LDREQ   R1, =0xFF04FA34 /*'RotaryEncoder.c'*/ \n"
"    BLEQ    _DebugAssert \n"
"    RSB     R0, R4, R4, LSL#3 \n"
"    ADD     R0, R6, R0, LSL#2 \n"
"    LDR     R0, [R0, #0xC] \n"
"    BL      sub_FF00BC8C \n"
"    MOV     R0, #0 \n"
"    STR     R0, [R9, R4, LSL#2] \n"
"    B       loc_FF04F7C4 \n"
);
}

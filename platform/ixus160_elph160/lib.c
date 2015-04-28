#include "platform.h"
#include "platform_palette.h"
#include "lolevel.h"

#define LED_PR 0xc022f1fc
#define LED_AF 0xc022d200


void debug_led(int state)
{
	*(int*)LED_PR=state ? 0x93d800 : 0x83dc00;
}

void shutdown()
{
    volatile long *p = (void*)LED_PR;    // Green LED

    asm(
        "MRS     R1, CPSR\n"
        "AND     R0, R1, #0x80\n"
        "ORR     R1, R1, #0x80\n"
        "MSR     CPSR_cf, R1\n"
        :::"r1","r0");

    *p = 0x83dc00;  // power off.

    while(1);
}

// TODO: how to find the two values of the led_table
// A2500 has two 'lights' - Power LED, and AF assist lamp
// Power Led = first entry in table (led 0)
// AF Assist Lamp = second entry in table (led 1)
void camera_set_led(int led, int state, int bright) {
    static char led_table[2]={0,4};
    if(state<=1) _LEDDrive(led_table[led%sizeof(led_table)], (!state)&1);
}

void *vid_get_bitmap_fb()        { return (void*)0x40711000; }             // Found @0xff8662d0
void *vid_get_viewport_fb()      { return (void*)0x40866b80; }             // Found @0xffb7af7c
char *camera_jpeg_count_str()    { return (char*)0x000d7868; }             // Found @0xff9ff7f0
int get_flash_params_count(void) { return 0xd4; }                          // Found @0xff9af548

// Search for imgddev.c and work up from there
void *vid_get_bitmap_active_buffer()
{
//FF9437EC
//Also at FF943304
    return (void *)(*(int*)(0x6144+0x18)); // found @0xFF9437CC by comparing with a2500
}
// start palette table FFC062B0?
void *vid_get_bitmap_active_palette()
{
    extern int active_palette_buffer;
    extern int** palette_buffer_ptr;
    int *p = palette_buffer_ptr[active_palette_buffer];
    if(!p) {
        p = palette_buffer_ptr[0];
    }
    return (p+1);
}

//totally random
int vid_get_palette_type()
{
return 3;
}
int vid_get_palette_size()
{
return 256*4;
}

long vid_get_bitmap_buffer_height()
{
    return 360;
}

long vid_get_bitmap_buffer_width()
{
    return 720;
}

/*
void *vid_get_viewport_fb_d()
{
//sub_FF84E884
// try 32d4+0x40 and 32d4 0x48
// Maybe 0x9C instead of 0x90
//return (void*)(*(int*)(0x2108+0x9c));
return (void*)(*(int*)(0x32d4+0x40));
}
*/

void *vid_get_viewport_fb_d()
{
    extern char *viewport_fb_d;
    return viewport_fb_d;
}

void *vid_get_viewport_live_fb()
{
//sub_FF882F08
// Maybe not 0x54 but 0x48
//return (void*)(*(int*) (0x32D4 + 0x54));
//return (void*)(*(int*)(0x32d4+0x40));
//return (void*)0x40866b80;
return vid_get_viewport_fb_d();
} 

int vid_get_viewport_width()
{
return 360;
}

long vid_get_viewport_height()
{
return 240;
}

char *hook_raw_image_addr()
{
//FFB7C9F0
// Also try 40D608E0
return (char*) 0x41FF6B60;
}


// taken from a2200
void vid_bitmap_refresh()
{
    extern int full_screen_refresh;
    extern void _ScreenLock();
    extern void _ScreenUnlock();

    full_screen_refresh |= 3;
    _ScreenLock();
    _ScreenUnlock();
}

//see viewport.h
int vid_get_aspect_ratio()
{
    return 0; // 4:3
}

int vid_get_viewport_yscale()
{
return 2;
}

//Copypasted from somewhere...
#ifdef CAM_LOAD_CUSTOM_COLORS
// Function to load CHDK custom colors into active Canon palette

void load_chdk_palette() {

        extern int active_palette_buffer;
        // Only load for the standard record and playback palettes
        if ((active_palette_buffer == 0) || (active_palette_buffer == 5))
        {
                int *pal = (int*)vid_get_bitmap_active_palette();
                if (pal[CHDK_COLOR_BASE+0] != 0x33ADF62)
                {
                        pal[CHDK_COLOR_BASE+0]  = 0x33ADF62;  // Red
                        pal[CHDK_COLOR_BASE+1]  = 0x326EA40;  // Dark Red
                        pal[CHDK_COLOR_BASE+2]  = 0x34CD57F;  // Light Red
                        pal[CHDK_COLOR_BASE+3]  = 0x373BFAE;  // Green
                        pal[CHDK_COLOR_BASE+4]  = 0x34BD6CA;  // Dark Green
                        pal[CHDK_COLOR_BASE+5]  = 0x395AB95;  // Light Green
                        pal[CHDK_COLOR_BASE+6]  = 0x34766F0;  // Blue
                        pal[CHDK_COLOR_BASE+7]  = 0x31250F3;  // Dark Blue
                        pal[CHDK_COLOR_BASE+8]  = 0x37F408F;  // Cyan
                        pal[CHDK_COLOR_BASE+9]  = 0x3512D5B;  // Magenta
                        pal[CHDK_COLOR_BASE+10] = 0x3A9A917;  // Yellow
                        pal[CHDK_COLOR_BASE+11] = 0x3819137;  // Dark Yellow
                        pal[CHDK_COLOR_BASE+12] = 0x3DED115;  // Light Yellow

                        extern char palette_control;
                        palette_control = 1;
                        vid_bitmap_refresh();
        }
    }
}
#endif

#include "lolevel.h"
#include "platform.h"
#include "core.h"
#include "conf.h"
#include "keyboard.h"

typedef struct {
    short grp;
    short hackkey;
    long canonkey;
} KeyMap;

long kbd_new_state[3] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
static long kbd_prev_state[3] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
static long kbd_mod_state[3] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };

extern void _GetKbdState(long*);
extern int forced_usb_port ;

// override key and feather bits to avoid feather osd messing up chdk display in ALT mode
//#define KEYS_MASK0         (0x00008F42)
#define KEYS_MASK0         (0x0000DF42)
#define KEYS_MASK1         (0x00000000) //Logic OR of group 1 Keymap values
#define KEYS_MASK2         (0x00000033) //Logic OR of group 2 Keymap values

// Bitmap masks and physw_status index values for SD_READONLY and USB power flags (for kbd.c).
#define SD_READONLY_FLAG    0x02000000 // Found @0xffbc0ba0, levent 0x20a
#define SD_READONLY_IDX     2
#define USB_MASK            0x10000000 // Found @0xffbc0bb0, levent 0x202
#define USB_IDX             2
#define BATTCOVER_FLAG      0x04000000 // Found @0xffbc0ba8, levent 0x205
#define BATTCOVER_IDX       2

// Keymap values for kbd.c. Additional keys may be present, only common values included here.
static KeyMap keymap[] = {
    { 0, KEY_MENU            ,0x00008000 },
    { 0, KEY_HELP            ,0x00004000 },
    { 0, KEY_LEFT            ,0x00001000 },
    { 0, KEY_DOWN            ,0x00000800 },
    { 0, KEY_RIGHT           ,0x00000400 },
    { 0, KEY_UP              ,0x00000200 },
    { 0, KEY_SET             ,0x00000100 },
    { 0, KEY_PLAYBACK        ,0x00000040 },
    { 0, KEY_VIDEO           ,0x00000002 },
    { 2, KEY_ZOOM_IN         ,0x00000020 },
    { 2, KEY_ZOOM_OUT        ,0x00000010 },
    { 2, KEY_SHOOT_FULL      ,0x00000003 },
    { 2, KEY_SHOOT_FULL_ONLY ,0x00000002 },
    { 2, KEY_SHOOT_HALF      ,0x00000001 },
    { 0, 0, 0 }
};

int get_usb_bit()
{
        long usb_physw[3];
        usb_physw[USB_IDX] = 0;
        _kbd_read_keys_r2(usb_physw);
        return(( usb_physw[USB_IDX] & USB_MASK)==USB_MASK) ;
}

long __attribute__((naked,noinline)) wrap_kbd_p1_f() {
    // ixus160 100a 0xff82dce8
    asm volatile(
        "STMFD  SP!, {R1-R7,LR} \n"
        "MOV    R5, #0 \n"
        "BL     my_kbd_read_keys \n"    // patched
        "B      _kbd_p1_f_cont \n"
    );

    return 0; // shut up the compiler
}

static void __attribute__((noinline)) mykbd_task_proceed() {
    while (physw_run) {
        _SleepTask(physw_sleep_delay);
        if (wrap_kbd_p1_f() == 1) {   // autorepeat ?
            _kbd_p2_f();
        }
    }
}

void __attribute__((naked,noinline)) mykbd_task() {

    mykbd_task_proceed();
    _ExitTask();
}

void my_kbd_read_keys() {

    kbd_prev_state[0] = kbd_new_state[0];
    kbd_prev_state[1] = kbd_new_state[1];
    kbd_prev_state[2] = kbd_new_state[2];

    _GetKbdState(kbd_new_state);
    _kbd_read_keys_r2(kbd_new_state);

    if (kbd_process() == 0) {
        // we read keyboard state with _kbd_read_keys()
        physw_status[0] = kbd_new_state[0];
        physw_status[1] = kbd_new_state[1];
        physw_status[2] = kbd_new_state[2];
    } else {
        // override keys
        physw_status[0] = (kbd_new_state[0] | KEYS_MASK0) & (~KEYS_MASK0 | kbd_mod_state[0]);
        physw_status[1] = (kbd_new_state[1] | KEYS_MASK1) & (~KEYS_MASK1 | kbd_mod_state[1]);
        physw_status[2] = (kbd_new_state[2] | KEYS_MASK2) & (~KEYS_MASK2 | kbd_mod_state[2]);
    }

    #ifdef CAM_ALLOWS_USB_PORT_FORCING
        if ( forced_usb_port )  physw_status[USB_IDX] = (physw_status[USB_IDX]& ~(SD_READONLY_FLAG)) | USB_MASK ;
        else if (conf.remote_enable)
                                physw_status[USB_IDX] =  physw_status[USB_IDX] & ~(SD_READONLY_FLAG  | USB_MASK);
             else               physw_status[USB_IDX] =  physw_status[USB_IDX] & ~SD_READONLY_FLAG;
    #else
        if (conf.remote_enable) physw_status[USB_IDX] =  physw_status[USB_IDX] & ~(SD_READONLY_FLAG  | USB_MASK);
        else                    physw_status[USB_IDX] =  physw_status[USB_IDX] & ~SD_READONLY_FLAG;
    #endif

}

void kbd_key_press(long key)
{
    int i;
    for (i=0;keymap[i].hackkey;i++) {
        if (keymap[i].hackkey == key) {
            kbd_mod_state[keymap[i].grp] &= ~keymap[i].canonkey;
            return;
        }
    }
}

void kbd_key_release(long key)
{
    int i;
    for (i=0;keymap[i].hackkey;i++) {
        if (keymap[i].hackkey == key) {
            kbd_mod_state[keymap[i].grp] |= keymap[i].canonkey;
            return;
        }
    }
}

void kbd_key_release_all()
{
    kbd_mod_state[0] |= KEYS_MASK0;
    kbd_mod_state[1] |= KEYS_MASK1;
    kbd_mod_state[2] |= KEYS_MASK2;
}

long kbd_is_key_pressed(long key)
{
    int i;
    for (i=0;keymap[i].hackkey;i++) {
        if (keymap[i].hackkey == key) {
            return ((kbd_new_state[keymap[i].grp] & keymap[i].canonkey) == 0) ? 1:0;
        }
    }
    return 0;
}

long kbd_is_key_clicked(long key)
{
    int i;
    for (i=0;keymap[i].hackkey;i++) {
        if (keymap[i].hackkey == key) {
            return ((kbd_prev_state[keymap[i].grp] & keymap[i].canonkey) != 0) &&
            ((kbd_new_state[keymap[i].grp] & keymap[i].canonkey) == 0);
        }
    }
    return 0;
}

long kbd_get_pressed_key()
{
    int i;
    for (i=0;keymap[i].hackkey;i++) {
        if ((kbd_new_state[keymap[i].grp] & keymap[i].canonkey) == 0) {
            return keymap[i].hackkey;
        }
    }
    return 0;
}

long kbd_get_clicked_key()
{
    int i;
    for (i=0;keymap[i].hackkey;i++) {
        if (((kbd_prev_state[keymap[i].grp] & keymap[i].canonkey) != 0) &&
        ((kbd_new_state[keymap[i].grp] & keymap[i].canonkey) == 0)) {
        return keymap[i].hackkey;
        }
    }
    return 0;
}

// ??? Some define it as zero, some do real things
long kbd_use_zoom_as_mf() {
return 0;
}

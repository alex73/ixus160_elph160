#include "stubs_asm.h"

// guessed from sub_FF863170
DEF(recreview_hold, 0x000027E4)

// guessed from sub_FF84E884
DEF(viewport_fb_d, 0x00003314)

// guessed from sub_FF99A728 ShutterSoundTask
DEF(_nrflag, 0x75F4)

// guessed by comparing with a2500 and ixus140
DEF(active_viewport_buffer, 0x2108)

// guess by comparing with a2500 and searching for Palette Class.
DEF(active_palette_buffer, 0x7C10) // 0x7C04 + 0xC
DEF(palette_buffer_ptr, 0x7C14)

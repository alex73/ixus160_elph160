#define CAM_DRYOS         1
#define CAM_DRYOS_2_3_R39 1 // Defined for cameras with DryOS version R39 or higher
#define CAM_DRYOS_2_3_R47 1 // Defined for cameras with DryOS version R47 or higher
#define CAM_RAW_ROWPIX    5248 // Found @0xff96986c
#define CAM_RAW_ROWS      3920 // Found @0xff969874
#undef  CAM_UNCACHED_BIT
#define CAM_UNCACHED_BIT  0x40000000 // Found @0xff826238
#define CAM_DATE_FOLDER_NAMING 0x080 // Found @0xffaab6e4 (pass as 3rd param to GetImageFolder)
#define PARAM_CAMERA_NAME 3 // Found @0xffcb1568
#define MKDIR_RETURN_ONE_ON_SUCCESS

// Guessed
#define CAM_PROPSET	6

#define CAM_DNG_LENS_INFO {50,10, 400,10, 32,10, 69,10 } // See comments in camera.h
#define CAM_HAS_FILEWRITETASK_HOOK 1
#define CAM_FILEWRITETASK_SEEKS	1

// Testing
#define CAM_LOAD_CUSTOM_COLORS 1
#undef CAM_USES_ASPECT_CORRECTION
#undef CAM_BITMAP_WIDTH
#undef CAM_BITMAP_HEIGHT
#define CAM_USES_ASPECT_CORRECTION 1
#define CAM_BITMAP_WIDTH 720
#define CAM_BITMAP_HEIGHT 360

//#define DRAW_ON_ACTIVE_BITMAP_BUFFER_ONLY

#undef CAM_HAS_ERASE_BUTTON
#undef CAM_HAS_DISP_BUTTON
#undef CAM_DEFAULT_ALT_BUTTON
#define CAM_DEFAULT_ALT_BUTTON KEY_PLAYBACK
#define SHORTCUT_TOGGLE_RAW KEY_VIDEO
//#define CAM_HAS_VIDEO_BUTTON

#undef CAM_USB_EVENTID
#define CAM_USB_EVENTID 0x202

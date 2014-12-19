// Camera - ixus80_sd1100 - platform_camera.h

// This file contains the various settings values specific to the ixus80_sd1100 camera.
// This file is referenced via the 'include/camera.h' file and should not be loaded directly.

// If adding a new settings value put a suitable default in 'include/camera.h',
// along with documentation on what the setting does and how to determine the correct value.
// If the setting should not have a default value then add it in 'include/camera.h'
// using the '#undef' directive along with appropriate documentation.

// Override any default values with your camera specific values in this file. Try and avoid
// having override values that are the same as the default value.

// When overriding a setting value there are two cases:
// 1. If removing the value, because it does not apply to your camera, use the '#undef' directive.
// 2. If changing the value it is best to use an '#undef' directive to remove the default value
//    followed by a '#define' to set the new value.

// When porting CHDK to a new camera, check the documentation in 'include/camera.h'
// for information on each setting. If the default values are correct for your camera then
// don't override them again in here.

    #define CAM_PROPSET                     2
    #define CAM_DRYOS                       1

    #define CAM_RAW_ROWPIX                  3336    // for new 8 MP
    #define CAM_RAW_ROWS                    2480    // for new 8 MP

    #undef  CAM_USE_ZOOM_FOR_MF
    #undef  CAM_HAS_ERASE_BUTTON
    #define CAM_MULTIPART                   1
    #undef  CAM_HAS_IRIS_DIAPHRAGM
    #define CAM_HAS_ND_FILTER               1
    #undef  CAM_HAS_MANUAL_FOCUS
    #undef  CAM_HAS_USER_TV_MODES
    #define CAM_HAS_HI_ISO_AUTO_MODE        1
    #define CAM_SHOW_OSD_IN_SHOOT_MENU      1
    #define CAM_QUALITY_OVERRIDE            1
    // TODO / test

    #define CAM_DNG_LENS_INFO               { 62,10, 186,10, 28,10, 49,10 } // See comments in camera.h
    // pattern
    #define cam_CFAPattern                  0x01000201 // Green  Blue  Red  Green
    // color
    #define CAM_COLORMATRIX1                              \
     893969, 1000000, -343107, 1000000, -119590, 1000000, \
     -41350, 1000000,  581255, 1000000,   49523, 1000000, \
      24221, 1000000,   47554, 1000000,  209996, 1000000

    #define cam_CalibrationIlluminant1      1       // Daylight
    // cropping
    #define CAM_JPEG_WIDTH                  3264
    #define CAM_JPEG_HEIGHT                 2448
    #define CAM_ACTIVE_AREA_X1              6
    #define CAM_ACTIVE_AREA_Y1              6
    #define CAM_ACTIVE_AREA_X2              3304
    #define CAM_ACTIVE_AREA_Y2              2476
    // camera name
    #define PARAM_CAMERA_NAME               4       // parameter number for GetParameterData

    #define CAM_HAS_FILEWRITETASK_HOOK      1

//    #define REMOTE_SYNC_STATUS_LED     0xC0xxyyyy        // specifies an LED that turns on while camera waits for USB remote to sync

    #define CAM_SD_OVER_IN_AF               1
    #define CAM_SD_OVER_IN_AFL              1
    #define CAM_SD_OVER_IN_MF               1

    #define CAM_IS_VID_REC_WORKS            1   // is_video_recording() function works

//--------------------------------------------------

    #undef  CAM_DEFAULT_MENU_CURSOR_BG
    #undef  CAM_DEFAULT_MENU_CURSOR_FG
    #define CAM_DEFAULT_MENU_CURSOR_BG  IDX_COLOR_RED      // Override menu cursor colors
    #define CAM_DEFAULT_MENU_CURSOR_FG  IDX_COLOR_WHITE    // Override menu cursor colors

/*
 *   THIS FILE IS FOR DECLARATION/LISTING OF EXPORTED TO MODULES SYMBOLS
 *
 *    CHDK-FLAT Module System.  Sergey Taranenko aka tsvstar
 */

#include "camera_info.h"
#include "stdlib.h"
#include "conf.h"
#include "lang.h"
#include "keyboard.h"
#include "raw_buffer.h"
#include "gui_draw.h"
#include "gui_batt.h"
#include "gui_space.h"
#include "gui_osd.h"
#include "gui_mbox.h"
#include "gui_usb.h"
#include "raw.h"
#include "math.h"
#include "font.h"
#include "histogram.h"
#include "shot_histogram.h"
#include "levent.h"
#include "ptp_chdk.h"
#include "shooting.h"
#include "clock.h"
#include "viewport.h"
#include "debug_led.h"
#include "battery.h"
#include "properties.h"
#include "shutdown.h"
#include "sd_card.h"
#include "meminfo.h"
#include "sound.h"
#include "temperature.h"
#include "file_counter.h"
#include "backlight.h"
#include "modes.h"
#include "lens.h"
#include "action_stack.h"
#include "console.h"

#include "modules.h"
#include "module_load.h"
#include "module_exportlist.h"

extern unsigned _ExecuteEventProcedure(const char *name,...);

// ATTENTION: DO NOT USE BRACES OR OWN /**/-STYLE COMMENTS ANYWHERE IN THIS FILE TO AVOID AUTO PARSING MISTAKES


// ** SECTION 1: DEFINE UNDECLARED EXPORTED ITEMS


// ** SECTION 2: IMPLEMENTATION OF EXPORTED #define VALUES
// 	  1. PLEASE DO NOT CHANGE START AND FINAL COMMENTS TO CORRECT AUTOPARSING

/* EXPORTED_DEFINES_BEGIN */

/* EXPORTED_DEFINES_END */


// ** SECTION 3: LIST OF EXPORTED SYMBOLS (pointer to function/variable)
//	STOPLIST: open, opendir, closedir, rewinddir, readdir, stat

// This section is parsed by the makeexport.c program to generate the
// symbol hash table loaded later (from module_hashlist.h)
// Symbols to be exported should be on seperate lines, blank lines and '//' style comments are allowed

#if 0
{
            module_async_unload,
            module_set_flags,
            module_run,
            module_get_adr,
            module_async_unload_allrunned,
            module_rawop_load,
            module_curves_load,
            module_rawop_unload,
            module_mpopup_init,
            module_convert_dng_to_chdk_raw,
            module_grids_load,
            module_restore_edge,
            module_save_edge,
            module_mdetect_load,
            module_fselect_init,
            module_tbox_load,
            &module_colors,

            &altGuiHandler,
            &camera_info,
            &camera_screen,
            &camera_sensor,
            &gui_version,
            &conf,
            &dof_values,
            &libscriptapi,
            &circle_of_confusion,
            &zoom_points,
            &movie_status,

            malloc,
            free,
            umalloc,
            ufree,

            write,
            lseek,
            open,
            close,
            read,
            remove,
            rename,
            stat,
            opendir,
            readdir,
            closedir,
            mkdir,

            fopen,
            fclose,
            fseek,
            fread,
            fwrite,
            feof,
            fflush,
            fgets,
            ftell,
            SetFileAttributes,

            get_tick_count,
            time,
            utime,
            localtime,
            get_localtime,
            mktime,
            strftime,
            msleep,

            rand,
            srand,
            qsort,
            pow,
            sqrt,
            log10,
            log2,
            log,

            lang_str,
            sprintf,
            strlen,
            strcpy,
            strcat,
            strncmp,
            strpbrk,
            strchr,
            strcmp,
            strtol,
            strrchr,
            strncpy,
            strerror,
            strtoul,
            tolower,
            toupper,
            memcpy,
            memset,
            memchr,
            memcmp,
            isalnum,
            isalpha,
            iscntrl,
            islower,
            ispunct,
            isspace,
            isupper,
            isxdigit,

            draw_txt_string,
            draw_string,
            draw_rect,
            draw_filled_rect,
            draw_filled_ellipse,
            draw_filled_round_rect,
            draw_line,
            draw_char,
            draw_get_pixel,
            draw_pixel,
            draw_txt_char,
            draw_rect_thick,
            draw_filled_rect_thick,
            draw_rect_shadow,
            draw_ellipse,
            draw_set_draw_proc,
            draw_restore,

            gui_get_mode,
            gui_set_mode,
            gui_default_kbd_process_menu_btn,
            get_batt_perc,
            gui_osd_draw_clock,
            gui_mbox_init,
            gui_browser_progress_show,
            gui_enum_value_change,
            gui_set_need_restore,

            gui_osd_calc_dof,
            gui_osd_draw_dof,
            gui_batt_draw_osd,
            gui_space_draw_osd,
            gui_osd_draw_state,
            gui_osd_draw_raw_info,
            gui_osd_draw_values,
            gui_osd_draw_temp,
            gui_osd_draw_histo,
            gui_usb_draw_osd,
            gui_osd_draw_ev_video,

            vid_get_bitmap_fb,
            vid_bitmap_refresh,
            vid_get_viewport_fb,
            vid_get_viewport_fb_d,
            vid_get_viewport_live_fb,
            vid_get_viewport_height,
            vid_get_viewport_width,
            vid_get_viewport_byte_width,
            vid_get_viewport_display_xoffset,
            vid_get_viewport_display_yoffset,
            vid_get_viewport_image_offset,
            vid_get_viewport_row_offset,
            vid_get_viewport_yscale,
            vid_get_viewport_active_buffer,

            get_raw_image_addr,
            hook_raw_image_addr,
            raw_prepare_develop,
            get_raw_pixel,
            patch_bad_pixel,

            kbd_get_autoclicked_key,
            kbd_is_key_pressed,
            kbd_key_press,
            kbd_get_clicked_key,
            get_jogdial_direction,
            JogDial_CCW,
            JogDial_CW,

            debug_led,
            camera_set_led,

            mode_get,
            mode_is_video,

            shooting_get_av96,
            shooting_get_bv96,
            shooting_get_common_focus_mode,
            shooting_get_display_mode,
            shooting_get_drive_mode,
            shooting_get_ev_correction1,
            shooting_get_flash_mode,
            shooting_get_focus_mode,
            shooting_get_focus_ok,
            shooting_get_focus_state,
            shooting_get_hyperfocal_distance,
            shooting_get_is_mode,
            shooting_get_iso_market,
            shooting_get_iso_mode,
            shooting_get_iso_real,
            shooting_get_prop,
            shooting_get_real_focus_mode,
            shooting_get_resolution,
            shooting_get_subject_distance,
            shooting_get_sv96,
            shooting_get_tv96,
            shooting_get_user_av96,
            shooting_get_user_av_id,
            shooting_get_user_tv96,
            shooting_get_user_tv_id,
            shooting_get_zoom,
            shooting_in_progress,
            shooting_is_flash,
            shooting_mode_chdk2canon,
            shooting_set_av96,
            shooting_set_av96_direct,
            shooting_set_focus,
            shooting_set_iso_mode,
            shooting_set_iso_real,
            shooting_set_mode_canon,
            shooting_set_mode_chdk,
            shooting_set_nd_filter_state,
            shooting_set_prop,
            shooting_set_sv96,
            shooting_set_tv96,
            shooting_set_tv96_direct,
            shooting_set_user_av96,
            shooting_set_user_av_by_id,
            shooting_set_user_av_by_id_rel,
            shooting_set_user_tv96,
            shooting_set_user_tv_by_id,
            shooting_set_user_tv_by_id_rel,
            shooting_set_zoom,
            shooting_set_zoom_rel,
            shooting_set_zoom_speed,
            shooting_update_dof_values,

            rbf_char_width,
            rbf_font_height,
            rbf_draw_char,
            rbf_load_from_file,

            GetMemInfo,
            GetExMemInfo,

            img_prefixes,
            img_exts,
            GetTotalCardSpaceKb,
            GetFreeCardSpaceKb,
            get_exposure_counter,
            get_target_dir_name,
            GetJpgCount,
            GetRawCount,

            TurnOnBackLight,
            TurnOffBackLight,

            // Action stack functions
            action_stack_create,
            action_pop_func,
            action_top,
            action_push,
            action_push_func,
            action_push_delay,
            action_push_click,
            action_push_press,
            action_push_release,
            action_push_shoot,

            // Console functions
            console_clear,
            console_add_line,
            console_redraw,
            console_set_autoredraw,
            console_set_layout,
            script_console_add_line,
            script_print_screen_statement,

            get_focal_length,
            get_effective_focal_length,

            get_flash_params_count,
            get_parameter_size,
            get_parameter_data,
            get_property_case,
            set_property_case,

            config_save,
            config_restore,
            conf_getValue,
            conf_save,
            conf_setValue,

            DoAFLock,
            UnlockAF,

            enter_alt,
            exit_alt,

            get_battery_temp,
            get_ccd_temp,
            get_optical_temp,

            lens_get_zoom_point,
            play_sound,

            shot_histogram_get_range,
            shot_histogram_isenabled,
            shot_histogram_set,
            live_histogram_read_y,

            stat_get_vbatt,
            swap_partitions,
            get_part_count,
            get_part_type,
            get_active_partition,

            PostLogicalEventForNotPowerType,
            PostLogicalEventToUI,
            SetLogicalEventActive,
            SetScriptMode,
            errnoOfTaskGet,

            levent_set_play,
            levent_set_record,
            levent_count,
            levent_id_for_name,
            levent_index_for_id,
            levent_index_for_name,
            levent_table,

            script_colors,
            script_start_gui,
            script_end,

            reboot,
            shutdown,
            camera_shutdown_in_a_second,

            get_usb_power,
            switch_mode_usb,

            call_func_ptr,
            _ExecuteEventProcedure,

            ptp_script_create_msg,
            ptp_script_write_msg,
            ptp_script_read_msg,
            ptp_script_write_error_msg,
}
#endif

// Symbol hash table for resolving exported symbol references
sym_hash symbol_hash_table[] =
{
    { EXPORTLIST_MAGIC_NUMBER, (void*)EXPORTLIST_LAST_IDX },
#include "module_hashlist.h"
};

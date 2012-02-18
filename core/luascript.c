#include "luascript.h"
#include "kbd.h"
#include "platform.h"
#include "script.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "conf.h"
#include "shot_histogram.h"
#include "stdlib.h"
#include "raw.h"
#include "modules.h"
#include "levent.h"
#include "console.h"
#include "action_stack.h"
#include "ptp.h"
#include "core.h"
#include "gui_fselect.h"
#include "lang.h"
#include "gui_lang.h"
#include "gui_draw.h"
#include "module_load.h"

#include "../lib/lua/lstate.h"  // for L->nCcalls, baseCcalls

lua_State* L;
lua_State* Lt;

static int lua_script_is_ptp;
static int run_first_resume; // 1 first 'resume', 0 = resuming from yield
static int run_start_tick; // tick count at start of this kbd_task iteration
static int run_hook_count; // number of calls to the count hook this kbd_task iteration
#define YIELD_CHECK_COUNT 100 // check for yield every N vm instructions
#define YIELD_MAX_COUNT_DEFAULT 25 // 25 checks = 2500 vm instructions
#define YIELD_MAX_MS_DEFAULT 10
static unsigned yield_max_count;
static unsigned yield_max_ms;
static int yield_hook_enabled;

static void lua_script_disable_yield_hook(void) {
    yield_hook_enabled = 0;
}
static void lua_script_enable_yield_hook(void) {
    yield_hook_enabled = 1;
}

#ifdef CAM_CHDK_PTP
// create a ptp message from the given stack index
// incompatible types will return a TYPE_UNSUPPORTED message
static ptp_script_msg *lua_create_usb_msg( lua_State* L, int index, unsigned msgtype) {
    // TODO maybe we should just pass the lua type constants
    unsigned datatype, datasize = 4;
    int ivalue = 0;
    void *data = &ivalue;
    int ltype = lua_type(L,index);
    switch(ltype) {
        case LUA_TNONE:
            return NULL; // nothing on the stack, no message generated
        break;
        case LUA_TNIL:
            datatype = PTP_CHDK_TYPE_NIL;
        break;
        case LUA_TBOOLEAN:
            datatype = PTP_CHDK_TYPE_BOOLEAN;
            ivalue = lua_toboolean(L,index);
        break;
        case LUA_TNUMBER:
            datatype = PTP_CHDK_TYPE_INTEGER;
            ivalue = lua_tonumber(L,index);
        break;
        case LUA_TSTRING:
            datatype = PTP_CHDK_TYPE_STRING;
            data = (char *)lua_tolstring(L,index,&datasize);
        break;
        // TODO this uses usb_msg_table_to_string to serialize the table
        // the default format is described in
        // http://chdk.setepontos.com/index.php?topic=4338.msg62606#msg62606
        // other formats can be implemented by overriding this function in your lua code
        case LUA_TTABLE: {
            int result;
            lua_script_disable_yield_hook(); // don't want to yield while converting
            lua_getglobal(L, "usb_msg_table_to_string"); // push function
            lua_pushvalue(L, index); // copy specified index to top of stack
            result = lua_pcall(L,1,1,0); // this will leave an error message as a string on the stack if call fails
            lua_script_enable_yield_hook();
            if( result ) {
                // if called from lua, throw a normal error
                if( msgtype == PTP_CHDK_S_MSGTYPE_USER ) {
                    luaL_error(L,lua_tostring(L,-1));
                    return NULL; // not reached
                } else { // if it's a return, convert the message to an ERR
                    msgtype = PTP_CHDK_S_MSGTYPE_ERR;
                    datatype = PTP_CHDK_S_ERRTYPE_RUN;
                    data = (char *)lua_tolstring(L,-1,&datasize);
                    break;
                }
            }
            // an empty table is returned as an empty string by default
            // a non-string should never show up here
            if ( !lua_isstring(L,-1) ) { 
                return NULL;
            }
            datatype = PTP_CHDK_TYPE_TABLE;
            data = (char *)lua_tolstring(L,-1,&datasize);
            lua_pop(L,1);
        }
        break;
        default:
            datatype = PTP_CHDK_TYPE_UNSUPPORTED;
            data = (char *)lua_typename(L,ltype); // return type name as message data
            datasize = strlen(data);
    }
    return ptp_script_create_msg(msgtype,datatype,datasize,data);
}

void lua_script_error_ptp(int runtime, const char *err) {
    if(runtime) {
        ptp_script_write_error_msg(PTP_CHDK_S_ERRTYPE_RUN, err);
        script_end();
    } else {
        ptp_script_write_error_msg(PTP_CHDK_S_ERRTYPE_COMPILE, err);
        lua_script_reset();
    }
}
#endif

void lua_script_reset()
{
  module_rawop_unload();
  lua_close( L );
  L = 0;
}

static void lua_count_hook(lua_State *L, lua_Debug *ar)
{
    run_hook_count++;
    if( L->nCcalls > L->baseCcalls || !yield_hook_enabled )
    return;
    if(run_hook_count >= yield_max_count || get_tick_count() - run_start_tick >= yield_max_ms)
    lua_yield( L, 0 );
}

void lua_script_error(lua_State *Lt,int runtime)
{
    const char *err = lua_tostring( Lt, -1 );
    script_console_add_line( err );
    if(lua_script_is_ptp) {
#ifdef CAM_CHDK_PTP
        lua_script_error_ptp(runtime,err);
#endif
    } else {
        if(runtime) {
            if(conf.debug_lua_restart_on_error) {
                lua_script_reset();
                script_start_gui(0);
            } else {
                script_wait_and_end();
            }
        } else {
            script_print_screen_end();
            script_wait_and_end();
        }
    }
}


// TODO more stuff from script.c should be moved here
void lua_script_finish(lua_State *L) 
{
#ifdef CAM_CHDK_PTP
    if(lua_script_is_ptp) {
        // send all return values as RET messages
        int i,end = lua_gettop(L);
        for(i=1;i<=end; i++) {
            ptp_script_msg *msg = lua_create_usb_msg(L,i,PTP_CHDK_S_MSGTYPE_RET);
            // if the queue is full return values will be silently discarded
            // incompatible types will be returned as TYPE_UNSUPPORTED to preserve expected number and order of return values
            if(msg) {
                ptp_script_write_msg(msg); 
                // create_usb_msg may convert the message to an error
                if(msg->type != PTP_CHDK_S_MSGTYPE_RET) {
                    break;
                }
            } else {
                ptp_script_write_error_msg(PTP_CHDK_S_ERRTYPE_RUN, "error creating return msg");
                break;
            }
        }
    }
#endif
}

int lua_script_start( char const* script, int ptp )
{
  lua_script_is_ptp = ptp;
  L = lua_open();
  luaL_openlibs( L );
  register_lua_funcs( L );

  Lt = lua_newthread( L );
  lua_setfield( L, LUA_REGISTRYINDEX, "Lt" );
  if( luaL_loadstring( Lt, script ) != 0 ) {
    lua_script_error(Lt,0);
    return 0;
  }
  lua_sethook(Lt, lua_count_hook, LUA_MASKCOUNT, YIELD_CHECK_COUNT );
  lua_script_enable_yield_hook();
  run_first_resume = 1;
  yield_max_count = YIELD_MAX_COUNT_DEFAULT;
  yield_max_ms = YIELD_MAX_MS_DEFAULT;
  return 1;
}

// run a timeslice of lua script
void lua_script_run(void)
{
    int Lres;
    int top;
    if (run_first_resume) {
        run_first_resume = 0;
        top = 0;
    } else {
        top = lua_gettop(Lt);
    }
    run_start_tick = get_tick_count();
    run_hook_count = 0;
    Lres = lua_resume( Lt, top );

    if (Lres == LUA_YIELD) {
        // yielded
        return;
    } else if(Lres != 0) {
        lua_script_error(Lt,1);
        return;
    } else {
        // finished normally, add ptp result
        lua_script_finish(Lt);
        script_console_add_line(lang_str(LANG_CONSOLE_TEXT_FINISHED));
        action_pop();
        script_end();
    }
}

// run the "restore" function at the end of a script
void lua_run_restore()
{
	lua_getglobal(Lt, "restore");
	if (lua_isfunction(Lt, -1)) {
		if (lua_pcall( Lt, 0, 0, 0 )) {
			script_console_add_line( lua_tostring( Lt, -1 ) );
		}
	}
}

// get key ID of key name at arg, throw error if invalid
static int lua_get_key_arg( lua_State * L, int narg )
{
    int k = script_keyid_by_name( luaL_checkstring( L, narg ) );
    if(!k) 
        luaL_error( L, "unknown key" );
    return k;
}

#ifdef OPT_CURVES
#include "curves.h"

static int luaCB_set_curve_state( lua_State* L )
{
  int value;
  value=luaL_checknumber( L, 1 );

  if ( libcurves && libcurves->curve_set_mode)
		libcurves->curve_set_mode(value);

  return 0;
}

static int luaCB_get_curve_state( lua_State* L )
{
  lua_pushnumber(L,conf.curve_enable);
  return 1;
}

static int luaCB_set_curve_file( lua_State* L )
{
  size_t l;
  const char *s = luaL_checklstring(L, 1, &l);
  if ( libcurves && libcurves->curve_set_file)
		libcurves->curve_set_file(s);
  return 0;
}

static int luaCB_get_curve_file( lua_State* L )
{
  lua_pushstring(L,conf.curve_file);
  return 1;
}
#endif

static int luaCB_set_aflock(lua_State* L) 
{
  int val = luaL_checknumber(L, 1);
  if (val>0) DoAFLock();  // 1: enable AFLock
  else UnlockAF();       // 0: disable unlock AF
  return 0;
}


static int luaCB_shoot( lua_State* L )
{
  action_push(AS_SHOOT);
  return lua_yield( L, 0 );
}

static int luaCB_sleep( lua_State* L )
{
  action_push_delay( luaL_checknumber( L, 1 ) );
  return lua_yield( L, 0 );
}

// for press,release and click
static int luaCB_keyfunc( lua_State* L )
{
  void* func = lua_touserdata( L, lua_upvalueindex(1) );
  ((void(*)(long))func)( lua_get_key_arg( L, 1 ) );
  return lua_yield( L, 0 );
}

static int luaCB_cls( lua_State* L )
{
  console_clear();
  return 0;
}

static int luaCB_set_console_layout( lua_State* L )
{
  console_set_layout(luaL_checknumber( L, 1 ),luaL_checknumber( L, 2 ),luaL_checknumber( L, 3 ),luaL_checknumber( L, 4 ));
  return 0;
}

static int luaCB_set_console_autoredraw( lua_State* L )
{
  console_set_autoredraw(luaL_checknumber( L, 1 ));
  return 0;
}

static int luaCB_console_redraw( lua_State* L )
{
  console_redraw();
  return 0;
}
static int luaCB_get_av96( lua_State* L )
{
  lua_pushnumber( L, shooting_get_av96() );
  return 1;
}

static int luaCB_get_bv96( lua_State* L )
{
  lua_pushnumber( L, shooting_get_bv96() );
  return 1;
}

static int luaCB_get_day_seconds( lua_State* L )
{
  lua_pushnumber( L, shooting_get_day_seconds() );
  return 1;
}

static int luaCB_get_disk_size( lua_State* L )
{
  lua_pushnumber( L, GetTotalCardSpaceKb() );
  return 1;
}

static int luaCB_get_dof( lua_State* L )
{
  lua_pushnumber( L, shooting_get_depth_of_field() );
  return 1;
}

static int luaCB_get_far_limit( lua_State* L )
{
  lua_pushnumber( L, shooting_get_far_limit_of_acceptable_sharpness() );
  return 1;
}

static int luaCB_get_free_disk_space( lua_State* L )
{
  lua_pushnumber( L, GetFreeCardSpaceKb() );
  return 1;
}

static int luaCB_get_focus( lua_State* L )
{
  lua_pushnumber( L, shooting_get_subject_distance() );
  return 1;
}

static int luaCB_get_hyp_dist( lua_State* L )
{
  lua_pushnumber( L, shooting_get_hyperfocal_distance() );
  return 1;
}

static int luaCB_get_iso_market( lua_State* L )
{
  lua_pushnumber( L, shooting_get_iso_market() );
  return 1;
}

static int luaCB_get_iso_mode( lua_State* L )
{
  lua_pushnumber( L, shooting_get_iso_mode() );
  return 1;
}

static int luaCB_get_iso_real( lua_State* L )
{
  lua_pushnumber( L, shooting_get_iso_real() );
  return 1;
}

static int luaCB_get_jpg_count( lua_State* L )
{
  lua_pushnumber( L, GetJpgCount() );
  return 1;
}

static int luaCB_get_near_limit( lua_State* L )
{
  lua_pushnumber( L, shooting_get_near_limit_of_acceptable_sharpness() );
  return 1;
}

/*
val=get_prop(id)
get propcase value identified by id
the propcase is read as a short and sign extended to an int
*/
static int luaCB_get_prop( lua_State* L )
{
  lua_pushnumber( L, shooting_get_prop( luaL_checknumber( L, 1 ) ) );
  return 1;
}

/*
val=get_prop_str(prop_id,length)
get the value of a propertycase as a string
numeric values may be extracted using string.byte or or the binstr.lua module
returns the value as a string, or false if the underlying propcase call returned non-zero
*/
static int luaCB_get_prop_str( lua_State* L ) {
    void *buf;
    unsigned size;
    unsigned prop_id = luaL_checknumber( L, 1 );
    size = luaL_checknumber( L, 2 );
    buf = malloc(size);
    if(!buf) {
        return luaL_error( L, "malloc failed in luaCB_get_prop" );
    }
    if(get_property_case(prop_id,buf,size) == 0) {
        lua_pushlstring( L, buf, size );
    } else {
        lua_pushboolean( L, 0);
    }
    free(buf);
    return 1;
}

static int luaCB_get_raw_count( lua_State* L )
{
  lua_pushnumber( L, GetRawCount() );
  return 1;
}

static int luaCB_get_sv96( lua_State* L )
{
  lua_pushnumber( L, shooting_get_sv96() );
  return 1;
}

static int luaCB_get_tick_count( lua_State* L )
{
  lua_pushnumber( L, shooting_get_tick_count() );
  return 1;
}

static int luaCB_get_exp_count( lua_State* L )
{
  lua_pushnumber( L, get_exposure_counter() );
  return 1;
}

static int luaCB_get_tv96( lua_State* L )
{
  lua_pushnumber( L, shooting_get_tv96() );
  return 1;
}

static int luaCB_get_user_av_id( lua_State* L )
{
  lua_pushnumber( L, shooting_get_user_av_id() );
  return 1;
}

static int luaCB_get_user_av96( lua_State* L )
{
  lua_pushnumber( L, shooting_get_user_av96() );
  return 1;
}

static int luaCB_get_user_tv_id( lua_State* L )
{
  lua_pushnumber( L, shooting_get_user_tv_id() );
  return 1;
}

static int luaCB_get_user_tv96( lua_State* L )
{
  lua_pushnumber( L, shooting_get_user_tv96() );
  return 1;
}

static int luaCB_get_vbatt( lua_State* L )
{
  lua_pushnumber( L, stat_get_vbatt() );
  return 1;
}

static int luaCB_get_zoom( lua_State* L )
{
  lua_pushnumber( L, shooting_get_zoom() );
  return 1;
}

static int luaCB_get_parameter_data( lua_State* L )
{
  extern long* FlashParamsTable[]; 

  unsigned size;
  unsigned id = luaL_checknumber( L, 1 );
  unsigned val;

  if (id >= get_flash_params_count()) {
    // return nil
    return 0;
  }

  size = FlashParamsTable[id][1]>>16;
  if (size == 0) {
    // return nil
    return 0;
  }
  if (size >= 1 && size <= 4) {
    val = 0;
    get_parameter_data( id, &val, size );
    lua_pushlstring( L, (char *)&val, size );
    // for convenience, params that fit in a number are returned in one as a second result
    lua_pushnumber( L, val );
    return 2;
  }
  else {
    char *buf = malloc(size);
    if(!buf) {
      luaL_error( L, "malloc failed in luaCB_get_parameter_data" );
    }
    get_parameter_data( id, buf, size );
    lua_pushlstring( L, buf, size );
    free(buf);
    return 1;
  }
}

static int luaCB_get_flash_params_count( lua_State* L )
{
  lua_pushnumber( L, get_flash_params_count() );
  return 1;
}

static int luaCB_set_av96_direct( lua_State* L )
{
  shooting_set_av96_direct( luaL_checknumber( L, 1 ), SET_LATER );
  return 0;
}

static int luaCB_set_av96( lua_State* L )
{
  shooting_set_av96( luaL_checknumber( L, 1 ), SET_LATER );
  return 0;
}

static int luaCB_set_focus( lua_State* L )
{
    int to = luaL_checknumber( L, 1 );
    int m=mode_get()&MODE_SHOOTING_MASK;
    int mode_video=MODE_IS_VIDEO(m);

#if CAM_HAS_MANUAL_FOCUS
    if (shooting_get_focus_mode() || (mode_video)) shooting_set_focus(to, SET_NOW);
    else shooting_set_focus(to, SET_LATER);
#else
    if (shooting_get_common_focus_mode() || mode_video) shooting_set_focus(to, SET_NOW);
    else shooting_set_focus(to, SET_LATER);    
#endif    
  return 0;
}

static int luaCB_set_iso_mode( lua_State* L )
{
  shooting_set_iso_mode( luaL_checknumber( L, 1 ) );
  return 0;
}

static int luaCB_set_iso_real( lua_State* L )
{
  shooting_set_iso_real( luaL_checknumber( L, 1 ), SET_LATER);
  return 0;
}

static int luaCB_set_led( lua_State* L )
{
  int to, to1, to2;
  to = luaL_checknumber( L, 1 );
  to1 = luaL_checknumber( L, 2 );
  to2 = 200;
  if( lua_isnumber( L, 3 ) )
    to2 = lua_tonumber( L, 3 );
  camera_set_led(to, to1, to2);
  return 0;
}

static int luaCB_set_nd_filter( lua_State* L )
{
  shooting_set_nd_filter_state( luaL_checknumber( L, 1 ), SET_LATER);
  return 0;
}

/*
set_prop(id,value)
the value is treated as a short
*/
static int luaCB_set_prop( lua_State* L )
{
  shooting_set_prop(luaL_checknumber( L, 1 ), luaL_checknumber( L, 2 ));
  return 0;
}

/*
status=set_prop_str(prop_id,value)
set propertycase value as a string. Length is taken from the string
numeric propcase values may be assembled by setting byte values using string.char or the binstr module
status: boolean - true if the underlying propcase call returns 0, otherwise false
*/
static int luaCB_set_prop_str( lua_State *L ) {
    int prop_id;
    unsigned len;
    const char *str;
    prop_id = luaL_checknumber( L, 1 );
    str = luaL_checklstring( L, 2, &len );
    if(str && len > 0) {
        lua_pushboolean( L, (set_property_case(prop_id,(void *)str,len) == 0));
    } else {
        return luaL_error( L, "invalid value");
    }
    return 1;
}

static int luaCB_set_raw_nr( lua_State* L )
{
  camera_set_nr(luaL_checknumber( L, 1 ));
  return 0;
}

static int luaCB_get_raw_nr( lua_State* L )
{
  lua_pushnumber( L, camera_get_nr() );
  return 1;
}

static int luaCB_set_raw( lua_State* L )
{
  camera_set_raw(luaL_checknumber( L, 1 ));
  return 0;
}

static int luaCB_get_raw( lua_State* L )
{
  lua_pushnumber( L, conf.save_raw );
  return 1;
}

static int luaCB_set_sv96( lua_State* L )
{
  shooting_set_sv96(luaL_checknumber( L, 1 ), SET_LATER);
  return 0;
}

static int luaCB_set_tv96_direct( lua_State* L )
{
  shooting_set_tv96_direct(luaL_checknumber( L, 1 ), SET_LATER);
  return 0;
}

static int luaCB_set_tv96( lua_State* L )
{
  shooting_set_tv96(luaL_checknumber( L, 1 ), SET_LATER);
  return 0;
}

static int luaCB_set_user_av_by_id_rel( lua_State* L )
{
  shooting_set_user_av_by_id_rel(luaL_checknumber( L, 1 ));
  return 0;
}

static int luaCB_set_user_av_by_id( lua_State* L )
{
  shooting_set_user_av_by_id(luaL_checknumber( L, 1 ));
  return 0;
}

static int luaCB_set_user_av96( lua_State* L )
{
  shooting_set_user_av96(luaL_checknumber( L, 1 ));
  return 0;
}

static int luaCB_set_user_tv_by_id_rel( lua_State* L )
{
  shooting_set_user_tv_by_id_rel(luaL_checknumber( L, 1 ));
  return 0;
}

static int luaCB_set_user_tv_by_id( lua_State* L )
{
  shooting_set_user_tv_by_id(luaL_checknumber( L, 1 ));
  return 0;
}

static int luaCB_set_user_tv96( lua_State* L )
{
  shooting_set_user_tv96(luaL_checknumber( L, 1 ));
  return 0;
}

static int luaCB_set_zoom_speed( lua_State* L )
{
  shooting_set_zoom_speed(luaL_checknumber( L, 1 ));
  return 0;
}

static int luaCB_set_zoom_rel( lua_State* L )
{
  shooting_set_zoom_rel(luaL_checknumber( L, 1 ));
  return 0;
}

static int luaCB_set_zoom( lua_State* L )
{
  shooting_set_zoom(luaL_checknumber( L, 1 ));
  return 0;
}

static int luaCB_wait_click( lua_State* L )
{
  int timeout = luaL_optnumber( L, 1, 0 );
  action_wait_for_click(timeout);
  return lua_yield( L, 0 );
}

static int luaCB_is_pressed( lua_State* L )
{
  lua_pushboolean( L, script_key_is_pressed(lua_get_key_arg( L, 1 )));
  return 1;
}

static int luaCB_is_key( lua_State* L )
{
  lua_pushboolean( L, script_key_is_clicked(lua_get_key_arg( L, 1 )));
  return 1;
}

#if CAM_HAS_JOGDIAL
static int luaCB_wheel_right( lua_State* L )
{
  JogDial_CW();
  return 0;
}

static int luaCB_wheel_left( lua_State* L )
{
  JogDial_CCW();
  return 0;
}
#endif

static int luaCB_md_get_cell_diff( lua_State* L )
{
    if (module_mdetect_load())
        lua_pushnumber( L, libmotiondetect->md_get_cell_diff(luaL_checknumber(L,1), luaL_checknumber(L,2)));
    else
        lua_pushnumber( L, 0 );
    return 1;
}

static int luaCB_md_detect_motion( lua_State* L )
{
  int columns = (luaL_optnumber(L,1,6));
  int rows = (luaL_optnumber(L,2,4));
  int pixel_measure_mode = (luaL_optnumber(L,3,1));
  int detection_timeout = (luaL_optnumber(L,4,10000));
  int measure_interval = (luaL_optnumber(L,5,7));
  int threshold = (luaL_optnumber(L,6,10));
  int draw_grid = (luaL_optnumber(L,7,1));
   // arg 8 is the return value in ubasic. We
   // ignore it here. - AUJ
  int clipping_region_mode = (luaL_optnumber(L,9,0));
  int clipping_region_column1 = (luaL_optnumber(L,10,0));
  int clipping_region_row1 = (luaL_optnumber(L,11,0));
  int clipping_region_column2 = (luaL_optnumber(L,12,0));
  int clipping_region_row2 = (luaL_optnumber(L,13,0));
  int parameters = (luaL_optnumber(L,14,1));
  int pixels_step = (luaL_optnumber(L,15,6));
  int msecs_before_trigger = (luaL_optnumber(L,16,0));
  if (module_mdetect_load() && libmotiondetect->md_init_motion_detector(
    columns, rows, pixel_measure_mode, detection_timeout, 
    measure_interval, threshold, draw_grid,
    clipping_region_mode,
    clipping_region_column1, clipping_region_row1,
    clipping_region_column2, clipping_region_row2,
    parameters, pixels_step, msecs_before_trigger
  ))
    return lua_yield(L, 0);
  else
    return luaL_error( L, "md_init_motion_detector failed" );
}

static void return_string_selected(const char *str) {
    // Reconnect button input to script - will also signal action stack
    // that file browser / textbox is finished and return last selected file
    // to script caller
    state_kbd_script_run = 1;
    // Clear the Func/Set key so that when the script exits, pressing
    // the Func/Set key again will enter the Script menu, not the File Browser / Textbox
    kbd_reset_autoclicked_key();

    // Push selected file as script return value
	lua_pushstring( Lt, (str && str[0])? str : NULL );
}

static int luaCB_file_browser( lua_State* L ) {
    // Disconnect button input from script so buttons will work in file browser
    state_kbd_script_run = 0;
    // Push file browser action onto stack - will loop doing nothing until file browser exits
    action_push(AS_FILE_BROWSER);
    // Switch to file browser gui mode. Path can be supplied in call or defaults to "A" (root directory).
    module_fselect_init(LANG_STR_FILE_BROWSER, luaL_optstring( L, 1, "A" ), "A", return_string_selected);
    // Yield the script so that the action stack will process the AS_FILE_BROWSER action
    return lua_yield(L, 0);
}

static int luaCB_textbox( lua_State* L ) {
    // Disconnect button input from script so buttons will work in the textbox
    state_kbd_script_run = 0;
    if (module_tbox_load())
    {
        // Push textbox action onto stack - will loop doing nothing until textbox exits
        action_push(AS_TEXTBOX);
        // Switch to textbox gui mode. Text box prompt should be passed as param.
        module_tbox_load()->textbox_init((int)luaL_optstring( L, 1, "Text box" ),   //title
                                         (int)luaL_optstring( L, 2, "Enter text" ), //message
                                         luaL_optstring( L, 3, ""  ),               //default string
                                         luaL_optnumber( L, 4, 30),                 //max size of a text
                                         return_string_selected, 0);
    }
    else
        return_string_selected(0);

    // Yield the script so that the action stack will process the AS_TEXTBOX action
    return lua_yield(L, 0);
}

// begin lua draw fuctions
unsigned char script_colors[][2]  = {

#ifdef CAM_USE_COLORED_ICONS
                                        {COLOR_TRANSPARENT,         COLOR_TRANSPARENT},         //  1   trans
                                        {COLOR_BLACK,               COLOR_BLACK},               //  2   black
                                        {COLOR_WHITE,               COLOR_WHITE},               //  3   white
                                        
                                        {COLOR_ICON_PLY_RED,        COLOR_ICON_REC_RED},        //  4   red
                                        {COLOR_ICON_PLY_RED_DK,     COLOR_ICON_REC_RED_DK},     //  5   red_dark
                                        {COLOR_ICON_PLY_RED_LT,     COLOR_ICON_REC_RED_LT},     //  6   red_light
                                        {COLOR_ICON_PLY_GREEN,      COLOR_ICON_REC_GREEN},      //  7   green
                                        {COLOR_ICON_PLY_GREEN_DK,   COLOR_ICON_REC_GREEN_DK},   //  8   green_dark
                                        {COLOR_ICON_PLY_GREEN_LT,   COLOR_ICON_REC_GREEN_LT},   //  9   green_light
                                        {COLOR_HISTO_B_PLAY,        COLOR_HISTO_B},             //  10  blue
                                        {COLOR_HISTO_B_PLAY,        COLOR_HISTO_B},             //  11  blue_dark   - placeholder
                                        {COLOR_HISTO_B_PLAY,        COLOR_HISTO_B},             //  12  blue_light  - placeholder

                                        {COLOR_ICON_PLY_GREY,       COLOR_ICON_REC_GREY},       //  13  grey
                                        {COLOR_ICON_PLY_GREY_DK,    COLOR_ICON_REC_GREY_DK},    //  14  grey_dark
                                        {COLOR_ICON_PLY_GREY_LT,    COLOR_ICON_REC_GREY_LT},    //  15  grey_light

                                        {COLOR_ICON_PLY_YELLOW,     COLOR_ICON_REC_YELLOW},     //  16  yellow
                                        {COLOR_ICON_PLY_YELLOW_DK,  COLOR_ICON_REC_YELLOW_DK},  //  17  yellow_dark
                                        {COLOR_ICON_PLY_YELLOW_LT,  COLOR_ICON_REC_YELLOW_LT}   //  18  yellow_light
#else
                                        {COLOR_TRANSPARENT,         COLOR_TRANSPARENT},         //  1   trans
                                        {COLOR_BLACK,               COLOR_BLACK},               //  2   black
                                        {COLOR_WHITE,               COLOR_WHITE},               //  3   white
                                        
                                        {COLOR_HISTO_R_PLAY,        COLOR_HISTO_R},             //  4   red
                                        {COLOR_HISTO_R_PLAY,        COLOR_HISTO_R},             //  5   red_dark    - placeholder
                                        {COLOR_HISTO_R_PLAY,        COLOR_HISTO_R},             //  6   red_light   - placeholder
                                        {COLOR_HISTO_G_PLAY,        COLOR_HISTO_G},             //  7   green
                                        {COLOR_HISTO_G_PLAY,        COLOR_HISTO_G},             //  8   green_dark  - placeholder
                                        {COLOR_HISTO_G_PLAY,        COLOR_HISTO_G},             //  9   green_light - placeholder
                                        {COLOR_HISTO_B_PLAY,        COLOR_HISTO_B},             //  10  blue
                                        {COLOR_HISTO_B_PLAY,        COLOR_HISTO_B},             //  11  blue_dark   - placeholder
                                        {COLOR_HISTO_B_PLAY,        COLOR_HISTO_B},             //  12  blue_light  - placeholder

                                        {COLOR_BLACK,               COLOR_BLACK},               //  13  grey        - placeholder (there's no universal grey)
                                        {COLOR_BLACK,               COLOR_BLACK},               //  14  grey_dark   - placeholder (there's no universal grey)
                                        {COLOR_WHITE,               COLOR_WHITE},               //  15  grey_light  - placeholder (there's no universal grey)

                                        {COLOR_WHITE,               COLOR_WHITE},               //  16  yellow      - placeholder
                                        {COLOR_WHITE,               COLOR_WHITE},               //  17  yellow_dark - placeholder
                                        {COLOR_WHITE,               COLOR_WHITE}                //  18  yellow_light- placeholder
#endif
                                    };

static int get_color(int cl) {
    char out=0;                     //defaults to 0 if any wrong number

    if (cl<256)
        out=cl;
    else {
        if (cl-256<sizeof(script_colors)) {
            if((mode_get()&MODE_MASK) == MODE_REC)
                out=script_colors[cl-256][1];
            else
                out=script_colors[cl-256][0];
        }
    }
    return out;
}

static int luaCB_draw_pixel( lua_State* L ) {
  coord x1=luaL_checknumber(L,1);
  coord y1=luaL_checknumber(L,2);
  color cl=get_color(luaL_checknumber(L,3));
  draw_pixel(x1,y1,cl);
  return 1;
}

static int luaCB_draw_line( lua_State* L ) {
  coord x1=luaL_checknumber(L,1);
  coord y1=luaL_checknumber(L,2);
  coord x2=luaL_checknumber(L,3);
  coord y2=luaL_checknumber(L,4);
  color cl=get_color(luaL_checknumber(L,5));
  draw_line(x1,y1,x2,y2,cl);
  return 1;
}

static int luaCB_draw_rect( lua_State* L ) {
  coord x1=luaL_checknumber(L,1);
  coord y1=luaL_checknumber(L,2);
  coord x2=luaL_checknumber(L,3);
  coord y2=luaL_checknumber(L,4);
  color cl=get_color(luaL_checknumber(L,5));
  int   th=luaL_optnumber(L,6,1);
  draw_rect_thick(x1,y1,x2,y2,cl,th);
  return 1;
}

static int luaCB_draw_rect_filled( lua_State* L ) {
  coord x1 =luaL_checknumber(L,1);
  coord y1 =luaL_checknumber(L,2);
  coord x2 =luaL_checknumber(L,3);
  coord y2 =luaL_checknumber(L,4);
  color clf=get_color(luaL_checknumber(L,5));
  color clb=get_color(luaL_checknumber(L,6));
  int   th =luaL_optnumber(L,7,1);
  clf=256*clb+clf;
  draw_filled_rect_thick(x1,y1,x2,y2,clf,th);
  return 1;
}

static int luaCB_draw_ellipse( lua_State* L ) {
  coord x1=luaL_checknumber(L,1);
  coord y1=luaL_checknumber(L,2);
  coord a=luaL_checknumber(L,3);
  coord b=luaL_checknumber(L,4);
  color cl=get_color(luaL_checknumber(L,5));
  draw_ellipse(x1,y1,a,b,cl);
  return 1;
}

static int luaCB_draw_ellipse_filled( lua_State* L ) {
  coord x1=luaL_checknumber(L,1);
  coord y1=luaL_checknumber(L,2);
  coord a=luaL_checknumber(L,3);
  coord b=luaL_checknumber(L,4);
  color cl=256*get_color(luaL_checknumber(L,5));
  draw_filled_ellipse(x1,y1,a,b,cl);
  return 1;
}

static int luaCB_draw_string( lua_State* L ) {
  coord x1=luaL_checknumber(L,1);
  coord y1=luaL_checknumber(L,2);
  const char *t = luaL_checkstring( L, 3 );
  color clf=get_color(luaL_checknumber(L,4));
  color clb=get_color(luaL_checknumber(L,5));
  clf=256*clb+clf;
  draw_string(x1,y1,t,clf);
  return 1;
}

static int luaCB_draw_clear( lua_State* L ) {
  draw_restore();
  return 1;
}
// end lua draw functions

static int luaCB_autostarted( lua_State* L )
{
  lua_pushboolean( L, camera_get_script_autostart() );
  return 1;
}

static int luaCB_get_autostart( lua_State* L )
{
  lua_pushnumber( L, conf.script_startup );
  return 1;
}

static int luaCB_set_autostart( lua_State* L )
{
  int to;
  to = luaL_checknumber( L, 1 );
  if ( to >= 0 && to <= 2 ) conf.script_startup = to;
  conf_save();
  return 0;
}

static int luaCB_get_usb_power( lua_State* L )
{
  lua_pushnumber( L, get_usb_power(luaL_optnumber( L, 1, 0 )) );
  return 1;
}

static int luaCB_enter_alt( lua_State* L )
{
  enter_alt();
  return 0;
}

static int luaCB_exit_alt( lua_State* L )
{
  exit_alt();
  return 0;
}

// optional parameter is 0 for soft shutdown (default) or 1 for hard/immediate
static int luaCB_shut_down( lua_State* L )
{
  if ( luaL_optnumber(L,1,0) == 1 )
  {
    shutdown();
  } else {
  camera_shutdown_in_a_second();
  }
  return 0;
}

static int luaCB_print_screen( lua_State* L )
{
  
  if (lua_isboolean( L, 1 ))
    script_print_screen_statement( lua_toboolean( L, 1 ) );
  else
    script_print_screen_statement( luaL_checknumber( L, 1 )+10000 );
  return 0;
}

static int luaCB_get_movie_status( lua_State* L )
{
  lua_pushnumber( L, movie_status );
  return 1;
}

static int luaCB_set_movie_status( lua_State* L )
{
  switch(luaL_checknumber( L, 1 )) {
    case 1:
      if (movie_status == 4) {
        movie_status = 1;
      }
    break;
    case 2:
      if (movie_status == 1) {
        movie_status = 4;
      }
    break;
    case 3:
      if (movie_status == 1 || movie_status == 4) {
        movie_status = 5;
      }
    break;
  }
  return 0;
}

static int luaCB_get_drive_mode( lua_State* L )
{
  lua_pushnumber( L, shooting_get_drive_mode() );
  return 1;
}

static int luaCB_get_focus_mode( lua_State* L )
{
  lua_pushnumber( L, shooting_get_real_focus_mode() );
  return 1;
}

static int luaCB_get_focus_state( lua_State* L )
{
  lua_pushnumber( L, shooting_get_focus_state() );
  return 1;
}

static int luaCB_get_focus_ok( lua_State* L )
{
  lua_pushnumber( L, shooting_get_focus_ok() );
  return 1;
}

static int luaCB_get_flash_mode( lua_State* L )
{
  lua_pushnumber( L, shooting_get_flash_mode() );
  return 1;
}

static int luaCB_get_shooting( lua_State* L )
{
  lua_pushboolean( L, shooting_in_progress() );
  return 1;
}

static int luaCB_get_flash_ready( lua_State* L )
{
  lua_pushboolean( L, shooting_is_flash() );
  return 1;
}

static int luaCB_get_IS_mode( lua_State* L )
{
  lua_pushnumber( L, shooting_get_is_mode() );
  return 1;
}

static int luaCB_get_orientation_sensor( lua_State* L )
{
  lua_pushnumber( L, shooting_get_prop(PROPCASE_ORIENTATION_SENSOR) );
  return 1;
}

static int luaCB_get_zoom_steps( lua_State* L )
{
  lua_pushnumber( L, zoom_points );
  return 1;
}

static int luaCB_get_nd_present( lua_State* L )
{
  int to;
  #if !CAM_HAS_ND_FILTER
  to = 0;
  #endif
  #if CAM_HAS_ND_FILTER && !CAM_HAS_IRIS_DIAPHRAGM
  to = 1;
  #endif
  #if CAM_HAS_ND_FILTER && CAM_HAS_IRIS_DIAPHRAGM
  to = 2;
  #endif
  lua_pushnumber( L, to );
  return 1;
}

static int luaCB_get_propset( lua_State* L )
{
  lua_pushnumber( L, CAM_PROPSET );
  return 1;
}

static int luaCB_get_ev( lua_State* L )
{
  lua_pushnumber( L, shooting_get_ev_correction1() );
  return 1;
}

static int luaCB_set_ev( lua_State* L )
{
  int to;
  to = luaL_checknumber( L, 1 );
  shooting_set_prop(PROPCASE_EV_CORRECTION_1, to);
  shooting_set_prop(PROPCASE_EV_CORRECTION_2, to);
  return 0;
}

static int luaCB_get_histo_range( lua_State* L )
{
  int from = (luaL_checknumber(L,1));
  int to = (luaL_checknumber(L,2));
  if (shot_histogram_isenabled()) lua_pushnumber( L, shot_histogram_get_range(from, to) );
  else lua_pushnumber( L, -1 ); // TODO should probably return nil 
  return 1;
}

static int luaCB_shot_histo_enable( lua_State* L )
{
  shot_histogram_set(luaL_checknumber( L, 1 ));
  return 0;
}

static int luaCB_play_sound( lua_State* L )
{
  play_sound(luaL_checknumber( L, 1 ));
  return 0;
}

static int luaCB_get_temperature( lua_State* L )
{
  int which = (luaL_checknumber( L, 1 ));
  int temp = -100; // do something insane if users passes bad value
  switch (which)
  {
    case 0:
      temp = get_optical_temp(); 
      break;
    case 1:
      temp = get_ccd_temp(); 
      break;
    case 2:
      temp = get_battery_temp();
      break;
  }
  lua_pushnumber( L, temp );
  return 1;
}

static int luaCB_get_time( lua_State* L )
{
  int r = -1;
  static struct tm *ttm;
  ttm = get_localtime();
  const char *t = luaL_checkstring( L, 1 );
  if (strncmp("s", t, 1)==0) r = ( L, ttm->tm_sec );
  else if (strncmp("m", t, 1)==0) r = ( L, ttm->tm_min );
  else if (strncmp("h", t, 1)==0) r = ( L, ttm->tm_hour );
  else if (strncmp("D", t, 1)==0) r = ( L, ttm->tm_mday );
  else if (strncmp("M", t, 1)==0) r = ( L, ttm->tm_mon+1 );
  else if (strncmp("Y", t, 1)==0) r = ( L, 1900+ttm->tm_year );
  lua_pushnumber( L, r );
  return 1;
}
/*
  val=peek(address[,size])
  return the value found at address in memory, or nil if address or size is invalid
  size is optional 1=byte 2=halfword 4=word. defaults is 4
*/
static int luaCB_peek( lua_State* L )
{
  unsigned addr = luaL_checknumber(L,1);
  unsigned size = luaL_optnumber(L, 2, 4);
  switch(size) {
    case 1: 
      lua_pushnumber( L, *(unsigned char *)(addr) );
    break;
    case 2:
      if (addr & 0x1) {
        lua_pushnil(L);
      }
      else {
        lua_pushnumber( L, *(unsigned short *)(addr) );
      }
    break;
    case 4:
      if (addr & 0x3) {
        lua_pushnil(L);
      }
      else {
        lua_pushnumber( L, *(unsigned *)(addr) );
      }
    break;
    default:
      lua_pushnil(L);

  }
  return 1;
}

/*
  status=poke(address,value[,size])
  writes value to address in memory
  size is optional 1=byte 2=halfword 4=word. defaults is 4
  returns true, or nil if address or size is invalid
*/
static int luaCB_poke( lua_State* L )
{
  unsigned addr = luaL_checknumber(L,1);
  unsigned val = luaL_checknumber(L,2);
  unsigned size = luaL_optnumber(L, 3, 4);
  int status = 0;
  switch(size) {
    case 1: 
        *(unsigned char *)(addr) = (unsigned char)val;
        status=1;
    break;
    case 2:
      if (!(addr & 0x1)) {
        *(unsigned short *)(addr) = (unsigned short)val;
        status=1;
      }
    break;
    case 4:
      if (!(addr & 0x3)) {
        *(unsigned *)(addr) = val;
        status=1;
      }
    break;
  }
  if(status) {
    lua_pushboolean(L,1);
  }
  else {
    lua_pushnil(L);
  }
  return 1;
}

static int luaCB_bitand( lua_State* L )
{
  int v1 = (luaL_checknumber(L,1));
  int v2 = (luaL_checknumber(L,2));
  lua_pushnumber( L, v1 & v2 );
  return 1;
}

static int luaCB_bitor( lua_State* L )
{
  int v1 = (luaL_checknumber(L,1));
  int v2 = (luaL_checknumber(L,2));
  lua_pushnumber( L, v1 | v2 );
  return 1;
}

static int luaCB_bitxor( lua_State* L )
{
  int v1 = (luaL_checknumber(L,1));
  int v2 = (luaL_checknumber(L,2));
  lua_pushnumber( L, v1 ^ v2 );
  return 1;
}

static int luaCB_bitshl( lua_State* L )
{
  int val = (luaL_checknumber(L,1));
  unsigned shift = (luaL_checknumber(L,2));
  lua_pushnumber( L, val << shift );
  return 1;
}

static int luaCB_bitshri( lua_State* L )
{
  int val = (luaL_checknumber(L,1));
  unsigned shift = (luaL_checknumber(L,2));
  lua_pushnumber( L, val >> shift );
  return 1;
}

static int luaCB_bitshru( lua_State* L )
{
  unsigned val = (luaL_checknumber(L,1));
  unsigned shift = (luaL_checknumber(L,2));
  lua_pushnumber( L, val >> shift );
  return 1;
}

static int luaCB_bitnot( lua_State* L )
{
  unsigned val = (luaL_checknumber(L,1));
  lua_pushnumber( L, ~val );
  return 1;
}

static void set_string_field(lua_State* L, const char *key, const char *val)
{
  lua_pushstring(L, val);
  lua_setfield(L, -2, key);
}

static void set_number_field(lua_State* L, const char *key, int val)
{
  lua_pushnumber(L, val);
  lua_setfield(L, -2, key);
}

static int luaCB_get_buildinfo( lua_State* L )
{
  lua_createtable(L, 0, 8);
  set_string_field( L,"platform", PLATFORM );
  set_string_field( L,"platsub", PLATFORMSUB );
  set_string_field( L,"version", HDK_VERSION );
  set_string_field( L,"build_number", BUILD_NUMBER );
  set_string_field( L,"build_date", __DATE__ );
  set_string_field( L,"build_time", __TIME__ );
#ifndef CAM_DRYOS
  set_string_field( L,"os", "vxworks" );
#else
  set_string_field( L,"os", "dryos" );
#endif
  set_number_field( L, "platformid", PLATFORMID );
  return 1;
}

static int luaCB_get_mode( lua_State* L )
{
  int m = mode_get();
  lua_pushboolean( L, (m&MODE_MASK) != MODE_PLAY );
  lua_pushboolean( L, MODE_IS_VIDEO(m) );
  lua_pushnumber( L, m );
  return 3;
}

// TODO sanity check file ?
static int luaCB_set_raw_develop( lua_State* L )
{
  raw_prepare_develop(luaL_optstring( L, 1, NULL ));
  return 0;
}

static int luaCB_raw_merge_start( lua_State* L )
{
  int op = luaL_checknumber(L,1);
  if (!module_rawop_load())
    return luaL_argerror(L,1,"fail to load raw merge module");

  if ( API_VERSION_MATCH_REQUIREMENT( librawop->version, 1, 0 ) &&
       (op == RAW_OPERATION_SUM || op == RAW_OPERATION_AVERAGE) ) {
    librawop->raw_merge_start(op);
  }
  else {
    return luaL_argerror(L,1,"invalid raw merge op");
  }
  return 0;
}

// TODO sanity check file ?
static int luaCB_raw_merge_add_file( lua_State* L )
{
  if (!module_rawop_load())
    return luaL_argerror(L,1,"fail to load raw merge module");
  librawop->raw_merge_add_file(luaL_checkstring( L, 1 ));
  return 0;
}

static int luaCB_raw_merge_end( lua_State* L )
{
  if (!module_rawop_load())
    return luaL_argerror(L,1,"fail to load raw merge module");
  librawop->raw_merge_end();
  return 0;
}

// Enable/disable LCD back light (input argument 1/0)
static int luaCB_set_backlight( lua_State* L )
{
  int val = (luaL_checknumber(L,1));

  if (val > 0) TurnOnBackLight();
  else TurnOffBackLight();
  return 0;
}

// get the string or number passed in index and return it as an event id
static unsigned levent_id_from_lua_arg( lua_State* L, int index)
{
  unsigned event_id;
  if (lua_type(L, index) == LUA_TSTRING) {
    const char *ev_name = lua_tostring(L, index);
  	event_id = levent_id_for_name(ev_name);
    if (event_id == 0) {
        return luaL_error( L, "bad event name '%s'", ev_name );
    }
  }
  // could check here if it is in the table, but even valid ones can crash
  // so we avoid searching the table if given a number
  else if (lua_type(L,index) == LUA_TNUMBER){
  	event_id = lua_tonumber(L,index);
  }
  else {
    return luaL_error( L, "expected event name or id" );
  }
  return event_id;
}

/*
  get a value where boolean or 0/!0 are accepted for on/off.
  normal lua toboolean will convert 0 to true, but ubasic and c users 
  will expect 0 to be off
  intentional HACK: numbers greater than 1 are returned as is
*/
static unsigned on_off_value_from_lua_arg( lua_State* L, int index)
{
  if( lua_isboolean(L,index) ) {
  	return lua_toboolean(L,index);
  }
  else {
  	return luaL_checknumber(L,index); 
  }
}

/*
  return the index of an event, given it's name or event id
*/
static unsigned levent_index_from_id_lua_arg( lua_State* L, int index )
{
  if (lua_type(L, index) == LUA_TSTRING) {
  	return levent_index_for_name(lua_tostring(L, index));
  }
  else if (lua_type(L,index) == LUA_TNUMBER){
  	return levent_index_for_id(lua_tonumber(L,index));
  }
  else {
    return luaL_error( L, "expected string or number" );
  }
}

/*
  name,id,param = get_levent_def(event)
  event is an event id (number) or name (string)
  returns nil if event is not found
*/
static int luaCB_get_levent_def( lua_State* L )
{
  unsigned event_index = levent_index_from_id_lua_arg(L,1);
  if (event_index == LEVENT_INVALID_INDEX) {
    lua_pushnil(L);
    return 1;
  }
  lua_pushstring(L, levent_table[event_index].name);
  lua_pushnumber(L, levent_table[event_index].id);
  lua_pushnumber(L, levent_table[event_index].param);
  return 3;
}

/*
  index=get_levent_index(event)
  event is an event id (number) or name (string)
  returns index or nil if not found
*/
static int luaCB_get_levent_index( lua_State* L )
{
  unsigned event_index = levent_index_from_id_lua_arg(L,1);
  if (event_index == LEVENT_INVALID_INDEX) {
    lua_pushnil(L);
  }
  else {
    lua_pushnumber(L, event_index);
  }
  return 1;
}

/*
  name,id,param = get_levent_def_by_index(event_index)
  event_index is number index into the event table
  returns nil if event is not found
*/
static int luaCB_get_levent_def_by_index( lua_State* L )
{
  unsigned i = luaL_checknumber(L,1);
  if(i >= levent_count()) {
  	lua_pushnil(L);
    return 1;
  }
  lua_pushstring(L, levent_table[i].name);
  lua_pushnumber(L, levent_table[i].id);
  lua_pushnumber(L, levent_table[i].param);
  return 3;
}

/*
  post_levent_*(event[,unk])
  post the event with PostLogicalEventToUI or PostLogicaEventForNotPowerType
  This sends the event. The difference between functions isn't clear.
  event is an event id (number) or name (string).
  unk is an optional number whose meaning is unknown, defaults to zero. 
    Based on code, other values would probably be a pointer.
	This is NOT the 3rd item in the event table.
*/
static int luaCB_post_levent_to_ui( lua_State* L )
{
  unsigned event_id,arg;

  event_id = levent_id_from_lua_arg(L,1);
  arg = luaL_optnumber(L, 2, 0);
  PostLogicalEventToUI(event_id,arg);
  return 0;
}

static int luaCB_post_levent_for_npt( lua_State* L )
{
  unsigned event_id,arg;

  event_id = levent_id_from_lua_arg(L,1);
  arg = luaL_optnumber(L, 2, 0);
  PostLogicalEventForNotPowerType(event_id,arg);
  return 0;
}

/*
  set_levent_active(event,state)
  event is an event id (number) or name (string)
  state is a numeric or boolean state. true or non zero numbers turn on zero, false or nil turn off
  exact meaning is unknown, but it has something to do with the delivery of the specified event.
*/
static int luaCB_set_levent_active( lua_State* L )
{
  unsigned event_id;
  unsigned state;

  event_id = levent_id_from_lua_arg(L,1);
  state = on_off_value_from_lua_arg(L,2);
  SetLogicalEventActive(event_id,state);
  return 0;
}

/*
  set_levent_script_mode(state)
  state is numeric or boolean state. true or non zero numbers turn on zero, false or nil turn off
  exact meaning is unknown, but it has something to do with the behavior of events and/or SetLogicalEventActive.
*/
static int luaCB_set_levent_script_mode( lua_State* L )
{
  SetScriptMode(on_off_value_from_lua_arg(L,1));
  return 0;
}

/* 
  result=set_capture_mode_canon(value)
  where value is a valid PROPCASE_SHOOTING_MODE value for the current camera
  result is true if the camera is in rec mode
*/
static int luaCB_set_capture_mode_canon( lua_State* L )
{
  int modenum = luaL_checknumber(L,1);
  // if the value as negative, assume it is a mistakenly sign extended PROPCASE_SHOOTING_MODE value
  if(modenum < 0) 
    modenum &= 0xFFFF;
  lua_pushboolean( L, shooting_set_mode_canon(modenum) );
  return 1;
}

/*
 result=set_capture_mode(modenum)
 where modenum is a valid CHDK modemap value
 result is true if modenum is a valid modemap value, otherwise false
*/
static int luaCB_set_capture_mode( lua_State* L )
{
  int modenum = luaL_checknumber(L,1);
  lua_pushboolean( L, shooting_set_mode_chdk(modenum) );
  return 1;
}

/*
 result=is_capture_mode_valid(modenum)
 where modenum is a valid CHDK modemap value
 result is true if modenum is a valid modemap value, otherwise false
*/
static int luaCB_is_capture_mode_valid( lua_State* L )
{
  int modenum = luaL_checknumber(L,1);
  lua_pushboolean( L, shooting_mode_chdk2canon(modenum) != -1 );
  return 1;
}

/* 
  set_record(state)
  if state is 0 (or false) the camera is set to play mode. If 1 or true, the camera is set to record mode.
  NOTE: this only begins the mode change. Script should wait until get_mode() reflects the change,
  before doing anything that requires the new mode. e.g.
  set_record(true)
  while not get_mode() do
  	sleep(10)
  end
*/
static int luaCB_set_record( lua_State* L )
{
  if(on_off_value_from_lua_arg(L,1)) {
    levent_set_record();
  }
  else {
    levent_set_play();
  }
  return 0;
}

// switch mode (0 = playback, 1 = record)
// only for when USB is connected
static int luaCB_switch_mode_usb( lua_State* L )
{
  int mode = luaL_checknumber(L,1);

  if ( mode != 0 && mode != 1 )
  {
    return 0;
  }

  return switch_mode_usb(mode);
}
 
#ifdef CAM_CHDK_PTP
// PTP Live View functions

// Function used to get viewport, bitmap and palette data via PTP
// Address of this function sent back to client program which then
// calls this with options to determine what to transfer
static int handle_video_transfer(ptp_data *data, int flags, int arg2)
{
    int total_size;             // Calculated total size of data to transfer to client

    // Structure containing the info for the current live view frame
    // This information may change across calls
    struct {
        int vp_xoffset;             // Viewport X offset in pixels (for cameras with variable image size)
        int vp_yoffset;             // Viewpoer Y offset in pixels (for cameras with variable image size)
        int vp_width;               // Actual viewport width in pixels (for cameras with variable image size)
        int vp_height;              // Actual viewport height in pixels (for cameras with variable image size)
        int vp_buffer_start;        // Offset in data transferred where the viewport data starts
        int vp_buffer_size;         // Size of viewport data sent (in bytes)
        int bm_buffer_start;        // Offset in data transferred where the bitmap data starts
        int bm_buffer_size;         // Size of bitmap data sent (in bytes)
        int palette_type;           // Camera palette type 
                                    // (0 = no palette, 1 = 16 x 4 byte AYUV values, 2 = 16 x 4 byte AYUV values with A = 0..3, 3 = 256 x 4 byte AYUV values with A = 0..3)
        int palette_buffer_start;   // Offset in data transferred where the palette data starts
        int palette_buffer_size;    // Size of palette data sent (in bytes)
    } vid_info;

    // Populate the above structure with the current default details
    vid_info.vp_xoffset = vid_get_viewport_xoffset_proper();
    vid_info.vp_yoffset = vid_get_viewport_yoffset_proper();
    vid_info.vp_width = vid_get_viewport_width_proper();
    vid_info.vp_height = vid_get_viewport_height_proper();
    vid_info.vp_buffer_start = 0;
    vid_info.vp_buffer_size = 0;
    vid_info.bm_buffer_start = 0;
    vid_info.bm_buffer_size = 0;
    vid_info.palette_type = vid_get_palette_type();
    vid_info.palette_buffer_start = 0;
    vid_info.palette_buffer_size = 0;

    total_size = sizeof(vid_info);

    // Add viewport details if requested
    if ( flags & 0x1 ) // live buffer
    {
        vid_info.vp_buffer_start = total_size;
        vid_info.vp_buffer_size = (vid_get_viewport_buffer_width_proper()*vid_get_viewport_max_height()*6)/4;
        total_size += vid_info.vp_buffer_size;
    }

    // Add bitmap details if requested
    if ( flags & 0x4 ) // bitmap buffer
    {
        vid_info.bm_buffer_start = total_size;
        vid_info.bm_buffer_size = camera_screen.buffer_width*camera_screen.height;
        total_size += vid_info.bm_buffer_size;
    }

    // Add palette detals if requested
    if ( flags & 0x8 ) // bitmap palette
    {
        vid_info.palette_buffer_start = total_size;
        vid_info.palette_buffer_size = vid_get_palette_size();
        total_size += vid_info.palette_buffer_size;
    }

    // Send header structure (along with total size to be sent)
    data->send_data(data->handle,(char*)&vid_info,sizeof(vid_info),total_size,0,0,0);

    // Send viewport data if requested
    if ( flags & 0x1 )
    {
        data->send_data(data->handle,vid_get_viewport_active_buffer(),vid_info.vp_buffer_size,0,0,0,0);
    }

    // Send bitmap data if requested
    if ( flags & 0x4 )
    {
        data->send_data(data->handle,vid_get_bitmap_active_buffer(),vid_info.bm_buffer_size,0,0,0,0);
    }

    // Send palette data if requested
    if ( flags & 0x8 )
    {
        data->send_data(data->handle,vid_get_bitmap_active_palette(),vid_info.palette_buffer_size,0,0,0,0);
    }

    return 0;
}

// Lua function to return base info for PTP live view, including address of above transfer function
static int luaCB_get_video_details( lua_State* L )
{
    // Structure to popualate with live view details
    // These details are static and only need to be retrieved once
    struct {
        int transfer_function;      // Address of transfer function above
        int vp_max_width;           // Maximum viewport width (in pixels)
        int vp_max_height;          // Maximum viewport height (in pixels)
        int vp_buffer_width;        // Viewport buffer width in case buffer is wider than visible viewport (in pixels)
        int bm_max_width;           // Maximum width of bitmap (in pixels)
        int bm_max_height;          // Maximum height of bitmap (in pixels)
        int bm_buffer_width;        // Bitmap buffer width in case buffer is wider than visible bitmap (in pixels)
        int lcd_aspect_ratio;       // 0 = 4:3, 1 = 16:9
    } details;

    // Populate structure info
    details.transfer_function = (int) handle_video_transfer;
    details.vp_max_width = vid_get_viewport_max_width();
    details.vp_max_height = vid_get_viewport_max_height();
    details.vp_buffer_width = vid_get_viewport_buffer_width_proper();
#if CAM_USES_ASPECT_CORRECTION
    details.bm_max_width = ASPECT_XCORRECTION(camera_screen.width);
#else
    details.bm_max_width = camera_screen.width;
#endif
    details.bm_max_height = camera_screen.height;
    details.bm_buffer_width = camera_screen.buffer_width;
    details.lcd_aspect_ratio = vid_get_aspect_ratio();

    // Send data back to client
    lua_pushlstring( L, (char *) &details, sizeof(details) );

    return 1;
}
#endif

/*
pack the lua args into a buffer to pass to the native code calling functions 
currently only handles strings/numbers
start is the stack index of the first arg
*/
#ifdef OPT_LUA_CALL_NATIVE
static int pack_native_args( lua_State* L, unsigned start, unsigned *argbuf)
{
  unsigned i;
  unsigned end = lua_gettop(L);

  for(i = start; i <= end; i++,argbuf++) {
    if (lua_type(L, i) == LUA_TSTRING) {
        *argbuf=(unsigned)lua_tostring( L, i);
    }
    else if (lua_type(L, i) == LUA_TNUMBER) {
        *argbuf=lua_tonumber( L, i);
    }
    else {
      return 0;
    }
  }
  return 1;
}

/*
Native function call interface. Can be used to call canon eventprocs or arbitrary
pointers.

NOTE: this is preliminary, interface may change in later versions!
All arguments must be strings or numbers.
If the function expects to modify it's arguments via a pointer,
then you must provide a number that is a valid pointer. 

You can use the "AllocateMemory" eventproc to obtain buffers.

If the function tries to write to a string passed from lua, Bad Things may happen.

This is potentially dangerous, functions exist which can destroy the onboard firmware.
*/

/*
result=call_func_ptr(ptr,...)
ptr: address of a valid ARM or Thumb function, which uses the normal C calling convention.
result: R0 value after the call returns
*/
static int luaCB_call_func_ptr( lua_State* L)
{
  unsigned *argbuf=NULL;
  unsigned n_args = lua_gettop(L)-1;
  void *fptr;

  fptr=(void *)luaL_checknumber( L, 1 );

  if (n_args) {
    argbuf=malloc(n_args * 4);
    if(!argbuf) {
      return luaL_error( L, "malloc fail" );
    }
    if(!pack_native_args(L, 2, argbuf)) {
      free(argbuf);
      return luaL_error( L, "expected string or number" );
    }
  }
  
  lua_pushnumber( L, call_func_ptr(fptr, argbuf, n_args) );
  free(argbuf);
  return 1;
}

/* 
Call an event procedure

result=call_event_proc("EventprocName",...)
result is the value returned by ExecuteEventProcedure, which is -1 if the eventproc is not found, 
or the eventproc return value (which could also be -1)
NOTE:
Many eventprocs are not registered by default, but can be loaded by calling another event proc
Some useful ones are
SystemEventInit
	includes AllocateMemory, FreeMemory, sprintf, memcpy, Fut functions, log ...
UI_RegistDebugEventProc
	includes capture mode functions, PTM_ functions and much more 
RegisterProductTestEvent
	includes PT_ functions

Others:
RegisterShootSeqEvent
RegisterNRTableEvent
*/

// grab from lowlevel
extern unsigned _ExecuteEventProcedure(const char *name,...);
static int luaCB_call_event_proc( lua_State* L )
{
  const char *evpname;
  unsigned *argbuf;
  unsigned n_args = lua_gettop(L);

  evpname=luaL_checkstring( L, 1 );

  argbuf=malloc(n_args * 4);
  if (!argbuf) {
    return luaL_error( L, "malloc fail" );
  }

  // event proc name is first arg
  *argbuf = (unsigned)evpname;
  
  if(!pack_native_args(L,2,argbuf+1)) {
    free(argbuf);
    return luaL_error( L, "expected string or number" );
  }
  
  lua_pushnumber( L, call_func_ptr(_ExecuteEventProcedure,argbuf,n_args) );
  free(argbuf);
  return 1;
}

#endif // OPT_LUA_CALL_NATIVE

/*
result = reboot(["filename"])
returns false on failure, does not return on success
see lib/armutil/reboot.c for details
*/
static int luaCB_reboot( lua_State* L )
{
    lua_pushboolean(L, reboot(luaL_optstring( L, 1, NULL )));
    return 1;
}

static int luaCB_get_config_value( lua_State* L ) {
    unsigned int argc = lua_gettop(L);
    unsigned int id, i;
    int ret = 1;
    tConfigVal configVal;
    
    if( argc>=1 ) {
        id = luaL_checknumber(L, 1);
        switch( conf_getValue(id, &configVal) ) {
            case CONF_VALUE:
                lua_pushnumber(L, configVal.numb);
            break;
            case CONF_INT_PTR:
                lua_createtable(L, 0, configVal.numb);
                for( i=0; i<configVal.numb; i++ ) {
                    lua_pushinteger(L, configVal.pInt[i]);
                    lua_rawseti(L, -2, i+1);  //t[i+1]=configVal.pInt[i]
                }
            break;
            case CONF_CHAR_PTR:
                lua_pushstring(L, configVal.str);
            break;
            case CONF_OSD_POS:
                lua_pushnumber(L, configVal.pos.x);
                lua_pushnumber(L, configVal.pos.y); ret++;
            break;
            default:
                if( argc>=2) { //Default
                    ret = argc-1;
                } else {
                    lua_pushnil(L);
                }
            break;
        }
    } else {
        lua_pushnil(L);
    }
    return ret;
}

static int luaCB_set_config_value( lua_State* L ) {
    unsigned int argc = lua_gettop(L);
    unsigned int id, i, j;
    tConfigVal configVal = {0,0,0,0};  //initialize isXXX
    
    if( argc>=2 ) {
        id = luaL_checknumber(L, 1);
        for( i=2; i<=argc; i++) {
            switch( lua_type(L, i) ) {
                case LUA_TNUMBER:
                    if( !configVal.isNumb ) {
                        configVal.numb = luaL_checknumber(L, i);
                        configVal.isNumb++;
                    }
                    switch( configVal.isPos ) {
                        case 0: configVal.pos.x = luaL_checknumber(L, i); configVal.isPos++; break;
                        case 1: configVal.pos.y = luaL_checknumber(L, i); configVal.isPos++; break;
                    }
                break;
                case LUA_TSTRING:
                    if( !configVal.isStr ) {
                        configVal.str = (char*)luaL_checkstring(L, i);
                        configVal.isStr++;
                    }
                break;
                case LUA_TTABLE:
                    if( !configVal.isPInt ) {
                        configVal.numb = lua_objlen(L, i);
                        if( configVal.pInt ) {
                            free(configVal.pInt);
                            configVal.pInt = NULL;
                        }
                        configVal.pInt = malloc(configVal.numb*sizeof(int));
                        if( configVal.pInt ) {
                            for( j=1; j<=configVal.numb; j++) {
                                lua_rawgeti(L, i, j);
                                configVal.pInt[j-1] = lua_tointeger(L, -1);
                                lua_pop(L, 1);
                            }
                        }
                        configVal.isPInt++;
                    }
                break;
            }
        }
        lua_pushboolean(L, conf_setValue(id, configVal));
        if( configVal.pInt ) {
            free(configVal.pInt);
            configVal.pInt = NULL;
        }
    } else lua_pushboolean(L, 0);
    return 1;
}

static int luaCB_set_file_attributes( lua_State* L ) {
    unsigned int argc = lua_gettop(L);
    if( argc>=2 ) {
        lua_pushnumber(L, SetFileAttributes(luaL_checkstring(L, 1), luaL_checknumber(L, 2)));
    }
    return 1;
}

#ifdef CAM_CHDK_PTP
/*
msg = read_usb_msg([timeout])
read a message from the CHDK ptp interface.
Returns the next available message as a string, or nil if no messages are available
If timeout is given and not zero, wait until a message is available or timeout expires
*/
static int luaCB_read_usb_msg( lua_State* L )
{
  int timeout = luaL_optnumber( L, 1, 0 );
  if(timeout) {
    action_push(timeout);
    action_push(AS_SCRIPT_READ_USB_MSG);
    return lua_yield( L, 0 );
  }
  ptp_script_msg *msg = ptp_script_read_msg();
  if(msg) {
    lua_pushlstring(L,msg->data,msg->size);
    return 1;
  }
  lua_pushnil(L);
  return 1;
}

/*
status = write_usb_msg(msg,[timeout])
writes a message to the CHDK ptp interface
msg may be nil, boolean, number, string or table (table has some restrictions, will be converted to string)
returns true if the message was queued successfully, otherwise false
if timeout is set and not zero, wait until message is written or timeout expires
NOTE strings will not include a terminating NULL, must be handled by recipient
*/
static int luaCB_write_usb_msg( lua_State* L )
{
  ptp_script_msg *msg;
  int timeout = luaL_optnumber( L, 2, 0 );
  // TODO would it be better to either ignore this or return nil ?
  // a write_usb_msg(function_which_returns_no_value()) is an error in this case
  // replacing with nil might be more luaish
  if(lua_gettop(L) < 1) {
    return luaL_error(L,"missing argument");
  }
  msg=lua_create_usb_msg(L,1,PTP_CHDK_S_MSGTYPE_USER);
  // for user messages, trying to create a message from an incompatible type throws an error
  if(msg->subtype == PTP_CHDK_TYPE_UNSUPPORTED) {
    free(msg);
    return luaL_error(L,"unsupported type");
  }
  if(!msg) {
    return luaL_error(L,"failed to create message");
  }
  if(timeout) {
    action_push(timeout);
    action_push((int)msg);
    action_push(AS_SCRIPT_WRITE_USB_MSG);
    return lua_yield( L, 0 );
  }
  lua_pushboolean(L,ptp_script_write_msg(msg)); 
  return 1;
}
#endif

/* helper for meminfo to set table field only if valid */
static void set_meminfo_num( lua_State* L,const char *name, int val) {
    if(val != -1) {
        set_number_field( L, name, val );
    }
}
/*
meminfo=get_meminfo([heapname])
get camera memory information
heapname="system" or "exmem" if not given, meminfo is returned for heap used by CHDK for malloc
meminfo is false if the requested heapname isn't valid ("exmem" when exmem is not enabled, or unknown)
otherwise, a table of the form
meminfo = {
    name -- string "system" or "exmem"
    chdk_malloc -- bool, this is the heap used by CHDK for malloc
    chdk_start -- number, load address of CHDK
    chdk_size -- number, size of CHDK image
    -- all the following are numbers, will not be set if not available
    start_address
    end_address
    total_size
    allocated_size
    allocated_peak
    allocated_count
    free_size
    free_block_max_size
    free_block_count
}
NOTES
* under vxworks and cameras without GetMemInfo only the only valid fields
  for the system heap will be those defined by chdk and free_block_max_size
* the meaning of fields may not correspond exactly between exmem and system
*/
static int luaCB_get_meminfo( lua_State* L ) {
    // for memory info, duplicated from lowlevel
    extern const char _start,_end;

#if defined(OPT_EXMEM_MALLOC) && !defined(OPT_EXMEM_TESTING)
    const char *default_heapname="exmem";
#else
    const char *default_heapname="system";
#endif
    const char *heapname = luaL_optstring( L, 1, default_heapname );
    cam_meminfo meminfo;

    if(strcmp(heapname,"system") == 0) {
        GetMemInfo(&meminfo);
    }
#if defined(OPT_EXMEM_MALLOC) && !defined(OPT_EXMEM_TESTING)
    else if(strcmp(heapname,"exmem") == 0) {
        GetExMemInfo(&meminfo);
        meminfo.allocated_count = -1; // not implemented in suba
    }
#endif
    else {
        lua_pushboolean(L,0);
        return 1;
    }
    // adjust start and size, if CHDK is loaded at heap start
    if(meminfo.start_address == (int)(&_start)) {
        meminfo.start_address += MEMISOSIZE;
        meminfo.total_size -= MEMISOSIZE;
    }
    lua_createtable(L, 0, 13); // might not always use 13, but doesn't hurt
    set_string_field( L,"name", heapname );
    lua_pushboolean( L, (strcmp(heapname,default_heapname)==0));
    lua_setfield(L, -2, "chdk_malloc");
    set_number_field( L, "chdk_start", (int)(&_start) );
    set_number_field( L, "chdk_size", MEMISOSIZE );
    set_meminfo_num( L, "start_address", meminfo.start_address );
    set_meminfo_num( L, "end_address", meminfo.end_address);
    set_meminfo_num( L, "total_size", meminfo.total_size);
    set_meminfo_num( L, "allocated_size", meminfo.allocated_size);
    set_meminfo_num( L, "allocated_peak", meminfo.allocated_peak);
    set_meminfo_num( L, "allocated_count", meminfo.allocated_count);
    set_meminfo_num( L, "free_size", meminfo.free_size);
    set_meminfo_num( L, "free_block_max_size", meminfo.free_block_max_size);
    set_meminfo_num( L, "free_block_count", meminfo.free_block_count);
    return 1;
}

/*
set scheduling parameters
old_max_count,old_max_ms=set_yield(max_count,max_ms)
*/
static int luaCB_set_yield( lua_State* L )
{
  lua_pushnumber(L,yield_max_count);
  lua_pushnumber(L,yield_max_ms);
  yield_max_count = luaL_optnumber(L,1,YIELD_MAX_COUNT_DEFAULT);
  yield_max_ms = luaL_optnumber(L,2,YIELD_MAX_MS_DEFAULT);
  return 2;
}

static void register_func( lua_State* L, const char *name, void *func) {
  lua_pushcfunction( L, func );
  lua_setglobal( L, name );
}

#define FUNC( X ) { #X,	luaCB_##X },
static const luaL_Reg chdk_funcs[] = {
    FUNC(shoot)
    FUNC(sleep)
    FUNC(cls)
    FUNC(set_console_layout)
    FUNC(set_console_autoredraw)
    FUNC(console_redraw)
    FUNC(get_av96)
    FUNC(get_bv96)
    FUNC(get_day_seconds)
    FUNC(get_disk_size)
  FUNC(get_dof)
  FUNC(get_far_limit)
    FUNC(get_free_disk_space)
    FUNC(get_focus)
    FUNC(get_hyp_dist)
    FUNC(get_iso_market)
    FUNC(get_iso_mode)
    FUNC(get_iso_real)
    FUNC(get_jpg_count)
    FUNC(get_near_limit)
    FUNC(get_prop)
    FUNC(get_prop_str)
    FUNC(get_raw_count)
    FUNC(get_raw_nr)
    FUNC(get_raw)
    FUNC(get_sv96)
    FUNC(get_tick_count)
    FUNC(get_tv96)
    FUNC(get_user_av_id)
    FUNC(get_user_av96)
    FUNC(get_user_tv_id)
    FUNC(get_user_tv96)
    FUNC(get_vbatt)
    FUNC(get_zoom)
    FUNC(get_exp_count)
    FUNC(get_flash_params_count)
    FUNC(get_parameter_data)

    FUNC(set_av96_direct)
    FUNC(set_av96)
    FUNC(set_focus)
    FUNC(set_iso_mode)
    FUNC(set_iso_real)
    FUNC(set_led)
    FUNC(set_nd_filter)
    FUNC(set_prop)
    FUNC(set_prop_str)
    FUNC(set_raw_nr)
    FUNC(set_raw)
    FUNC(set_sv96)
    FUNC(set_tv96_direct)
    FUNC(set_tv96)
    FUNC(set_user_av_by_id_rel)
    FUNC(set_user_av_by_id)
    FUNC(set_user_av96)
    FUNC(set_user_tv_by_id_rel)
    FUNC(set_user_tv_by_id)
    FUNC(set_user_tv96)
    FUNC(set_zoom_speed)
    FUNC(set_zoom_rel)
    FUNC(set_zoom)

    FUNC(wait_click)
    FUNC(is_pressed)
    FUNC(is_key)
#ifdef CAM_HAS_JOGDIAL
    FUNC(wheel_right)
    FUNC(wheel_left)
#endif
    FUNC(md_get_cell_diff)
    FUNC(md_detect_motion)
    FUNC(autostarted)
    FUNC(get_autostart)
    FUNC(set_autostart)
    FUNC(get_usb_power)
    FUNC(enter_alt)
    FUNC(exit_alt)
    FUNC(shut_down)
    FUNC(print_screen)

    FUNC(get_focus_mode)
    FUNC(get_focus_state)
    FUNC(get_focus_ok)
    FUNC(get_propset)
    FUNC(get_zoom_steps)
    FUNC(get_drive_mode)
    FUNC(get_flash_mode)
    FUNC(get_shooting)
    FUNC(get_flash_ready)
    FUNC(get_IS_mode)
    FUNC(set_ev)
    FUNC(get_ev)
    FUNC(get_orientation_sensor)
    FUNC(get_nd_present)
    FUNC(get_movie_status)
    FUNC(set_movie_status)
 
    FUNC(get_histo_range)
    FUNC(shot_histo_enable)
    FUNC(play_sound)
    FUNC(get_temperature)
    FUNC(peek)
    FUNC(poke)
    FUNC(bitand)
    FUNC(bitor)
    FUNC(bitxor)
    FUNC(bitshl)
    FUNC(bitshri)
    FUNC(bitshru)
    FUNC(bitnot)

    FUNC(get_time)

    FUNC(get_buildinfo)
    FUNC(get_mode)

    FUNC(set_raw_develop)
    // NOTE these functions normally run in the spytask.
    // called from lua they will run from kbd task instead
    FUNC(raw_merge_start)
    FUNC(raw_merge_add_file)
    FUNC(raw_merge_end)
    FUNC(set_backlight)
    FUNC(set_aflock)
#ifdef OPT_CURVES
    FUNC(set_curve_state)
    FUNC(get_curve_state)
    FUNC(set_curve_file)
    FUNC(get_curve_file)
#endif
    // get levent definition by name or id, nil if not found
    FUNC(get_levent_def)
    // get levent definition by index, nil if out of range
    FUNC(get_levent_def_by_index)
    // get levent index from name or ID
    FUNC(get_levent_index)
    FUNC(post_levent_to_ui)
    FUNC(post_levent_for_npt)
    FUNC(set_levent_active)
    FUNC(set_levent_script_mode)

    FUNC(set_capture_mode)
    FUNC(set_capture_mode_canon)
    FUNC(is_capture_mode_valid)

    FUNC(set_record)

    FUNC(switch_mode_usb)
#ifdef CAM_CHDK_PTP
   FUNC(get_video_details)
#endif

#ifdef OPT_LUA_CALL_NATIVE
    FUNC(call_event_proc)
    FUNC(call_func_ptr)
#endif
    FUNC(reboot)
    FUNC(get_config_value)
    FUNC(set_config_value)
    FUNC(set_file_attributes)
    FUNC(get_meminfo)
    FUNC(file_browser)
    FUNC(textbox)
    FUNC(draw_pixel)
    FUNC(draw_line)
    FUNC(draw_rect)
    FUNC(draw_rect_filled)
    FUNC(draw_ellipse)
    FUNC(draw_ellipse_filled)
    FUNC(draw_clear)
    FUNC(draw_string)
    FUNC(set_yield)
#ifdef CAM_CHDK_PTP
    FUNC(read_usb_msg)
    FUNC(write_usb_msg)
#endif
    {NULL, NULL},
};

void register_lua_funcs( lua_State* L )
{
  const luaL_reg *r;
  lua_pushlightuserdata( L, action_push_click );
  lua_pushcclosure( L, luaCB_keyfunc, 1 );
  lua_setglobal( L, "click" );

  lua_pushlightuserdata( L, action_push_press );
  lua_pushcclosure( L, luaCB_keyfunc, 1 );
  lua_setglobal( L, "press" );

  lua_pushlightuserdata( L, action_push_release );
  lua_pushcclosure( L, luaCB_keyfunc, 1 );
  lua_setglobal( L, "release" );

  for(r=chdk_funcs;r->name;r++) {
    lua_pushcfunction( L, r->func );
    lua_setglobal( L, r->name );
  }
#ifdef CAM_CHDK_PTP
   luaL_dostring(L,"function usb_msg_table_to_string(t)"
                    " local v2s=function(v)"
                        " local t=type(v)"
                        " if t=='string' then return v end"
                        " if t=='number' or t=='boolean' or t=='nil' then return tostring(v) end"
                        " return '' end"
                    " local r=''"
                    " for k,v in pairs(t) do"
                        " local s,vs=''"
                        " if type(v)=='table' then"
                            " for i=1,table.maxn(v) do"
                            " s=s..'\\t'..v2s(v[i]) end"
                        " else"
                            " vs=v2s(v)"
                            " if #vs then s=s..'\\t'..vs end"
                        " end"
                        " vs=v2s(k)"
                        " if #vs>0 and #s>0 then r=r..vs..s..'\\n' end"
                    " end"
                    " return r"
                   " end");

#endif
}

#ifndef __main_h_
#define __main_h_
#include <mqx.h>
#include <bsp.h>


#include <shell.h>
#define APPLICATION_HAS_SHELL

#include <rtcs.h>
#ifdef APPLICATION_HAS_SHELL
#include <sh_rtcs.h>
#endif
#include <ipcfg.h>
#include <httpd.h>//task2
#include <httpd_types.h>

#define MAIN_TASK   1


#define ENET_DEVICE 0
#define RTCS_DHCP 0

#define ENET_IPADDR IPADDR(192,168,105,192)
#define ENET_IPMASK IPADDR(255,255,255,0)
#define ENET_GATEWAY IPADDR(192,168,105,250)
#define RTCS_PPP 0

extern void Main_task (uint_32);


void rtcs_init(void);
_mqx_int new_callback(HTTPD_SESSION_STRUCT*);
_mqx_int settime_callback(HTTPD_SESSION_STRUCT*);
_mqx_int setzone_callback(HTTPD_SESSION_STRUCT*);
void toggle_web_room_enabled(int);
void button_push_1 (void*);
void button_push_2 (void*);
void button_push_3 (void*);
void button_push_4 (void*);
void hush_button_push (void*);
void hush();
void enable_button_push (void*);
void check_individual_led_enables();
void toggle_enable();
int room_enabled(int);
void manage_leds ();
void alarm_status_1(HTTPD_SESSION_STRUCT*);
void alarm_status_2(HTTPD_SESSION_STRUCT*);
void alarm_status_3(HTTPD_SESSION_STRUCT*);
void alarm_status_4(HTTPD_SESSION_STRUCT*);
void global_enabled_status(HTTPD_SESSION_STRUCT*);
void current_time_status(HTTPD_SESSION_STRUCT*);
void print_time_to_httpd(HTTPD_SESSION_STRUCT*, int);
void enable_time_zone_1_status(HTTPD_SESSION_STRUCT*);
void enable_time_zone_2_status(HTTPD_SESSION_STRUCT*);
void enable_time_zone_3_status(HTTPD_SESSION_STRUCT*);
void enable_time_zone_4_status(HTTPD_SESSION_STRUCT*);
void disable_time_zone_1_status(HTTPD_SESSION_STRUCT*);
void disable_time_zone_2_status(HTTPD_SESSION_STRUCT*);
void disable_time_zone_3_status(HTTPD_SESSION_STRUCT*);
void disable_time_zone_4_status(HTTPD_SESSION_STRUCT*);
int schedule_enabled(int);

/* PPP device must be set manually and 
** must be different from the default IO channel (BSP_DEFAULT_IO_CHANNEL) 
*/
#define PPP_DEVICE      "ittyb:" 

/*
** Define PPP_DEVICE_DUN only when using PPP to communicate
** to Win9x Dial-Up Networking over a null-modem
** This is ignored if PPP_DEVICE is not #define'd
*/
#define PPP_DEVICE_DUN  1

/*
** Define the local and remote IP addresses for the PPP link
** These are ignored if PPP_DEVICE is not #define'd
*/
#define PPP_LOCADDR     IPADDR(192,168,0,216)
#define PPP_PEERADDR    IPADDR(192,168,0,217)

/*
** Define a default gateway
*/
#define GATE_ADDR       IPADDR(192,168,0,1)

#endif /* __main_h_ */


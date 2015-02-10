/**HEADER*******************************************************************
 * * 
 * * Copyright (c) 2008 Freescale Semiconductor;
 * * All Rights Reserved
 * *
 * * Copyright (c) 1989-2008 ARC International;
 * * All Rights Reserved
 * *
 * **************************************************************************** 
 * *
 * * THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
 * * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
 * * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
 * * IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
 * * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 * * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
 * * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
 * * THE POSSIBILITY OF SUCH DAMAGE.
 * *
 * ****************************************************************************
 * *
 * * Comments:
 * *
 * *   This file contains main initialization for your application
 * *   and infinite loop
 * *
 * *END************************************************************************/

#include "main.h"
#include <tfs.h>//task2


#if defined(APPLICATION_HAS_SHELL) && (!SHELLCFG_USES_RTCS)
#error This application requires SHELLCFG_USES_RTCS defined non-zero in user_config.h. Please recompile libraries with this option if any Ethernet interface is available.
#endif

TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
/*  Task number, Entry point, Stack, Pri, String, Auto? */
   {MAIN_TASK,   Main_task,   2000,  9,   "main", MQX_AUTO_START_TASK},
   //{LED_FLASH_TASK,   Led_task,   2000,  8,   "led_flash", MQX_AUTO_START_TASK},
   {0,           0,           0,     0,   0,      0,                 }
};

/*TASK*-----------------------------------------------------------------
 * *
 * * Function Name  : Main_task
 * * Comments       :
 * *    This task initializes MFS and starts SHELL.
 * *
 * *END------------------------------------------------------------------*/
HMI_CLIENT_STRUCT_PTR hmi_client;
unsigned char http_refresh_text[] = 
"<Head>"
"<Title>Alarm Status</Title>"
"</Head>"
"<Body>"
"<center>"
"<font size=20>"
"<a href='new.cgi?1?'>Alarm 1 is <% alarm_status_1 %></a>"
"<br>"
"<a href=""new.cgi?2?"">Alarm 2 is <% alarm_status_2 %></a>"
"<br>"
"<a href=""new.cgi?3?"">Alarm 3 is <% alarm_status_3 %></a>"
"<br>"
"<a href=""new.cgi?4?"">Alarm 4 is <% alarm_status_4 %></a>"
"<br>"
"The system is <a href=""new.cgi?5?""><% global_e_status %></a>"
"<br>"
"<a href=""new.cgi?6?"">Hush all alarms</a>"
"<br>"
"<br>"
"The alarm system clock is currently set to <% current_time_status %>"
"<br>"
"<form action=""new.cgi?1?"" method=""post""><INPUT TYPE=""submit"" VALUE=""Alarm 1 is <% alarm_status_1 %>""></FORM>"
"</center>"
"</Body>";

unsigned char http_refresh_text2[] = 
"<Head><Title>Alarm Status</Title></Head>"
"<Body>Alarm 1 is  <a href=""new.cgi?1?"">toggle1</a> "
"<FORM METHOD=""LINK"" ACTION=""http://192.168.105.192/new.cgi?1""><INPUT TYPE=""submit"" VALUE=""<% alarm_status_1 %>""></FORM>"
"<br>Alarm 2 is <FORM METHOD=""LINK"" ACTION=""http://192.168.105.192/new.cgi?2""><INPUT TYPE=""submit"" VALUE=""<% alarm_status_2 %>""></FORM></Body>";

char page[] = "<!DOCTYPE HTML><html lang=""en-US""><head><meta charset=""UTF-8""><meta http-equiv=""refresh"" content=""1;url=index.html""><script type=""text/javascript"">window.location.href = ""index.html/""</script><title>Page Redirection</title></head><body>If you are not redirected automatically, follow the <a href='index.html'>link to example</a></body></html>"; //redirects you to index.html
const TFS_DIR_ENTRY static_data[] = {	{"/index.html", 0, http_refresh_text, sizeof(http_refresh_text)}, {0,0,0,0}};
static HTTPD_CGI_LINK_STRUCT http_cgi_params[] = {{"new", new_callback}, {0,0}};

const HTTPD_FN_LINK_STRUCT fn_lnk_tbl[] = {
	{"alarm_status_1", alarm_status_1},
	{"alarm_status_2", alarm_status_2},
	{"alarm_status_3", alarm_status_3},
	{"alarm_status_4", alarm_status_4},
	{"global_e_status", global_enabled_status},
	{0, 0}
};

short alarm1 = 0; //1 if the alarm has been triggered
short alarm2 = 0;
short alarm3 = 0;
short alarm4 = 0;
short room_enabled_1 = 1; //1 if an individual room's alarm is enabled
short room_enabled_2 = 1;
short room_enabled_3 = 1;
short room_enabled_4 = 1;
short global_enabled = 0; //1 if the alarm system is enabled
int flash_counter = 0; //flash the leds when this reaches a threshold

void Main_task(uint_32 initial_data)
{
	static HTTPD_ROOT_DIR_STRUCT http_root_dir[] = {{"","tfs:"},{0,0}};
	HTTPD_STRUCT* http_server;

	hmi_client = _bsp_btnled_init();
	rtcs_init();
	_rtc_init(RTC_INIT_FLAG_ENABLE);
	_io_tfs_install("tfs:", static_data);
	http_server = httpd_server_init_af(http_root_dir,"\\index.html",AF_INET);
	HTTPD_SET_PARAM_CGI_TBL(http_server, http_cgi_params);
	HTTPD_SET_PARAM_FN_TBL(http_server, (HTTPD_FN_LINK_STRUCT*) fn_lnk_tbl);
	httpd_server_run(http_server);
	
	btnled_set_value(hmi_client, HMI_LED_1, HMI_VALUE_OFF);
	btnled_set_value(hmi_client, HMI_LED_2, HMI_VALUE_OFF);
	btnled_set_value(hmi_client, HMI_LED_3, HMI_VALUE_OFF);
	btnled_set_value(hmi_client, HMI_LED_4, HMI_VALUE_OFF);
	btnled_add_clb(hmi_client, HMI_BUTTON_1, HMI_VALUE_PUSH, button_push_1, NULL);
	btnled_add_clb(hmi_client, HMI_BUTTON_2, HMI_VALUE_PUSH, button_push_2, NULL);
	btnled_add_clb(hmi_client, HMI_BUTTON_3, HMI_VALUE_PUSH, button_push_3, NULL);
	btnled_add_clb(hmi_client, HMI_BUTTON_4, HMI_VALUE_PUSH, button_push_4, NULL);
	btnled_add_clb(hmi_client, HMI_BUTTON_5, HMI_VALUE_PUSH, hush_button_push, NULL); //hush button
	btnled_add_clb(hmi_client, HMI_BUTTON_6, HMI_VALUE_PUSH, enable_button_push, NULL); //enable button
	while(1) 
	{
		btnled_poll (hmi_client);
		ipcfg_task_poll();
		manage_leds ();
	}
}

_mqx_int new_callback(HTTPD_SESSION_STRUCT *session)
{
	int option = atoi(session->request.urldata);
	if (option == 5){
		toggle_enable();
	}
	if (option == 6){
		hush();
	}
	if ((option>=1) && (option<=4))
	{
		toggle_room_enabled(option);
	}
	httpd_sendstr(session->sock, page);
	return session->request.content_len;	
}

void toggle_room_enabled(room)
{
	switch ( room ) {
		case 1:
			  if (room_enabled_1) { //If room is enabled then disable it and turn the corresponding led off and turn of the alarm.
					room_enabled_1=0;
					btnled_set_value(hmi_client, HMI_LED_1, HMI_VALUE_OFF);
					alarm1=0;	
				} 
				else {
					room_enabled_1=1; //If room is disabled then enable it and if the the alarmsystem is enabled turn the corresponding led on and turn of the alarm.
					if (global_enabled) {
						btnled_set_value(hmi_client, HMI_LED_1, HMI_VALUE_ON);	
					}
					alarm1=0;
				}
			  break;
		case 2:
			  if (room_enabled_2) { //If room is enabled then disable it and turn the corresponding led off and turn of the alarm.
					room_enabled_2=0;
					btnled_set_value(hmi_client, HMI_LED_2, HMI_VALUE_OFF);
					alarm2=0;	
				} 
				else {
					room_enabled_1=1; //If room is disabled then enable it and if the the alarmsystem is enabled turn the corresponding led on and turn of the alarm.
					if (global_enabled) {
						btnled_set_value(hmi_client, HMI_LED_2, HMI_VALUE_ON);	
					}
					alarm2=0;
				}
			  break;
		case 3:
			  if (room_enabled_3) { //If room is enabled then disable it and turn the corresponding led off and turn of the alarm.
					room_enabled_3=0;
					btnled_set_value(hmi_client, HMI_LED_3, HMI_VALUE_OFF);
					alarm3=0;	
				} 
				else {
					room_enabled_3=1; //If room is disabled then enable it and if the the alarmsystem is enabled turn the corresponding led on and turn of the alarm.
					if (global_enabled) {
						btnled_set_value(hmi_client, HMI_LED_3, HMI_VALUE_ON);	
					}
					alarm3=0;
				}
			  break;
		case 4:
			  if (room_enabled_4) { //If room is enabled then disable it and turn the corresponding led off and turn of the alarm.
					room_enabled_4=0;
					btnled_set_value(hmi_client, HMI_LED_4, HMI_VALUE_OFF);
					alarm4=0;	
				} 
				else {
					room_enabled_4=1; //If room is disabled then enable it and if the the alarmsystem is enabled turn the corresponding led on and turn of the alarm.
					if (global_enabled) {
						btnled_set_value(hmi_client, HMI_LED_4, HMI_VALUE_ON);	
					}
					alarm4=0;
				}
			  break;

		default:
		  
				break;
	}
}

void button_push_1 (void *ptr)
{
	if (room_enabled_1 && global_enabled) {
		alarm1=1;
		btnled_toogle(hmi_client, HMI_GET_LED_ID(1));
	}
}
void button_push_2 (void *ptr)
{
	if (room_enabled_2 && global_enabled) {
		alarm2=1;
		btnled_toogle(hmi_client, HMI_GET_LED_ID(2));
	}

}

void button_push_3 (void *ptr)
{
	if (room_enabled_3 && global_enabled) {
		alarm3=1;
		btnled_toogle(hmi_client, HMI_GET_LED_ID(3));
	}
}

void button_push_4 (void *ptr)
{
	if (room_enabled_4 && global_enabled) {
		alarm4=1;
		btnled_toogle(hmi_client, HMI_GET_LED_ID(4));
	}
}

void hush_button_push (void *ptr)
{
	hush();
	
}
void hush (){
	alarm1=0;
	alarm2=0;
	alarm3=0;
	alarm4=0;
	if(global_enabled){
		check_individual_led_enables();
	}
}

void enable_button_push (void *ptr)
{
	toggle_enable();
}

void check_individual_led_enables() //turns on each led if it is individually enabled, ignoring the global enable
{
		if (room_enabled_1) {
			btnled_set_value(hmi_client, HMI_LED_1, HMI_VALUE_ON);
		}
		if (room_enabled_2) {
			btnled_set_value(hmi_client, HMI_LED_2, HMI_VALUE_ON);
		}
		if (room_enabled_3) {
			btnled_set_value(hmi_client, HMI_LED_3, HMI_VALUE_ON);
		}
		if (room_enabled_4) {
			btnled_set_value(hmi_client, HMI_LED_4, HMI_VALUE_ON);
		}
}

void toggle_enable()
{
	if(global_enabled == 1){
		global_enabled = 0;
		alarm1=0;
		alarm2=0;
		alarm3=0;
		alarm4=0;
		btnled_set_value(hmi_client, HMI_LED_1, HMI_VALUE_OFF);
		btnled_set_value(hmi_client, HMI_LED_2, HMI_VALUE_OFF);
		btnled_set_value(hmi_client, HMI_LED_3, HMI_VALUE_OFF);
		btnled_set_value(hmi_client, HMI_LED_4, HMI_VALUE_OFF);
	}
	else{
		global_enabled = 1;
		check_individual_led_enables();
	}
}

void manage_leds (){
	if (flash_counter > 80) {
		flash_counter = 0;
		if (alarm1) {
			btnled_toogle(hmi_client, HMI_GET_LED_ID(1));
		}
		if (alarm2) {
			btnled_toogle(hmi_client, HMI_GET_LED_ID(2));
		}
		if (alarm3) {
			btnled_toogle(hmi_client, HMI_GET_LED_ID(3));
		}
		if (alarm4) {
			btnled_toogle(hmi_client, HMI_GET_LED_ID(4));
		}
	}

	flash_counter++;
}

static void alarm_status_1(HTTPD_SESSION_STRUCT *session)
{
	if (room_enabled_1==1){
		if (alarm1 == 1)
			httpd_sendstr(session->sock, "triggered");
		else
			httpd_sendstr(session->sock, "enabled");
	}
	else
		httpd_sendstr(session->sock, "disabled");
}

static void alarm_status_2(HTTPD_SESSION_STRUCT *session)
{
	if (room_enabled_2==1){
		if (alarm2 == 1)
			httpd_sendstr(session->sock, "triggered");
		else
			httpd_sendstr(session->sock, "enabled");
	}
	else
		httpd_sendstr(session->sock, "disabled");
}
static void alarm_status_3(HTTPD_SESSION_STRUCT *session)
{
	if (room_enabled_3==1){
		if (alarm3 == 1)
			httpd_sendstr(session->sock, "triggered");
		else
			httpd_sendstr(session->sock, "enabled");
	}
	else
		httpd_sendstr(session->sock, "disabled");
}
static void alarm_status_4(HTTPD_SESSION_STRUCT *session)
{
	if (room_enabled_4==1){
		if (alarm4 == 1)
			httpd_sendstr(session->sock, "triggered");
		else
			httpd_sendstr(session->sock, "enabled");
	}
	else
		httpd_sendstr(session->sock, "disabled");
}
static void global_enabled_status(HTTPD_SESSION_STRUCT *session)
{
	if (global_enabled)
		httpd_sendstr(session->sock, "enabled");
	else
		httpd_sendstr(session->sock, "disabled");
}
/* EOF */


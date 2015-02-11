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
"<html>"
"<Head>"
"<Title>Alarm Status</Title>"
"</Head>"
"<Body>"
"<center>"

"<table border=1>"
"<tr><td>"
"<a href='new.cgi?1?'>Alarm 1 is <% alarm_status_1 %></a>"
"</td></tr><tr><td>"
"<a href='new.cgi?2?'>Alarm 2 is <% alarm_status_2 %></a>"
"</td></tr><tr><td>"
"<a href='new.cgi?3?'>Alarm 3 is <% alarm_status_3 %></a>"
"</td></tr><tr><td>"
"<a href='new.cgi?4?'>Alarm 4 is <% alarm_status_4 %></a>"
"</td></tr>"
"</table>"

"<br><br>"
"<a href='new.cgi?5?'>The system is <% global_e_status %></a><br>"
"<a href='new.cgi?6?'>Hush all alarms</a>"


"<br><br>"

"Schedule<br>"
"<table border=1>"
"<tr><td>zone</td><td>time</td><td>action</td></tr>"
"<tr><td>1</td><td><% enable_t_zone_1 %></td><td>enable</td></tr>"
"<tr><td>2</td><td><% enable_t_zone_2 %></td><td>enable</td></tr>"
"<tr><td>3</td><td><% enable_t_zone_3 %></td><td>enable</td></tr>"
"<tr><td>4</td><td><% enable_t_zone_4 %></td><td>enable</td></tr>"
"<tr><td>1</td><td><% d_time_1 %></td><td>disable</td></tr>"
"<tr><td>2</td><td><% d_time_2 %></td><td>disable</td></tr>"
"<tr><td>3</td><td><% d_time_3 %></td><td>disable</td></tr>"
"<tr><td>4</td><td><% d_time_4 %></td><td>disable</td></tr>"
"</table>"

"<br><br><br>"
"<table>"
"<tr>"
"<td>"
"<form action='setzone.cgi'>"
"<input type='radio' name='enable' value='1'>Enable<br>"
"<input type='radio' name='enable' value='0'>Disable"
"</td>"

"<td> the alarm in </td>"

"<td>"
"<input type='radio' name='zone' value='1'>Zone 1<br>"
"<input type='radio' name='zone' value='2'>Zone 2<br>"
"<input type='radio' name='zone' value='3'>Zone 3<br>"
"<input type='radio' name='zone' value='4'>Zone 4"
"</td>"

"<td> at time </td>"

"<td>"
"<table><tr><td><center>hh:mm:ss</center></td></tr>"
"<tr><td>"
"<input type='text' name='hh' pattern='[0-9]{2}' maxlength='2' style='width: 2em;' value='00'>"
"<input type='text' name='mm' pattern='[0-9]{2}' maxlength='2' style='width: 2em;' value='00'>"
"<input type='text' name='ss' pattern='[0-9]{2}' maxlength='2' style='width: 2em;' value='00'>"
"</td></tr>"
"</table>"
"</td>"
		
"<td><input type='submit' value='Submit'></td>"
"</tr>"
"</table>"
"</form>"

"<br>"
//Start of new code
"<form action='settime.cgi'>"
"<table>"
"<tr>"

"<td> Set time to </td>"

"<td>"
"<table><tr><td><center>hh:mm:ss</center></td></tr>"
"<tr><td>"
"<input type='text' name='hh' pattern='[0-9]{2}' maxlength='2' style='width: 2em;' value='00'>"
"<input type='text' name='mm' pattern='[0-9]{2}' maxlength='2' style='width: 2em;' value='00'>"
"<input type='text' name='ss' pattern='[0-9]{2}' maxlength='2' style='width: 2em;' value='00'>"
"</td></tr>"
"</table>"
"</td>"
		
"<td><input type='submit' value='Submit'></td>"
"</tr>"
"</table>"
"</form>"
//End of new code
"<br>"
"Set the system time will return the url 'settime.cgi?hh=00&mm=00&ss=00?'.<br>"
"Setting the enable/disable time for 'setzone.cgi?enable==0&zone===1&hh==00&mm==00&ss==00'.<br>"
"<br>"
"The alarm system clock is currently set to <% time %> <br>"
"<br>"
"<br>"
"<form action='new.cgi?1?' method='post'><INPUT TYPE='submit' VALUE='Alarm 1 is <% alarm_status_1 %>'></FORM>"


"</center>"
"</Body>"
"</html>";

char page[] = "<!DOCTYPE HTML><html lang=""en-US""><head><meta charset=""UTF-8""><meta http-equiv=""refresh"" content=""1;url=index.html""><script type=""text/javascript"">window.location.href = ""index.html/""</script><title>Page Redirection</title></head><body>If you are not redirected automatically, follow the <a href='index.html'>link to example</a></body></html>"; //redirects you to index.html
const TFS_DIR_ENTRY static_data[] = {	{"/index.html", 0, http_refresh_text, sizeof(http_refresh_text)}, {0,0,0,0}};
static HTTPD_CGI_LINK_STRUCT http_cgi_params[] = {{"new", new_callback},{"settime", settime_callback},{"setzone", setzone_callback},{0,0}};

const HTTPD_FN_LINK_STRUCT fn_lnk_tbl[] = {
	{"alarm_status_1", alarm_status_1},
	{"alarm_status_2", alarm_status_2},
	{"alarm_status_3", alarm_status_3},
	{"alarm_status_4", alarm_status_4},
	{"global_e_status", global_enabled_status},
	{"time", current_time_status},
	{"enable_t_zone_1", enable_time_zone_1_status},
	{"enable_t_zone_2", enable_time_zone_2_status},
	{"enable_t_zone_3", enable_time_zone_3_status},
	{"enable_t_zone_4", enable_time_zone_4_status},
	{"d_time_1", disable_time_zone_1_status},
	{"d_time_2", disable_time_zone_2_status},
	{"d_time_3", disable_time_zone_3_status},
	{"d_time_4", disable_time_zone_4_status},
	{0, 0}
};

short alarm[] = {0,0,0,0};//1 if the alarm has been triggered
short room_enabled[] = {1,1,1,1};//1 if an individual room's alarm is enabled
short global_enabled = 0; //1 if the alarm system is enabled
int flash_counter = 0; //flash the leds when this reaches a threshold
int enable_time_zone_seconds[4] = {0,0,0,0};
int disable_time_zone_seconds[4] = {0,0,0,0};
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

_mqx_int settime_callback(HTTPD_SESSION_STRUCT *session)
{
	int hours, minutes, seconds;
	RTC_TIME_STRUCT the_new_time;
	sscanf(session->request.urldata, "%u:%u:%u", &hours, &minutes, &seconds);//TODO
	the_new_time.seconds=seconds+60*minutes+3600*hours;
	
	//seconds = atoi(session->request.urldata);//Just for test
	//the_new_time.seconds=seconds;//Just for test, only uses seconds right now

	_rtc_set_time(&the_new_time); //set the new time

	httpd_sendstr(session->sock, page);
	return session->request.content_len;	
}

_mqx_int setzone_callback(HTTPD_SESSION_STRUCT *session)
{
	//char[] inputdata;
	//char[] temp= {'0', '0'};
	int enable, zone, hours, minutes, seconds;
	//int hours, minutes, seconds;
	//sscanf(session->request.urldata, "%u:%u:%u", &hours, &minutes, &seconds);//TODO
	//enable_time_zone_1_seconds=seconds+60*minutes+3600*hours;
	//TODO if ZONE == 1 then...
	//TODO if ZONE == 2 then...
	/*inputdata = session->request.urldata;

	temp[1]=inputdata[8];
	enable=atoi(temp);
	temp[1]=inputdata[16];
	zone=atoi(temp);

	temp[0]=inputdata[21];
	temp[1]=inputdata[22];
	hours=atoi(temp);
	temp[0]=inputdata[27];
	temp[1]=inputdata[28];
	minutes=atoi(temp);
	temp[0]=inputdata[33];
	temp[1]=inputdata[34];
	seconds=atoi(temp);*/
	
	sscanf(session->request.urldata, "enable=%1u&zone=%1u&hh=%2u&mm=%2u&ss=%2u", &enable, &zone, &hours, &minutes, &seconds);

	if (enable==1) {
		enable_time_zone_seconds[zone-1] = seconds + 60*minutes + 60*hours;
	} else {
		disable_time_zone_seconds[zone-1] = seconds + 60*minutes + 60*hours;
	}
	
	httpd_sendstr(session->sock, session->request.urldata);
	return session->request.content_len;	
}

void toggle_room_enabled(room)
{
	switch ( room ) {
		case 1:
			  if (room_enabled[0]) { //If room is enabled then disable it and turn the corresponding led off and turn of the alarm.
					room_enabled[0]=0;
					btnled_set_value(hmi_client, HMI_LED_1, HMI_VALUE_OFF);
					alarm[0]=0;	
				} 
				else {
					room_enabled[0]=1; //If room is disabled then enable it and if the the alarmsystem is enabled turn the corresponding led on and turn of the alarm.
					if (global_enabled) {
						btnled_set_value(hmi_client, HMI_LED_1, HMI_VALUE_ON);	
					}
					alarm[0]=0;
				}
			  break;
		case 2:
			  if (room_enabled[1]) { //If room is enabled then disable it and turn the corresponding led off and turn of the alarm.
					room_enabled[1]=0;
					btnled_set_value(hmi_client, HMI_LED_2, HMI_VALUE_OFF);
					alarm[1]=0;	
				} 
				else {
					room_enabled[0]=1; //If room is disabled then enable it and if the the alarmsystem is enabled turn the corresponding led on and turn of the alarm.
					if (global_enabled) {
						btnled_set_value(hmi_client, HMI_LED_2, HMI_VALUE_ON);	
					}
					alarm[1]=0;
				}
			  break;
		case 3:
			  if (room_enabled[2]) { //If room is enabled then disable it and turn the corresponding led off and turn of the alarm.
					room_enabled[2]=0;
					btnled_set_value(hmi_client, HMI_LED_3, HMI_VALUE_OFF);
					alarm[2]=0;	
				} 
				else {
					room_enabled[2]=1; //If room is disabled then enable it and if the the alarmsystem is enabled turn the corresponding led on and turn of the alarm.
					if (global_enabled) {
						btnled_set_value(hmi_client, HMI_LED_3, HMI_VALUE_ON);	
					}
					alarm[2]=0;
				}
			  break;
		case 4:
			  if (room_enabled[3]) { //If room is enabled then disable it and turn the corresponding led off and turn of the alarm.
					room_enabled[3]=0;
					btnled_set_value(hmi_client, HMI_LED_4, HMI_VALUE_OFF);
					alarm[3]=0;	
				} 
				else {
					room_enabled[3]=1; //If room is disabled then enable it and if the the alarmsystem is enabled turn the corresponding led on and turn of the alarm.
					if (global_enabled) {
						btnled_set_value(hmi_client, HMI_LED_4, HMI_VALUE_ON);	
					}
					alarm[3]=0;
				}
			  break;

		default:
		  
				break;
	}
}

void button_push_1 (void *ptr)
{
	if (room_enabled[0] && global_enabled) {
		alarm[0]=1;
		btnled_toogle(hmi_client, HMI_GET_LED_ID(1));
	}
}
void button_push_2 (void *ptr)
{
	if (room_enabled[1] && global_enabled) {
		alarm[1]=1;
		btnled_toogle(hmi_client, HMI_GET_LED_ID(2));
	}

}

void button_push_3 (void *ptr)
{
	if (room_enabled[2] && global_enabled) {
		alarm[2]=1;
		btnled_toogle(hmi_client, HMI_GET_LED_ID(3));
	}
}

void button_push_4 (void *ptr)
{
	if (room_enabled[3] && global_enabled) {
		alarm[3]=1;
		btnled_toogle(hmi_client, HMI_GET_LED_ID(4));
	}
}

void hush_button_push (void *ptr)
{
	hush();
	
}
void hush (){
	int i;
	for (i=0; (i<4); i++) {alarm[i]=0;};
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
		if (room_enabled[0]) {
			btnled_set_value(hmi_client, HMI_LED_1, HMI_VALUE_ON);
		}
		if (room_enabled[1]) {
			btnled_set_value(hmi_client, HMI_LED_2, HMI_VALUE_ON);
		}
		if (room_enabled[2]) {
			btnled_set_value(hmi_client, HMI_LED_3, HMI_VALUE_ON);
		}
		if (room_enabled[3]) {
			btnled_set_value(hmi_client, HMI_LED_4, HMI_VALUE_ON);
		}
}

void toggle_enable()
{
	int i;
	if(global_enabled == 1){
		global_enabled = 0;
		for (i=0; (i<4); i++) {alarm[i]=0;};
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
		if (alarm[0]) {
			btnled_toogle(hmi_client, HMI_GET_LED_ID(1));
		}
		if (alarm[1]) {
			btnled_toogle(hmi_client, HMI_GET_LED_ID(2));
		}
		if (alarm[2]) {
			btnled_toogle(hmi_client, HMI_GET_LED_ID(3));
		}
		if (alarm[3]) {
			btnled_toogle(hmi_client, HMI_GET_LED_ID(4));
		}
	}

	flash_counter++;
}

static void alarm_status_1(HTTPD_SESSION_STRUCT *session)
{
	if (room_enabled[0]==1){
		if (alarm[0] == 1)
			httpd_sendstr(session->sock, "triggered");
		else
			httpd_sendstr(session->sock, "enabled");
	}
	else
		httpd_sendstr(session->sock, "disabled");
}

static void alarm_status_2(HTTPD_SESSION_STRUCT *session)
{
	if (room_enabled[1]==1){
		if (alarm[1] == 1)
			httpd_sendstr(session->sock, "triggered");
		else
			httpd_sendstr(session->sock, "enabled");
	}
	else
		httpd_sendstr(session->sock, "disabled");
}
static void alarm_status_3(HTTPD_SESSION_STRUCT *session)
{
	if (room_enabled[2]==1){
		if (alarm[2] == 1)
			httpd_sendstr(session->sock, "triggered");
		else
			httpd_sendstr(session->sock, "enabled");
	}
	else
		httpd_sendstr(session->sock, "disabled");
}
static void alarm_status_4(HTTPD_SESSION_STRUCT *session)
{
	if (room_enabled[3]==1){
		if (alarm[3] == 1)
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

static void current_time_status(HTTPD_SESSION_STRUCT *session)
{
   	char time_string[32];
	int hours, minutes, seconds;
	RTC_TIME_STRUCT curr_time;
	_rtc_get_time(&curr_time);
	seconds = curr_time.seconds;
	hours = seconds/3600;
	minutes = (seconds%3600)/60;
	seconds = seconds%60;
	sprintf(time_string, "%u:%u:%u\n", hours, minutes, seconds);
	httpd_sendstr(session->sock, time_string);
}

static void enable_time_zone_1_status(HTTPD_SESSION_STRUCT *session)
{
	print_time_to_httpd(session, enable_time_zone_seconds[0]);
}

static void enable_time_zone_2_status(HTTPD_SESSION_STRUCT *session)
{
	print_time_to_httpd(session, enable_time_zone_seconds[1]);
}

static void enable_time_zone_3_status(HTTPD_SESSION_STRUCT *session)
{
	print_time_to_httpd(session, enable_time_zone_seconds[2]);
}

static void enable_time_zone_4_status(HTTPD_SESSION_STRUCT *session)
{
	print_time_to_httpd(session, enable_time_zone_seconds[3]);
}

static void disable_time_zone_1_status(HTTPD_SESSION_STRUCT *session)
{
	print_time_to_httpd(session, disable_time_zone_seconds[0]);
}

static void disable_time_zone_2_status(HTTPD_SESSION_STRUCT *session)
{
	print_time_to_httpd(session, disable_time_zone_seconds[1]);
}

static void disable_time_zone_3_status(HTTPD_SESSION_STRUCT *session)
{
	print_time_to_httpd(session, disable_time_zone_seconds[2]);
}

static void disable_time_zone_4_status(HTTPD_SESSION_STRUCT *session)
{
	print_time_to_httpd(session, disable_time_zone_seconds[3]);
}

static void print_time_to_httpd(HTTPD_SESSION_STRUCT *session, int seconds)
{
	char time_string[32];
    	int hours, minutes;
	hours = seconds/3600;
	minutes = (seconds%3600)/60;
	seconds = seconds%60;
	sprintf(time_string, "%u:%u:%u\n", hours, minutes, seconds);
	httpd_sendstr(session->sock, time_string);
}
/* EOF */


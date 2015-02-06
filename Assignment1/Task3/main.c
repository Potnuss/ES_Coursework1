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
unsigned char http_refresh_text[] = "<Head><Title>Welcome to our board test page</Title></Head><Body bgcolor=""Red""><Centre>Some text floating in the centre.</Centre><br> <marquee behavior=""slide"" direction=""left"">No cool car</marquee></Body>";
const TFS_DIR_ENTRY static_data[] = {	{"/index.html", 0, http_refresh_text, sizeof(http_refresh_text)}, {0,0,0,0}};
static HTTPD_CGI_LINK_STRUCT http_cgi_params[] = {{"led", led_callback}, {"time", time_callback}, {"set_time", set_time_callback}, {0,0}};
char buffer[32];

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
	httpd_server_run(http_server);
	
	
	btnled_set_value(hmi_client, HMI_LED_1, HMI_VALUE_OFF);
	btnled_set_value(hmi_client, HMI_LED_2, HMI_VALUE_OFF);
	btnled_set_value(hmi_client, HMI_LED_3, HMI_VALUE_OFF);
	btnled_set_value(hmi_client, HMI_LED_4, HMI_VALUE_OFF);
	btnled_add_clb(hmi_client, HMI_BUTTON_1, HMI_VALUE_PUSH, button_push_1, NULL);
	btnled_add_clb(hmi_client, HMI_BUTTON_1, HMI_VALUE_RELEASE, button_release_1, NULL);
	btnled_add_clb(hmi_client, HMI_BUTTON_2, HMI_VALUE_PUSH, button_push_2, NULL);
	btnled_add_clb(hmi_client, HMI_BUTTON_2, HMI_VALUE_RELEASE, button_release_2, NULL);
	btnled_add_clb(hmi_client, HMI_BUTTON_3, HMI_VALUE_PUSH, button_push_3, NULL);
	btnled_add_clb(hmi_client, HMI_BUTTON_3, HMI_VALUE_RELEASE, button_release_3, NULL);
	btnled_add_clb(hmi_client, HMI_BUTTON_4, HMI_VALUE_PUSH, button_push_4, NULL);
	btnled_add_clb(hmi_client, HMI_BUTTON_4, HMI_VALUE_RELEASE, button_release_4, NULL);
	while(1) 
	{
		btnled_poll (hmi_client);
		ipcfg_task_poll();
	}
	//for(;;) {}

}

_mqx_int led_callback(HTTPD_SESSION_STRUCT *session)
{
	int led = atoi(session->request.urldata);
	httpd_sendstr(session->sock, "<html><body>LED toggled</body><html>");
	btnled_toogle(hmi_client, HMI_GET_LED_ID(led));
	return session->request.content_len;
	
}

_mqx_int time_callback(HTTPD_SESSION_STRUCT *session)
{
        int hours, minutes, seconds;
        RTC_TIME_STRUCT curr_time;
	_rtc_get_time(&curr_time);
	seconds = curr_time.seconds;
	hours = seconds/3600;
	minutes = (seconds%3600)/60;
	seconds = seconds%60;
	sprintf(buffer, "%u:%u:%u\n", hours, minutes, seconds);
	httpd_sendstr(session->sock, buffer);
	return session->request.content_len;
}

_mqx_int set_time_callback(HTTPD_SESSION_STRUCT *session)
{
        int hours, minutes, seconds;
	RTC_TIME_STRUCT the_new_time;
	sscanf(session->request.urldata, "%u:%u:%u", &hours, &minutes, &seconds);
	the_new_time.seconds=seconds+60*minutes+3600*hours;
	_rtc_set_time(&the_new_time); //set the new time

	_rtc_get_time(&the_new_time); //print the current time
	seconds = the_new_time.seconds;
	hours = seconds/3600;
	minutes = (seconds%3600)/60;
	seconds = seconds%60;
	sprintf(buffer, "%u:%u:%u\n", hours, minutes, seconds);
	httpd_sendstr(session->sock, buffer);
	return session->request.content_len;
}

void button_push_1 (void *ptr)
{
	btnled_set_value(hmi_client, HMI_LED_1, HMI_VALUE_ON);
}

void button_release_1 (void *ptr)
{
	btnled_set_value(hmi_client, HMI_LED_1, HMI_VALUE_OFF);
}

void button_push_2 (void *ptr)
{
	btnled_set_value(hmi_client, HMI_LED_2, HMI_VALUE_ON);
}

void button_release_2 (void *ptr)
{
	btnled_set_value(hmi_client, HMI_LED_2, HMI_VALUE_OFF);
}

void button_push_3 (void *ptr)
{
	btnled_set_value(hmi_client, HMI_LED_3, HMI_VALUE_ON);
}

void button_release_3 (void *ptr)
{
	btnled_set_value(hmi_client, HMI_LED_3, HMI_VALUE_OFF);
}
void button_push_4 (void *ptr)
{
	btnled_set_value(hmi_client, HMI_LED_4, HMI_VALUE_ON);
}

void button_release_4 (void *ptr)
{
	btnled_set_value(hmi_client, HMI_LED_4, HMI_VALUE_OFF);
}
/* EOF */


#include "utils.h"

struct tm* get_local_time(){
    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    return timeinfo;
}


char* format_timeinfo(struct tm* timeinfo){
	static char formated_time[9];
	snprintf((char *) &formated_time,sizeof(formated_time), "%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	
	return formated_time;
}


char* get_formated_time(){
	return format_timeinfo(get_local_time());
}


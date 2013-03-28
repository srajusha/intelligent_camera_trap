#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sstream>
#include <iostream>
#include <time.h>
#include "utilities.h"
#include <sys/statvfs.h>
#include "flirROSAdaptive.h"
using namespace std;

#define TERMINATE 	 0
#define WAIT_FOR_TRIGGER 1
#define TRACK_OBJECT 	 2
#define EXTRACT_FILES 	 3

typedef int (*State_Variable)(void);


State_Variable current_state;
string testString;
bool vibration_trigger = false;


int Wait_For_Trigger(void);
int Track_Object(void);
int Extract_Files(void);
int Call_State_Function(State_Variable);

long int getDiskSpace();

State_Variable State_Table[4] = { 
	NULL, 
	&Wait_For_Trigger, 
	&Track_Object,
	&Extract_Files
};


void VibrationCallback(const std_msgs::String::ConstPtr& msg){
	ROS_INFO("Vibration callback recieved: [%s]", msg->data.c_str());
	vibration_trigger = true;

	trackObject();
	write(fd, &stop_cmd, 4);
}

int Wait_For_Trigger(){
	ROS_INFO("Waiting for vibration trigger\n");


	while(!vibration_trigger){
		ros::spinOnce();
		//check time stuff here
	}
	return TRACK_OBJECT;
}


int Track_Object(){
	ROS_INFO("Tracking object\n");
	return EXTRACT_FILES;
}

int Extract_Files(void){
	ROS_INFO("checking termination conditions\n");
	
	time_t currentTime;
	struct tm * time_in;
	char time_out[30];

	time( &currentTime );
	time_in = localtime(&currentTime);
	
	strftime(time_out, 30, "%H %M %S %m %d %Y", time_in); 
	
	ROS_INFO("%s", time_out);
	
//	cout << "hour = " << getHour(time_out) << "\n"; 	
//	cout << "min = " << getMin(time_out) << "\n"; 
//	cout << "sec = " << getSec(time_out) << "\n"; 
//	cout << "month = " << getMonth(time_out) << "\n"; 
//	cout << "day = " << getDay(time_out) << "\n"; 
//	cout << "year = " << getYear(time_out) << "\n"; 
	
	bool endProgram = true;
	return (endProgram ? TERMINATE : WAIT_FOR_TRIGGER);
}

long int getDiskSpace(){
	struct statvfs vfs;
	statvfs("/home/", &vfs);
	unsigned long free = vfs.f_bavail;
	unsigned long total = vfs.f_blocks;
	return ((free*100)/total);
}

int Call_State_Function(State_Variable function){
	int new_index = (*function)();
	return new_index;
}

int main(int argc, char **argv){	
	ros::init(argc, argv, "main");
	ros::NodeHandle n;

	ROS_INFO("disk space percent free: %ld", getDiskSpace());
	
	int lookup = 1;
	


	ROS_INFO("attempting to subscribe to Vibration_Node");
	ros::Subscriber sub = n.subscribe("Vibration_Node", 0, VibrationCallback);
			 
	while (ros::ok()){
		ros::spinOnce();
	}
	return 0;
} 
	
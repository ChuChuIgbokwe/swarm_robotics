#include <kilolib.h>
#include <math.h>

#define UID 0
#define DISTANCE_64_32 1
#define DISTANCE_96_32 2
#define DISTANCE_96_64 3
#define X_COORD 5
#define Y_COORD 6

#define HOP_COUNT 8

//KILOBOT UNIQUE ID'S
#define ORIGIN_KILOBOT 32
#define FIRST_KILOBOT 64
#define SECOND_KILOBOT 96
#define THIRD_KILOBOT 128
#define GRADIENT_MAX 255



int new_message = 0;    // Flag to keep track of new messages.
int own_gradient = GRADIENT_MAX;
int id_received = 0;        //used to track the UID of a kilobot sending a message
int distance_primary = 0;  //used to track of how far away a robot
int distance_secondary = 0;
int distance_tertiary = 0
int x_coordinate = 0;
int y_coordinate = 0;


int distance;
int distance_64_32 = 0;     //distance of 64 from 32
int distance_96_32 = 0;     //distance of 96 from 32
int distance_96_64 = 0;  //distance of 96 from 64 

int received_gradient = 0;
float alpha = 0.2;  //0 < alpha << 1


//At the top of the file, declare a "flag" for when a message is sent
int message_sent = 0;


uint32_t last_gradient_anchored;
message_t message;


void setup()
{

    if (kilo_uid == ORIGIN_KILOBOT)
    {
        own_gradient = 0;
    }
    
    // Set the transmission message.
    message.type = NORMAL;
    message.data[HOP_COUNT] = own_gradient;
    message.data[UID] = kilo_uid;
    message.data[DISTANCE] = distance;
    message.data[DISTANCE_64_92] = distance;
    message.crc = message_crc(&message);
}

message_t *message_tx()
{
    return &message;
}

void message_rx(message_t *m, distance_measurement_t *d)
{
    // Only process this message if the previous one has been processed.
    if (new_message == 0)
    {
        new_message = 1;
        distance = estimate_distance(d)
        rdeceived_gradient = m->data[0];
        
        id_received =  m->data[UID];
        distance_primary = m->data[DISTANCE_64_32];
        distance_secondary = m->data[DISTANCE_64_92];
        distance_tertiary = m->data[DISTANCE_96_64];
    }
}

void message_tx_success() {
   message_sent = 1;
}

int main()
{
    kilo_init();
    kilo_message_rx = message_rx;
    kilo_message_tx = message_tx; // Register the message_tx callback function.
    kilo_message_tx_success = message_tx_success; // Register the message_tx_success callback function.
    kilo_start(setup, loop);
    
    return 0;
}



void selective_listening_to_first_kilobot_from_origin() 
{   
	if(message_sent){
	    set_color(RGB(1, 0, 1));// Magenta
	    delay(100);
        set_color(RGB(0, 0, 0));
    	message_sent = 0;
    }

    if (kilo_uid == ORIGIN_KILOBOT)
    {
        if (new_message == 1  && id_received ==  FIRST_KILOBOT)
        {
            //new_message = 0; // Reset the flag 
            //distance_to_light(distance);
            set_color(RGB(1, 0, 0)); // Red
            distance_64_32 = distance;
            x_coordinate = distance; 
            y_coordinate = 0;

            message.data[X_COORD] = x_coordinate;
            message.data[Y_COORD] = y_coordinate;
            message.data[DISTANCE_64_32] = distance_64_32;
            message.crc = message_crc(&message); // Update the transmission message whenever the gradient changes.

        }


void selective_listening_to_second_kilobot_from_origin_and_first_kilobot()
{
	if(message_sent){
	    set_color(RGB(1, 1, 1)); // White
	    delay(100);
        set_color(RGB(0, 0, 0));
    	message_sent = 0;
    }
	if (new_message == 1 && id_received == ORIGIN_KILOBOT)
        {
            //new_message = 0; // Reset the flag
            set_color(RGB(1, 1, 0)); // Yellow
            distance_96_32 = distance;
            message.data[DISTANCE_96_32] = distance_96_32;
        }
    else if (new_message == 1 && id_received == SECOND_KILOBOT)
    {
    	// Update the fields regarding the a message from the x-axis
			distance_96_64 = distance;
			message.data[DISTANCE_96_64] = distance_96_64;
			
			distance_64_32 = m->data[DISTANCE_64_32];
			message.data[D_O_X_INDEX] = distance_64_32;
			//blink_light(3, 0, 3, 1, 75);	// Purple
			set_color(RGB(3, 0, 3));
    }
      

    }
}









if (kilo_uid == FIRST_KILOBOT)
{
	if (id_received == SECOND_KILOBOT)
	{
		distance_96_64 = distance;
		message.data[DISTANCE_96_64] = distance_96_64;
	}
}  


#include <kilolib.h>
#include <math.h>

#define UID 0
#define DISTANCE_64_32 1
#define DISTANCE_96_32 2
#define DISTANCE_96_64 3
#define X_COORD 4
#define Y_COORD 5
#define DELTA_X 6
#define DELTA_Y 7
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
int distance_tertiary = 0;
int x_coordinate = 0;
int y_coordinate = 0;
int delta_x = 0;
int delta_y =0;


int distance;
int distance_64_32 = 0;     //distance of 64 from 32
int distance_96_32 = 0;     //distance of 96 from 32
int distance_96_64 = 0;  //distance of 96 from 64 

int received_gradient = 0;
float alpha = 0.2;  //0 < alpha << 1


//At the top of the file, declare a "flag" for when a message is sent
int message_sent = 0;


uint32_t last_changed = 0;
uint32_t last_gradient_anchored;
message_t message;


void count_the_blinks(int r,int g,int b,int count)
{
    int i;
    //Blink white three times in between counts
    for (i = 0; i < 3; i++)
    {
        set_color(RGB(1, 1, 1)); // White
        delay(100);
    }
    // Blink specific colours
    for(i = 0; i < count; i++)
    {
        set_color(RGB(r, g, b));
        delay(500);
        set_color(RGB(0, 0, 0));
        delay(500);
    }
}


void distance_to_light(int n)
{
    //This function allows you to see the distance between kilobots by counting the number of blinks
    int h,t,u, temp_t;
    
    h = n/100;          // Find out how many hundreds the number has
    temp_t = n%100;     // Get the remainder of division by hundreds
    t = temp_t/10;      // Find out how many tens the number has   
    u = temp_t%10;     // Find out how many units the number has

    //Blink Red for Hundreds, Grren for tens and Blue for Units
    count_the_blinks(1,0,0,h); //Red
    count_the_blinks(0,1,0,t); //Green
    count_the_blinks(0,0,1,u); // Blue
}


void hop_count_colour_change(gradient)
{
    if (gradient == 0)
    {
        set_color(RGB(1, 1, 1)); // White
    }
    else if (gradient == 1)
    {
        set_color(RGB(1, 0, 0)); // Red
    }
    else if (gradient == 2)
    {
        set_color(RGB(0, 1, 0)); // Green
    }
    else if (gradient == 3)
    {
        set_color(RGB(0, 0, 1)); // Blue
    }
    else if (gradient == 4)
    {
        set_color(RGB(1, 0, 1)); // Magenta
    }
    else if (gradient >= 5)
    {
        set_color(RGB(1, 1, 0)); // Yellow
    }
}

void gradient_adaptive()
{
     if (kilo_uid !=  ORIGIN_KILOBOT)
    {
        if (new_message == 1)
        {
            // If a neighbor's gradient is 1 or more less than this robot's
            // gradient, the latter should not increase.
            // Set last_gradient_anchored to kilo_ticks to inhibit activation of
            // the increment statement.
            if (own_gradient >= received_gradient + 1)
            {
                last_gradient_anchored = kilo_ticks;
            }           
     
            // If a neighbor's gradient is 2 or more less than this robot's 
            // gradient, reduce the latter to the neighbor's gradient + 1.
            if (own_gradient > received_gradient + 1)
            {
                own_gradient = received_gradient + 1;
                
                // Update the transmission message whenever the gradient changes.
                message.type = NORMAL;
                message.data[HOP_COUNT] = own_gradient;
                message.crc = message_crc(&message);
            }
            new_message = 0;
        }
        
        // If no neighbor with a gradient of 1 or more less than this robot's
        // gradient is detected within 2 seconds, increment the latter by 1.
        if ((kilo_ticks > (last_gradient_anchored + 64)) && (own_gradient < GRADIENT_MAX))
        {
            own_gradient = own_gradient + 1;
        }
    }
    hop_count_colour_change(own_gradient);
}


void three_way_selective_listening()
{   
    if(message_sent)
    {
        // set_color(RGB(1, 1, 1)); // White
        // delay(100);
        // set_color(RGB(0, 0, 0));
        message_sent = 0;
    }
        
   
    
        //if (new_message == 1 && id_received == ORIGIN_KILOBOT && kilo_uid == FIRST_KILOBOT)
        if (new_message == 1 && kilo_uid == FIRST_KILOBOT)
        {   
            if (id_received == ORIGIN_KILOBOT)
            {
                if (kilo_ticks > last_changed + 64)
                {
                    last_changed = kilo_ticks;
                    new_message = 0;// Reset the flag
                    set_color(RGB(1, 1, 0)); // Yellow
                    delay(100);
                    set_color(RGB(0, 0, 0));
                }
                
                
            }
            if (id_received == SECOND_KILOBOT)
            {
                if (kilo_ticks > last_changed + 64)
                {
                    last_changed = kilo_ticks;
                    new_message = 0;// Reset the flag
                    set_color(RGB(0, 1, 0)); // Green
                    delay(100);
                    set_color(RGB(0, 0, 0));
                }
            }
            
            // distance_96_32 = distance;
            // message.data[DISTANCE_96_32] = distance_96_32;
            // if (kilo_ticks > last_changed + 64)
            // {
            //  new_message = 2;
            // }
        }
        //if (new_message == 1 && id_received == SECOND_KILOBOT && kilo_uid == ORIGIN_KILOBOT)
        if (new_message == 1  && kilo_uid == ORIGIN_KILOBOT)
        {   
            if (id_received == SECOND_KILOBOT)
            {
                if (kilo_ticks > last_changed + 64)
                {
                    last_changed = kilo_ticks;
                    new_message = 0;// Reset the flag
                    set_color(RGB(1, 0, 0)); // Red
                    delay(100);
                    set_color(RGB(0, 0, 0));
                }
            }
            if (id_received == FIRST_KILOBOT)
            {
                 if (kilo_ticks > last_changed + 64)
                {
                    last_changed = kilo_ticks;
                    new_message = 0;// Reset the flag
                    set_color(RGB(0, 0, 1)); // Blue
                    delay(100);
                    set_color(RGB(0, 0, 0));
                }
            }
            
            // distance_96_64 = distance;
            // // Update the message
            // message.data[DISTANCE_96_64] = distance_96_64;
            // distance_64_32 = distance_primary;
            // message.data[DISTANCE_64_32] = distance_64_32;
            
            
    //         // if (kilo_ticks > last_changed + 64)
    //         // {
    //         //   new_message = 1;
    //         // }
        }
        
        // if (new_message == 1 && id_received == FIRST_KILOBOT && kilo_uid == SECOND_KILOBOT)
        // //if (new_message == 1  && kilo_uid == SECOND_KILOBOT)
        // {
        //     new_message = 0; // Reset the flag
        //     set_color(RGB(1, 0, 0)); // Red
        //     delay(100);
        //     set_color(RGB(0, 0, 0));
        // }
     
    
}


void setup()
{
//     // Initialize message:
    //Setup for Hop Count
    if (kilo_uid == ORIGIN_KILOBOT)
    {
        own_gradient = 0;
    }
    
    distance_64_32 = 0;     //distance of 64 from 32
    distance_96_32 = 0;     //distance of 96 from 32
    distance_96_64 = 0;  //distance of 96 from 64 
    x_coordinate = 0;
    y_coordinate = 0;
    delta_x = 0;
    delta_y =0;
    
    // Set the transmission message. The type is always NORMAL
    message.type = NORMAL;
    
    message.data[UID] = kilo_uid;
    message.data[DISTANCE_64_32] = distance_64_32;
    message.data[DISTANCE_96_32] = distance_96_32;
    message.data[DISTANCE_96_64] = distance_96_64;
    message.data[X_COORD] = x_coordinate;
    message.data[Y_COORD] = y_coordinate;
    message.data[DELTA_X] = delta_x;
    message.data[DELTA_Y] = delta_y;
    message.data[HOP_COUNT] = own_gradient;
    /*It's important that the CRC is computed after the data has been set;
    otherwise it would be wrong and the message would be dropped by the
    receiver.*/
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
        distance = estimate_distance(d);
        received_gradient = m->data[0];
        
        id_received =  m->data[UID];
        distance_primary = m->data[DISTANCE_64_32];
        distance_secondary = m->data[DISTANCE_96_32];
        distance_tertiary = m->data[DISTANCE_96_64];
    }
}


void loop()
{
    three_way_selective_listening();
}


void message_tx_success() {
   message_sent = 1;
}


int main()
{
    kilo_init();                    // initialize hardware
    kilo_message_rx = message_rx;   // Register the message_tx callback function.
    kilo_message_tx = message_tx;   // Register the message_tx_success callback function.
    kilo_message_tx_success = message_tx_success; // Register the message_tx_success callback function.
    kilo_start(setup, loop);        // start program
    
    return 0;
}


/*
phi = ((d_64)^2 - (d_64_96)^2 + (d_96)^2)/ (2* d_64)
lamda = sqrt(abs((d_96)^2 - (phi)^2)))
*/

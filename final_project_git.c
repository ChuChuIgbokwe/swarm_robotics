#include <kilolib.h>
#include <math.h>

#define UID 0
#define DISTANCE 1
#define X_COORD 2
#define Y_COORD 3
#define DEDX 4
#define DEDY 5
#define DELTAX 6
#define DELTAY 7
#define HOP_COUNT 8

//KILOBOT UNIQUE ID'S
#define ORIGIN_KILOBOT 32
#define FIRST_KILOBOT 64
#define SECOND_KILOBOT 96
#define THIRD_KILOBOT 128
#define GRADIENT_MAX 255


int new_message = 0;    // Flag to keep track of new messages.
int distance;
int own_gradient = GRADIENT_MAX;
int received_gradient = 0;
float alpha = 0.2;  //0 < alpha << 1
int id_received = 0;

uint32_t last_gradient_anchored;
message_t message;


void count_the_blinks(int r,int g,int b,int count)
{
    int i;
    //Blink white three times in between counts
    set_color(RGB(1, 1, 1)); // White
    delay(100);
    set_color(RGB(0, 0, 0)); // Off
    delay(100);
    set_color(RGB(1, 1, 1)); // White
    delay(100);
    set_color(RGB(0, 0, 0)); //Off
    delay(100);
    set_color(RGB(1, 1, 1)); // White
    delay(100);
    
    // Blink
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


void selective_listening__to_first_kilobot_from_origin()
{   if (kilo_uid == ORIGIN_KILOBOT)
    {
        if (new_message == 1  && id_received ==  FIRST_KILOBOT)
        {
            // Reset the flag so the LED is only blinked once per message.
            new_message = 0;
            //
            distance_to_light(distance);    
        }
    }
}

void selective_listening__to_second_kilobot_from_origin()
{   if (kilo_uid == ORIGIN_KILOBOT)
    {
        if (new_message == 1  && kilo_uid ==  SECOND_KILOBOT)
        {
            // Reset the flag so the LED is only blinked once per message.
            new_message = 0;
            //
            distance_to_light(distance);    
        }
    }
}

void selective_listening__to_third_kilobot_from_second()
{   if (kilo_uid == SECOND_KILOBOT)
    {
        if (new_message == 1  && kilo_uid ==  THIRD_KILOBOT)
        {
            // Reset the flag so the LED is only blinked once per message.
            new_message = 0;
            //
            distance_to_light(distance);    
        }
    }
}


void setup()
{
//     // Initialize message:
//     // The type is always NORMAL.
//     message.type = NORMAL;
//     // Some dummy data as an example.
//     message.data[0] = 0;
//     // It's important that the CRC is computed after the data has been set;
//     // otherwise it would be wrong and the message would be dropped by the
//     // receiver.
//     message.crc = message_crc(&message);
    
    //Setup for Hop Count
    if (kilo_uid == ORIGIN_KILOBOT)
    {
        own_gradient = 0;
    }
    
    // Set the transmission message.
    message.type = NORMAL;
    message.data[HOP_COUNT] = own_gradient;
    message.data[UID] = kilo_uid;
    message.crc = message_crc(&message);
}


message_t *message_tx()
{
    return &message;
}


void message_rx(message_t *m, distance_measurement_t *d)
{
    // Set the flag on message reception.
    //new_message = 1;
    distance = estimate_distance(d);
    
     // Only process this message if the previous one has been processed.
    if (new_message == 0)
    {
        new_message = 1; // Set the flag on message reception.
        received_gradient = m->data[HOP_COUNT];
        id_received =  m->data[UID];
    }
    
}


void loop()
{
            //THIS PART WORKS
    
    
//  // Update whenever a message is received.
//     // Blink the LED yellow whenever a message is received.
//     if (new_message == 1)
//     {
//         // Reset the flag so the LED is only blinked once per message.
//         new_message = 0;
//         distance_to_light(distance);
        
            //SO DOES THIS PART
        
        
        // if (distance < 50)
        // {
        //     set_color(RGB(1, 1, 0));//yellow
            
        // }
        // else 
        // {
        //     set_color(RGB(1, 0, 0));//red
        // }
    //}
        
        
    //     set_color(RGB(1, 1, 0));    //yellow
    //     delay(100);
    //     set_color(RGB(0, 0, 0));
    // }
    
     // Only pay attention to messages if this robot is not the seed.
   
    //gradient_adaptive();    // Encapsulation works :)
    selective_listening__to_first_kilobot_from_origin();
}

int main()
{
    kilo_init();                    // initialize hardware
    kilo_message_rx = message_rx;   // Register the message_tx callback function.
    kilo_message_tx = message_tx;   // Register the message_tx_success callback function.
    kilo_start(setup, loop);        // start program
    
    return 0;
}


/*
phi = ((d_64)^2 - (d_64_96)^2 + (d_96)^2)/ (2* d_64)
lamda = sqrt(abs((d_96)^2 - (phi)^2)))
*/

// void  get_coordinates(){
//  if (kilo_uid ==  ORIGIN_KILOBOT){
//      if (new_message == 1){

//      }

//  message.data[X_COORD] = 0;
//  message.data[Y_COORD] = 0;

//  }
// }

//  if (kilo_uid == FIRST_KILOBOT)
//  {
//      message.data[X_COORD] = distance;
//      message.data[Y_COORD] = 0;
//  }

//  if (kilo_uid == SECOND_KILOBOT)
//  {

//  }

//  if(message_received->data[ID_INDEX] == ORIGIN_BOT && message_received->data[VALID_COORD_INDEX]){    
//  // if receiving the message from the origin bot
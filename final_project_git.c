#include <kilolib.h>
#include <math.h>
#define DEBUG
#include <debug.h>


#define NUMBEROFSENSORS 2

#define UID 0
#define X_ORIGIN    1
#define Y_ORIGIN    2
#define X1          3
#define Y1          4
#define X2          5 
#define Y2          6
#define HOP_COUNT   7
#define DISTANCE    8

//KILOBOT UNIQUE ID'S
#define ORIGIN_KILOBOT  32
#define FIRST_KILOBOT   64
#define SECOND_KILOBOT  96
#define THIRD_KILOBOT   128
#define GRADIENT_MAX    255

int new_message = 0;            // Flag to keep track of new messages.
int own_gradient = GRADIENT_MAX;
int id_received = 0;            //used to track the UID of a kilobot sending a message

 
int dji[NUMBEROFSENSORS];
int dji_hat[NUMBEROFSENSORS]; 
int Error = 0;


int x_origin, y_origin, delta_x , delta_y;
int x1, yy1, x2, y2, x_coord[2], y_coord[2]; // y1 is a built in function in one of the above libraries
int x_0, y_0, x_1, y_1, x_2, y_2;

//declare distances
int distance;
int distance_current =0;
int distance_64_32;     // distance between kilobot 64 to kilobot 32
int distance_96_32;     // distance between kilobot 96 to kilobot 32
int distance_96_64;     // distance between kilobot 96 to kilobot 64 

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
    // for (i = 0; i < 3; i++)
    // {
    //     set_color(RGB(1, 1, 1)); // White
    //     delay(100);
    // }
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
    printf("hop count working");
}


void blink_coordinates(int origin_x, int origin_y, int first_x, int first_y, int  second_x, int second_y)
{
    int tens_x_origin, tens_y_origin, units_x_origin, units_y_origin;
    int tens_x_first, tens_y_first, units_x_first, units_y_first;
    int tens_x_second, tens_y_second, units_x_second, units_y_second;
    int i;
    if (kilo_uid == ORIGIN_KILOBOT)
    {
        // origin_x = x_0;
        // origin_y = y_0;
        //tens
        tens_x_origin = origin_x / 10;
        tens_y_origin = origin_y / 10;
        //units
        units_x_origin = origin_x % 10;
        units_y_origin = origin_y % 10;
        
        //Blink x coordinates
        if (tens_x_origin == 0)
        {count_the_blinks(1,1,0,1);}// blink yellow  
        else 
        {count_the_blinks(1,0,0,tens_x_origin);} //Red 
        if (units_x_origin == 0)
        {count_the_blinks(1,1,0,1);}// blink yellow    
        else
        {count_the_blinks(0,0,1,units_x_origin);} //Blue
        
        // Blink white thrice
        for (i = 0; i < 3; i++)
        {
        set_color(RGB(1, 1, 1)); // White
        delay(100);
        
        //Blink y coordinates
        if (tens_y_origin == 0)
        {count_the_blinks(1,1,0,1);}// blink yellow}
        else
        {count_the_blinks(1,0,0,tens_y_origin);}//Red
        if(units_y_origin == 0)
        {count_the_blinks(1,1,0,1);}
        else
        {count_the_blinks(0,0,1,units_y_origin);} //Blue
    }

}

    if (kilo_uid == FIRST_KILOBOT)
    {
        /*first_x = x_1;
        first_y = y_1;*/
        //tens
        tens_x_first = first_x / 10;
        tens_y_first = first_y / 10;
        //units
        units_x_first= first_x % 10;
        units_y_first = first_y % 10;
        //Blink x coordinates
        if (tens_x_first == 0)
        {count_the_blinks(1,1,0,1);}// blink yellow  
        else 
        {count_the_blinks(1,0,0,tens_x_first);} //Red 
        if (units_x_first == 0)
        {count_the_blinks(1,1,0,1);}// blink yellow    
        else
        {count_the_blinks(0,0,1,units_x_first);} //Blue
        
           // Blink white thrice
        for (i = 0; i < 3; i++)
        {
        set_color(RGB(1, 1, 1)); // White
        delay(100);
        }
        //Blink y coordinates
        if (tens_y_first == 0)
        {count_the_blinks(1,1,0,1);}// blink yellow}
        else
        {count_the_blinks(1,0,0,tens_y_first);}//Red
        if(units_y_first == 0)
        {count_the_blinks(1,1,0,1);}
        else
        {count_the_blinks(0,0,1,units_y_first);} //Blue

    }
 if (kilo_uid == SECOND_KILOBOT)
    {
        // second_x = x_2;
        // second_y = y_2;
        
        //tens
        tens_x_second = second_x / 10;
        tens_y_second = second_y / 10;
        //units
        units_x_second = second_x % 10;
        units_y_second = second_y % 10;
        //Blink x coordinates
        if (tens_x_second == 0)
        {count_the_blinks(1,1,0,1);}// blink yellow  
        else 
        {count_the_blinks(1,0,0,tens_x_second);} //Red 
        if (units_x_second == 0)
        {count_the_blinks(1,1,0,1);}// blink yellow    
        else
        {count_the_blinks(0,0,1,units_x_second);} //Blue
        
           // Blink white thrice
        for (i = 0; i < 3; i++)
        {
        set_color(RGB(1, 1, 1)); // White
        delay(100);
        }
        //Blink y coordinates
        if (tens_y_second == 0)
        {count_the_blinks(1,1,0,1);}// blink yellow}
        else
        {count_the_blinks(1,0,0,tens_y_second);}//Red
        if(units_y_second == 0)
        {count_the_blinks(1,1,0,1);}
        else
        {count_the_blinks(0,0,1,units_y_second);} //Blue
    }

}


    int distance_64_32_received = 0;
    int distance_96_32_received = 0;
    int distance_96_64_received = 0;

void three_way_selective_listening()
{   
    int i;
    if(message_sent)
    {
        // set_color(RGB(1, 1, 1)); // White
        // delay(100);
        // set_color(RGB(0, 0, 0));
        message_sent = 0;
    }
        
    if (new_message == 1)
    {
        
        // //if (new_message == 1 && id_received == ORIGIN_KILOBOT && kilo_uid == FIRST_KILOBOT)
        // if (kilo_uid == FIRST_KILOBOT)
        // {   
        //     if (id_received == ORIGIN_KILOBOT)
        //     {   distance_64_32_received = 1;
        //         printf("***distance_64_32_received*****\r\n\n");
        //         distance_64_32 = distance/10;
        //         dji_hat[0] = distance_64_32;
        //         new_message = 0;// Reset the flag


                
        //         // if (kilo_ticks > last_changed + 64)
        //         // {
        //         //     last_changed = kilo_ticks;
        //         //     new_message = 0;// Reset the flag
        //         //     // set_color(RGB(1, 1, 0)); // Yellow
        //         //     // delay(100);
        //         //     // set_color(RGB(0, 0, 0));
        //         //     distance_64_32 = distance/10;
        //         //     dji_hat[0] = distance_64_32;    //store the distance in the array dji_hat
                    

        //         // }
                
                
        //     }
        //     if (id_received == SECOND_KILOBOT)//if works too
        //     {
        //         distance_96_64_received = 1;
        //         // if (kilo_ticks > last_changed + 64)
        //         // {
        //         //     last_changed = kilo_ticks;
        //         //     new_message = 0;// Reset the flag
        //         //     // set_color(RGB(0, 1, 0)); // Green
        //         //     // delay(100);
        //         //     // set_color(RGB(0, 0, 0));
        //         //     distance_96_64 = distance/10;
                   
        //         // }
        //     }

        // }
        //if (new_message == 1 && id_received == SECOND_KILOBOT && kilo_uid == ORIGIN_KILOBOT)
        if (kilo_uid == ORIGIN_KILOBOT)
        {   
            if (id_received == SECOND_KILOBOT)
            {
                distance_96_32_received = 1;
                printf("****distance_96_32_received***\r\n\n");
                
                // distance_96_32 = distance/10;
                // dji_hat[1] = distance_96_32;
                // new_message = 0;// Reset the flag
                
                if (kilo_ticks > last_changed + 64)
                {
                    last_changed = kilo_ticks;
                    new_message = 0;// Reset the flag
                    set_color(RGB(1, 0, 0)); // Red
                    delay(100);
                    set_color(RGB(0, 0, 0));

                    distance_96_32 = distance/10;
                    dji_hat[1] = distance_96_32;    //store the distance in the array dji_hat
                    message.crc = message_crc(&message);
                    //message_to_send.data[DISTANCE_96_32] = distance_96_32;
                    printf("origin kilobot received message from second kilobot\r\n");
                    
                }
            }
            if (id_received == FIRST_KILOBOT) //if works too
            {
                distance_64_32_received = 1;
                printf("***distance_64_32_received*****\r\n\n");
                
                // distance_64_32 = distance/10;
                // dji_hat[0] = distance_64_32;
                new_message = 0;// Reset the flag
                 if (kilo_ticks > last_changed + 64)
                {
                    last_changed = kilo_ticks;
                    new_message = 0;// Reset the flag
                    set_color(RGB(0, 0, 1)); // Blue
                    delay(100);
                    set_color(RGB(0, 0, 0));
                    distance_64_32 = distance/10;
                    dji_hat[0] = distance_64_32;    //store the distance in the array dji_hat
                    message.crc = message_crc(&message);
                    printf("origin kilobot received message from first kilobot\r\n");
                   
                }
            }
 
        }
        
    /*
phi = ((d_64_32)^2 - (d_96_64)^2 + (d_96_32)^2)/ (2* d_64_32)
lamda = sqrt(abs((d_96_32)^2 - (phi)^2)))
*/
    // if (distance_64_32_received && distance_96_32_received && distance_96_64_received)
    // {
        distance_64_32_received = 0;
        distance_96_32_received = 0;
        distance_96_64_received = 0;
        printf("*****FLAG RESET *****\r\n\n\n\n");
        int Error_temp, delta_x, delta_y;
        //int dEdx_temp, dEdy_temp;
        int dEdx = 0;
        int dEdy = 0;

         if (kilo_ticks > last_changed + 64)
                {
                    last_changed = kilo_ticks;
                    new_message = 0;// Reset the flag
                    
                   
                


        for (i = 0; i < NUMBEROFSENSORS; i++)
        {
            dji[i] = sqrt(pow((x_coord[i] - x_origin),2) + pow((y_coord[i] - y_origin),2));
        }

        for (i = 0; i < NUMBEROFSENSORS; i++)
        {
            Error_temp  = pow((dji[i]-dji_hat[i]),2);
            Error = Error + Error_temp;
        }

        
        //for (i = 0; i < NUMBEROFSENSORS; i++)
        //{
            //dEdx_temp = (x_coord[i] - x_origin) * (1 -(dji[i]/dji_hat[i]));
            //dEdy_temp = (y_coord[i] - y_origin) * (1 -(dji[i]/dji_hat[i]));
            //dEdx = dEdx + dEdx_temp;
            //dEdy = dEdy + dEdy_temp;
        //}
        dEdx = (x_coord[0] - x_origin) * (1 -(dji[0]/dji_hat[0])) + (x_coord[1] - x_origin) * (1 -(dji[1]/dji_hat[1]));
        dEdy = (y_coord[0] - y_origin) * (1 -(dji[0]/dji_hat[0])) + (y_coord[1] - y_origin) * (1 -(dji[1]/dji_hat[1]));
    
        delta_x = -alpha * dEdx;
        delta_y = -alpha * dEdy;

        //update coordinates for origin kilobot
        x_origin = x_origin + round(delta_x);
        y_origin = y_origin + round(delta_y);
        // update coordinates for first kilobot
        x1 = x1 + round(delta_x);
        yy1 = yy1 + round(delta_y);
        //update coordinates for second kilobot
        x2 = x2 + round(delta_x);
        y2 = y2 + round(delta_y);

        message.data[X_ORIGIN] = x_origin;
        message.data[Y_ORIGIN] = y_origin;
        message.data[X1] = x1;
        message.data[Y1] = yy1;
        message.data[X2] = x2;
        message.data[Y2] = y2;
        message.crc = message_crc(&message);
        //set_color(RGB(0, 0, 1)); // Blue
        //blink_coordinates(x_origin, y_origin, x1, yy1, x2, y2);
        //new_message = 0;

        //DEBUGGING
        printf("x origin = %d, y_origin = %d\r\n",x_origin, y_origin);
        printf("x1 = %d, y1 = %d\r\n",x_coord[0], y_coord[0]);
        printf("x2 = %d, y2 = %d\r\n",x_coord[1], y_coord[1]);
        //printf("dEdx_temp = %d, dEdy_temp = %d\r\n",dEdx_temp,dEdy_temp);
        //printf("kilo_uid is %d\n\r\n", kilo_uid);
        printf("dEdx = %d, dEdy = %d\r\n", dEdx,dEdy);
        printf("delta x = %d, delta y = %d\r\n",delta_x, delta_y);
        printf(" distance_64_32 = %d ,distance_96_64 = %d, distance_96_32 = %d\r\n",distance_64_32,distance_96_64,distance_96_32 );
        printf("dji[0] = %d, dji[1] = %d\r\n", dji[0],dji[1]);
        printf("dji_hat[0] = %d, dji_hat[1] = %d\r\n", dji_hat[0],dji_hat[1]);
    }
    }
    //}     
        
}


void setup()
{   rand_seed(rand_hard());
//     // Initialize message:
    //Setup for Hop Count
    if (kilo_uid == ORIGIN_KILOBOT)
    {
        own_gradient = 0;
    }
    
    
    /*Generate random coordinates for each kilobot. We're using a unit square of 10 cm by 10 cm.
    so the maximum coordinates a kilobot can have are (10,10).rand_soft is a uint8_t pseudo number generator.
    i.e it generates random numbers between 0-255 modulo 11 means we can have remainders of 10
    */
    x_origin = rand_soft() % 11;
    y_origin = rand_soft() % 11;
    x1 = rand_soft() % 11;
    yy1 = rand_soft() % 11;
    x2 = rand_soft() % 11;
    y2 = rand_soft() % 11;
    x_coord[0] = x1;
    x_coord[1] = x2;
    y_coord[0] = yy1;
    y_coord[1] = y2;
    delta_x = 0;
    delta_y =0;
    
    // Set the transmission message. The type is always NORMAL
    message.type = NORMAL;
    
    message.data[UID] = kilo_uid;
    message.data[X_ORIGIN] = x_origin;
    message.data[Y_ORIGIN] = y_origin;
    message.data[X1] = x1;
    message.data[Y1] = yy1;
    message.data[X2] = x2;
    message.data[Y2] = y2;
    message.data[HOP_COUNT] = own_gradient;
    message.data[DISTANCE] = distance;
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
        
        
        id_received =  m->data[UID];
        x_0 = m->data[X_ORIGIN];
        y_0 = m->data[Y_ORIGIN];
        x_1 = m->data[X1];
        y_1 = m->data[Y1];
        x_2 = m->data[X2];
        y_2 = m->data[Y2];
        received_gradient = m->data[HOP_COUNT];
        distance_current = m->data[DISTANCE];
    }
}


void loop()
{ 
    printf("******ENTERED LOOP*******\r\n\n\n\n");
    three_way_selective_listening();
    // for (i = 0; i < 9; i++)
    // {
    //     printf("message[%d] = %d\n",message.data[i]);
    // }
}


void message_tx_success() {
   message_sent = 1;
}


int main()
{
    kilo_init();   
    debug_init();                 // initialize hardware
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

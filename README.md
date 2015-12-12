###OBJECTIVES
The goal of this project was to implement the algorithms in the Organizing a Global Coordinate System from Local Information on an Ad Hoc Sensor Network paper by Nagpal et al. The paper proposes using gradient descent and multilateration to create a local coordinate system

##Algorithm and Setup
I used three kilobots with Unique ID’s 32, 64 and 96 also referred to as the origin, first and second kilobot respectively. Where origin is the seed with subscript i and first and second are the sensors.
I make use of a unit square 100mm x 100mm which  corresponds to the range of a the kilobots distance sensor. The positions of the three sensors are chosen randomly - random x and y coordinates for each- using the seeded rand_soft() function.
The gradient descent is achieved by using the  hop count in gradient_adaptive.c file found in the kilobot labs
The multilateration is achieved by following algorithm

![equation](http://latex.codecogs.com/gif.latex?d_j_i%20%3D%20%5Csqrt%7B%28x_%7Bi%7D%20-%20x_%7Bj%7D%29%5E2%20&plus;%20%7B%28y_%7Bi%7D%20-%20y_%7Bj%7D%29%5E2%7D%7D)

Where dji is an array made up the manhattan distance ‘originfirst’ and ‘originsecond’. The least squared error is computed by

![equation](http://latex.codecogs.com/gif.latex?E_j%20%3D%20%5Csum_%7Bi%3D1%7D%5E%7Bn%7D%20%28d_j_i-%5Cbar%7Bd_j_i%7D%29)

![equation] (http://latex.codecogs.com/gif.latex?%5Cbar%7Bd_j_i%7D)
is an array containing the distance sensed by the kilobots distance sensors made up of distances ‘originfirst’ and ‘originsecond’. A pseudo gradient descent is performed  and the results are used to update the  coordinates where ![equation](http://latex.codecogs.com/gif.latex?0%3C%20%5Calpha%20%5Cll%201).I chose alpha = 0.2.

![equation](http://latex.codecogs.com/gif.latex?%5Cfrac%7B%5Cpartial%20E_j%7D%7B%5Cpartial%20x_j%7D%20%3D%20%5Csum_%7Bi%3D1%7D%5E%7Bn%7D%28x_i%20-%20x_j%29%281%20-%20%5Cfrac%7B%5Cmathrm%7Bd_j_i%7D%20%7D%7B%5Cmathrm%7Bd%7D%5Cbar%7B_j_i%7D%7D%29)
and 

![equation](http://latex.codecogs.com/gif.latex?%5Cfrac%7B%5Cpartial%20E_j%7D%7B%5Cpartial%20y_j%7D%20%3D%20%5Csum_%7Bi%3D1%7D%5E%7Bn%7D%28y_i%20-%20y_j%29%281%20-%20%5Cfrac%7B%5Cmathrm%7Bd_j_i%7D%20%7D%7B%5Cmathrm%7Bd%7D%5Cbar%7B_j_i%7D%7D%29)

![equation](http://latex.codecogs.com/gif.latex?%5CDelta%20x%20%3D%20-%5Calpha%20%5Cfrac%7B%5Cpartial%20E_j%7D%7B%5Cpartial%20x_j%7D)

![equation] (http://latex.codecogs.com/gif.latex?%5CDelta%20y%20%3D%20-%5Calpha%20%5Cfrac%7B%5Cpartial%20E_j%7D%7B%5Cpartial%20y_j%7D)

##RESULTS
+ I got the hop count working easily as the code for that already existed. I did not incorporate movement into mine so I was not able to replicate the sensors forming concentric circles around the seed
Using the debugger I was able to confirm random coordinates were being chosen
+ I was able to get the kilobots to blink out it’s coordinates in a morse code fashion. It blinks its x cooordinates followed by threee green blinks then it’s y coordinates. It blinks red once for each ten and blue for each unit, so position (10,4) would be one red blink, three green blinks then four blue blinks.
+ When moved the kilobots update their new positions
+ In the paper the algorithm was implemented using a minimum of three sensors, I used two. 
Handling the distances between proved tricky, however I was able to get it working reasonably well and the coordinates update on when in debugging mode and with their LED’s
+ I suspect I have a bug in my code. In the video when I move around the kilobots, their coordinates update but kilobot 96 blinks red more than once which should be impossible from a physical standpoint given that one red blink is equal to 10 cm.
+ I tried using multiple unsuccessfully messages by message_t message[2] unsuccessfully

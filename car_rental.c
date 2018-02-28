/* External definitions for car-rental model. */

#include "simlib.h"		/* Required for use of simlib.c. */
#define EVENT_ARRIVAL_1         1	/* Event type for arrival of a person to the
				   location 1. */
#define EVENT_ARRIVAL_2         2	/* Event type for arrival of a person to the
				   location 2. */
#define EVENT_ARRIVAL_3         3	/* Event type for arrival of a person to the
				   location 3. */
#define EVENT_DEPARTURE       2	/* Event type for departure of a person from a
				   the bus. */
#define EVENT_END_SIMULATION  3	/* Event type for end of the simulation. */
#define STREAM_INTERARRIVAL_1   1	/* Random-number stream for interarrivals at location 1. */
#define STREAM_INTERARRIVAL_2   2	/* Random-number stream for interarrivals at location 2. */
#define STREAM_INTERARRIVAL_3   3	/* Random-number stream for interarrivals at location 3. */
#define STREAM_UNLOADING   4	/* Random-number stream for unloading times. */
#define STREAM_LOADING   5	/* Random-number stream for loading times. */
#define STREAM_DESTINATION   6	/* Random-number stream for determining destination of an arrival at the car rental. */

#define MAX_NUM_BUS 1	/* Maximum number of bus. */
#define MAX_SEATS    20	/* Maximum number of stations. */
#define MAX_NUM_LOCATION     3	/* Maximum number of locations. */


/* Declare non-simlib global variables. */

int waiting_time, unload_a, unload_b, load_a, load_b, speed,length_simulation, interarrival_rate_1, interarrival_rate_2, interarrival_rate_3, num_bus, num_location, i, j, num_seats,
  num_person[MAX_NUM_LOCATION+ 1],
  route[MAX_NUM_LOCATION + 1][MAX_NUM_BUS + 1], num_seats_taken[MAX_NUM_BUS + 1], location, person;
double mean_interarrival_1, mean_interarrival_2, mean_interarrival_3, prob_distrib_destination[26],
  mean_service[MAX_NUM_LOCATION + 1][MAX_NUM_BUS + 1], destination[MAX_NUM_LOCATION+1];
FILE *infile, *outfile;

int
main ()				/* Main function. */
{
  /* Open input and output files. */

  infile = fopen ("car_rental.in", "r");
  outfile = fopen ("car_rental.out", "w");

  /* Read input parameters. */

  fscanf (infile, "%d %d", &num_bus, &num_location);
  
  // INTERARRIVAL RATE PER HOUR AT LOCATION 1, 2, 3
  fscanf (infile, "%d %d %d", &interarrival_rate_1, &interarrival_rate_2, &interarrival_rate_3);
  // interarrival kedatangan orang dalam detik
  mean_interarrival_1 = 3600/interarrival_rate_1;
  mean_interarrival_2 = 3600/interarrival_rate_2;
  mean_interarrival_3 = 3600/interarrival_rate_3;
 
  // KAPASITAS PER BUS, KECEPATAN PER HOUR
  fscanf (infile, "%d %d", &num_seats, &speed);
  speed = speed * 3600; // miles per second
  
  // initial location: 3
  
  // PELUANG PERSON TURUN KE DESTINATION 1, 2, dan 3
  for (i = 1; i <= num_location; ++i)
    fscanf (infile, "%lg", &prob_distrib_destination[i]);

  // JARAK COUNTER CLOCKWISE LOCATION 3->1, 1->2, 2->3 in MILES
  for (i = 1; i <= num_location; ++i)
    fscanf (infile, "%lg", &destination[i]);
    
  // replace jarak dengan waktu tempuh  
  for (i = 1; i<= num_location; ++i) {
	destination[i] = speed/destination[i];
  }

  // UNLOAD SECONDS
  fscanf (infile, "%d %d", &unload_a, &unload_b);

  // LOAD SECONDS
  fscanf (infile, "%d %d", &load_a, &load_b);

  // WAITING_TIME in MINUTES
  fscanf (infile, "%d", &waiting_time);
  waiting_time = waiting_time * 60; // waiting time in seconds
  
  // LENGTH SIMULATION 80 hours
  fscanf (infile, "%d", &length_simulation);
  length_simulation = length_simulation * 3600; // simulation length in seconds
  fclose (infile);
  fclose (outfile);
  
  /*
  printf("Jumlah lokasi: %d\n", num_location);
  printf("Unload a: %d\n", unload_a);
  printf("Distance 3 to 1 %lg\n", destination[1]);
  printf("Kecepatan: %d\n", speed);
  printf("Length simulation %d\n", length_simulation);
  */ 

  return 0;
}

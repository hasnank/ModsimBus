/* 
 * IF 4021 Pemodelan dan Simulasi
 * NAMA - NIM: Hasna Nur Karimah - 13514106
 * NAMA - NIM: Audry Nyonata     - 13515087
 * Tanggal: 2 Maret 2018
 */

/* External definitions for car-rental model. */

#include "simlib.h"		/* Required for use of simlib.c. */
#define EVENT_ARRIVAL_1         1	/* Event type for arrival of a person to the
				   location 1. */
#define EVENT_ARRIVAL_2         2	/* Event type for arrival of a person to the
				   location 2. */
#define EVENT_ARRIVAL_3         3	/* Event type for arrival of a person to the
				   location 3. */
#define EVENT_LOAD       4	/* Event type for loading of a person to the bus. */
#define EVENT_DEPARTURE       5	/* Event type for departure of a person from a
				   the bus. */
#define EVENT_UNLOAD       6	/* Event type for unloading of a person from the bus. */

#define EVENT_END_SIMULATION  7	/* Event type for end of the simulation. */
#define STREAM_INTERARRIVAL_1   1	/* Random-number stream for interarrivals at location 1. */
#define STREAM_INTERARRIVAL_2   2	/* Random-number stream for interarrivals at location 2. */
#define STREAM_INTERARRIVAL_3   3	/* Random-number stream for interarrivals at location 3. */
#define STREAM_UNLOADING   4	/* Random-number stream for unloading times. */
#define STREAM_LOADING   5	/* Random-number stream for loading times. */
#define STREAM_DESTINATION   6	/* Random-number stream for determining destination of an arrival at the car rental. */

#define MAX_NUM_BUS 1	/* Maximum number of bus. */
#define MAX_SEATS    20	/* Maximum number of seat in a bus. */
#define MAX_NUM_LOCATION     3	/* Maximum number of locations. */


/* Declare non-simlib global variables. */

int arrival_location, waiting_time, unload_a, unload_b, load_a, load_b, speed,length_simulation, num_bus, num_location, i, j, num_seats,
  num_person[MAX_NUM_LOCATION+ 1], interarrival_rate[MAX_NUM_LOCATION+1], num_seats_taken, destination, person, bus_position;
double prob_dest, mean_interarrival[MAX_NUM_LOCATION+1], prob_distrib_destination[26],
  mean_service[MAX_NUM_LOCATION + 1][MAX_NUM_BUS + 1], distance[MAX_NUM_LOCATION+1][MAX_NUM_LOCATION+1], load_time, unload_time;
FILE *infile, *outfile;

/* If no person is departing from bus, generate the time of this person
	  to get on bus and determine the destination and number of person queueing in front of him. */
	  
void
arrive(int arrival_location)		/* Function to serve an arrival event of a person
				   to the location arrival_location */
{
	switch (arrival_location) {
		case 1: 
			event_schedule (sim_time + expon (mean_interarrival[1], STREAM_INTERARRIVAL_1), EVENT_ARRIVAL_1);
			destination = 3;
			person = 0;
			
			if (list_size[arrival_location] == 0) {
				transfer[3] = 3; // destination
				transfer[4] = 0; // how many person queueing in front of him
			printf("Arrival_location 1: %0.3f %0.3f %0.3f %0.3f\n", transfer[1], transfer[2], transfer[3], transfer[4]);
				list_file (LAST, arrival_location);
				// event_schedule			
					
			} else {
				 /* This person is not the first in his queue
				 * so place the arriving person at the end of the appropriate queue.
				 1. Time of arrival to this location.
				 2. Destination.
				 3. How many person is queueing in front of him.  */
				transfer[1] = sim_time;
				transfer[2] = destination;
				transfer[3] = list_size[arrival_location];
			printf("Arrival_location 1 queue: %0.3f %0.3f %0.3f \n", transfer[1], transfer[2], transfer[3]);
				list_file (LAST, arrival_location);
			}
			break;
		case 2:
			arrival_location = 2;
			event_schedule (sim_time + expon (mean_interarrival[2], STREAM_INTERARRIVAL_2), EVENT_ARRIVAL_2);
			destination = 3;
			person = 0;
			
			if (list_size[arrival_location] == 0) {
				transfer[3] = destination;
				transfer[4] = person;
			printf("Arrival_location 2 : %0.3f %0.3f %0.3f %0.3f\n", transfer[1], transfer[2], transfer[3], transfer[4]);
				list_file (LAST, arrival_location);
				// event_schedule			
					
			} else {
				 /* This person is not the first in his queue
				 * so place the arriving person at the end of the appropriate queue.
				 1. Time of arrival to this location.
				 2. Destination.
				 3. How many person is queueing in front of him.  */
				transfer[1] = sim_time;
				transfer[2] = destination;
				transfer[3] = list_size[2];
			printf("Arrival_location 2 queue : %0.3f %0.3f %0.3f \n", transfer[1], transfer[2], transfer[3]);
				list_file (LAST, arrival_location);
			}
			break;
		case 3:
			event_schedule (sim_time + expon (mean_interarrival[3], STREAM_INTERARRIVAL_3), EVENT_ARRIVAL_3);
			prob_dest = lcgrand(STREAM_DESTINATION);
			  if (prob_dest >= prob_distrib_destination[1]) 
				destination = 1;
			  else 
				destination = 2;
			person = 0;
			
			if (list_size[3] == 0) {
				transfer[3] = destination;
				transfer[4] = person;
			printf("Arrival_location 3 : %0.3f %0.3f %0.3f %0.3f\n", transfer[1], transfer[2], transfer[3], transfer[4]);
				list_file (LAST, arrival_location);
				// event_schedule			
			} else {
				 /* This person is not the first in his queue
				 * so place the arriving person at the end of the appropriate queue.
				 1. Time of arrival to this location.
				 2. Destination.
				 3. How many person is queueing in front of him.  */
				transfer[1] = sim_time;
				transfer[2] = destination;
				transfer[3] = list_size[3];
			printf("Arrival_location 3 queue: %0.3f %0.3f %0.3f \n", transfer[1], transfer[2], transfer[3]);
				list_file (LAST, arrival_location);
			}
			break;
		default: break;
	}
}
	
int
main ()				/* Main function. */
{
  // Location awal bus
  bus_position = 3;
  
  /* Open input and output files. */
  infile = fopen ("car_rental.in", "r");
  outfile = fopen ("car_rental.out", "w");

  // Banyaknya bus, banyaknya location
  fscanf (infile, "%d %d", &num_bus, &num_location);
  
  // MEAN INTERARRIVAL RATE PER HOUR AT LOCATION 1, 2, 3
  for (i = 1; i <= num_location; ++i) {
	fscanf (infile, "%d", &interarrival_rate[i]);
	mean_interarrival[i] = 3600.0/interarrival_rate[i]; // mean interarrival in seconds
  }
  
  // KAPASITAS PER BUS, KECEPATAN PER HOUR
  fscanf (infile, "%d %d", &num_seats, &speed);
  speed = speed * 3600; // miles per second
  
  // PELUANG PERSON TURUN KE DESTINATION 1, 2, dan 3
  for (i = 1; i <= num_location; ++i)
    fscanf (infile, "%lg", &prob_distrib_destination[i]);

  // Jarak dalam mil dari location i ke location j
  for (i = 1; i <= num_location; ++i) {
	for (j = 1; j <=num_location; ++j) {
	  fscanf (infile, "%lg", &distance[i][j]);
	  distance[i][j] = speed/distance[i][j]; // replace jarak in miles menjadi waktu tempuh in seconds  
    }
  }
  
  // UNLOAD SECONDS
  fscanf (infile, "%d %d", &unload_a, &unload_b);

  // LOAD SECONDS
  fscanf (infile, "%d %d", &load_a, &load_b);

  // WAITING_TIME in MINUTES
  fscanf (infile, "%d", &waiting_time);
  waiting_time = waiting_time * 60; // waiting time in seconds
  
  // LENGTH SIMULATION in hours
  fscanf (infile, "%d", &length_simulation);
  length_simulation = length_simulation * 3600; // simulation length in seconds
  
  // END OF READING INPUT
  
  /* Write report heading and input parameters. */
  fprintf (outfile, "Car-rental model\n\n");
  fprintf (outfile, "Number of bus %d\n\n", num_bus);
  fprintf (outfile, "Number of seats in each bus %d\n\n", num_seats);
  fprintf (outfile, "Number of destination %d\n\n", num_location);
  fprintf (outfile, "Distribution function of destination  ");
  for (i = 1; i <= num_location; ++i)
    fprintf (outfile, "\n  Location %d %8.3f", i, prob_distrib_destination[i]);
  fprintf (outfile, "\n\nMean interarrival time of location   ");
  for (i = 1; i <= num_location; ++i)
    fprintf (outfile, "\n  Location %d %d per hour", i, interarrival_rate[i]);
  fprintf (outfile, "\n\nLength of the simulation %d seconds", length_simulation);
  fprintf (outfile, "\n\nBus speed %d miles per second\n\n", speed);
  fprintf (outfile, "Distance from  ");
  for (i = 1; i<= num_location; ++i)
    for (j = 1; j<=num_location; ++j) {
	  fprintf (outfile, "\n  Location %d to %d  %8.3f seconds", i, j, distance[i][j]);
	}


  /* Initialize all seats in bus to the empty state. */

  for (j = 1; j <= num_seats; ++j)
    num_seats_taken = 0;

  /* Initialize simlib */

  init_simlib ();

  /* Set maxatr = max(maximum number of attributes per record, 4) */

  maxatr = 4;			/* NEVER SET maxatr TO BE SMALLER THAN 4. */
  
  /* Schedule the arrival of the first person in each location. */

  event_schedule (expon (mean_interarrival[1], STREAM_INTERARRIVAL_1), EVENT_ARRIVAL_1);
  event_schedule (expon (mean_interarrival[2], STREAM_INTERARRIVAL_2), EVENT_ARRIVAL_2);
  event_schedule (expon (mean_interarrival[3], STREAM_INTERARRIVAL_3), EVENT_ARRIVAL_3);

  /* Schedule the end of the simulation.  (This is needed for consistency of
     units.) */

  event_schedule (length_simulation, EVENT_END_SIMULATION);

  /* Setting location list to stay ordered by attribute arrival time */
  for (i = 1; i<=num_location; ++i)
  {
	list_rank[i] = 1;  
  }
  
  /* Setting bus list to stay ordered by attribute destination */
  list_rank[num_location+1] = 3;

  /* Run the simulation until it terminates after an end-simulation event 
     (type EVENT_END_SIMULATION) occurs. */

  do
    {

      /* Determine the next event. */

      timing ();

	  /* Invoke the appropriate event function. */

      switch (next_event_type)
	{
	case EVENT_ARRIVAL_1:
	  arrive(1);
	  break;
    case EVENT_ARRIVAL_2:
      arrive(2);
      break;
    case EVENT_ARRIVAL_3:
      arrive(3);
      break;
    case EVENT_LOAD:
//      load ();
      break;
	case EVENT_DEPARTURE:
//	  depart ();
	  break;
	case EVENT_UNLOAD:
//      unload ();
      break;
	case EVENT_END_SIMULATION:
//	  report ();
	  break;
	}

      /* If the event just executed was not the end-simulation event (type
         EVENT_END_SIMULATION), continue simulating.  Otherwise, end the
         simulation. */

    }
  while (next_event_type != EVENT_END_SIMULATION);

  fclose (infile);
  fclose (outfile);
  
  return 0;
}

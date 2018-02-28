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
 
  // KAPASITAS PER BUS, KECEPATAN PER HOUR
  fscanf (infile, "%d %d", &num_seats, &speed);

  // initial location: 3
  
  // PELUANG PERSON TURUN KE DESTINATION 1, 2, dan 3
  for (i = 1; i <= num_location; ++i)
    fscanf (infile, "%lg", &prob_distrib_destination[i]);

  // JARAK COUNTER CLOCKWISE LOCATION 3->1, 1->2, 2->3 in MILES
  for (i = 1; i <= num_location; ++i)
    fscanf (infile, "%lg", &destination[i]);

  // UNLOAD SECONDS
  fscanf (infile, "%d %d", &unload_a, &unload_b);

  // LOAD SECONDS
  fscanf (infile, "%d %d", &load_a, &load_b);

  // WAITING_TIME in MINUTES
  fscanf (infile, "%d", &waiting_time);
  
  // LENGTH SIMULATION 80 hours
  fscanf (infile, "%d", &length_simulation);
  
  /* Initialize all seats in all busses to empty state. */

  for (j = 1; j <= num_bus; ++j)
    num_seats_taken[j] = 0;

  /* Initialize simlib */

  init_simlib ();

  /* Set maxatr = max(maximum number of attributes per record, 4) */

  maxatr = 4;			/* NEVER SET maxatr TO BE SMALLER THAN 4. */

  /* Schedule the arrival at each terminal queue. */

  event_schedule (expon (mean_interarrival_1, STREAM_INTERARRIVAL_1), EVENT_ARRIVAL_1);
  event_schedule (expon (mean_interarrival_2, STREAM_INTERARRIVAL_2), EVENT_ARRIVAL_2);
  event_schedule (expon (mean_interarrival_3, STREAM_INTERARRIVAL_3), EVENT_ARRIVAL_3);

  /* Schedule the end of the simulation.  (This is needed for consistency of
     units.) */

  event_schedule (length_simulation, EVENT_END_SIMULATION);

  /* Run the simulation until it terminates after an end-simulation event
     (type EVENT_END_SIMULATION) occurs. */

  do
    {

      /* Determine the next event. */

      timing ();

      /* Invoke the appropriate event function. */

      switch (next_event_type)
	{
	case EVENT_ARRIVAL:
	  arrive (1);
	  break;
	case EVENT_DEPARTURE:
	  depart ();
	  break;
	case EVENT_END_SIMULATION:
	  report ();
	  break;
	}

      /* If the event just executed was not the end-simulation event (type
         EVENT_END_SIMULATION), continue simulating.  Otherwise, end the
         simulation. */

    }
  while (next_event_type != EVENT_END_SIMULATION);

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

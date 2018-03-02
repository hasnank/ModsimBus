/* 
 * WITH LOG
 * Tugas 1 IF 4021 Pemodelan dan Simulasi
 * NAMA - NIM: Hasna Nur Karimah - 13514106
 * NAMA - NIM: Audry Nyonata     - 13515087
 * Tanggal: 2 Maret 2018
 */

/* External definitions for car-rental model. */

#include "simlib.h"		/* Required for use of simlib.c. */
#define EVENT_ARRIVAL_1         1	/* Event type for arrival of a person to the location 1. */
#define EVENT_ARRIVAL_2         2	/* Event type for arrival of a person to the location 2. */
#define EVENT_ARRIVAL_3         3	/* Event type for arrival of a person to the location 3. */
//#define EVENT_DEPARTURE     	4	/* Event type for a person to take off from the bus. */
#define EVENT_BUS_ARRIVE       	5	/* Event type for bus arriving at a location. */
#define EVENT_BUS_DEPART       	6	/* Event type for bus leaving a location. */
#define EVENT_END_SIMULATION  	7	/* Event type for end of the simulation. */
#define STREAM_INTERARRIVAL_1   1	/* Random-number stream for interarrivals at location 1. */
#define STREAM_INTERARRIVAL_2   2	/* Random-number stream for interarrivals at location 2. */
#define STREAM_INTERARRIVAL_3   3	/* Random-number stream for interarrivals at location 3. */
#define STREAM_UNLOADING   		4	/* Random-number stream for unloading times. */
#define STREAM_LOADING   		5	/* Random-number stream for loading times. */
#define STREAM_DESTINATION   	6	/* Random-number stream for determining destination of an arrival at the car rental. */

#define MAX_NUM_BUS 		1	/* Maximum number of bus	*/
#define MAX_SEATS    		20	/* Maximum number of seat in a bus. */
#define MAX_NUM_LOCATION    3	/* Maximum number of locations. */


/* Declare non-simlib global variables. */

int arrival_location, waiting_time, unload_a, unload_b, load_a, load_b, length_simulation, num_bus, num_location, i, j, num_seats,
  num_person[MAX_NUM_LOCATION+ 1], interarrival_rate[MAX_NUM_LOCATION+1], num_seats_taken, destination, person, bus_position,
  is_moving, total_passenger, is_loop;
double prob_dest, mean_interarrival[MAX_NUM_LOCATION+1], prob_distrib_destination[26],
  mean_service[MAX_NUM_LOCATION + 1][MAX_NUM_BUS + 1], distance[MAX_NUM_LOCATION+1][MAX_NUM_LOCATION+1], load_time, speed,
  loop_init, loop_final, arrive_time;
FILE *infile, *outfile;

void load(void) {
	while (list_size[bus_position]>0 && (num_seats_taken < 20)) {
		list_remove(FIRST,bus_position);
		transfer[3] = bus_position; //mengubah atribut yang tadinya nyimpen orang di depannya, jadi arrival_location
		printf("Masuk ke bus di jam %0.3f arrival_time: %0.3f destination: %0.3f arrival_location: %0.3f\n", sim_time+load_time, transfer[1], transfer[2], transfer[3]);
		
		if (transfer[2] == 1) {
			list_file(LAST,4);
		} else if (transfer[2] == 2) {
			list_file(LAST,5);
		} else if(transfer[2] ==3){
			list_file(LAST,6);
		}
		++num_seats_taken;
		timest((double) num_seats_taken, 1);
		++total_passenger;
		sampst(sim_time - transfer[1] + load_time, bus_position);
		load_time += uniform(load_a,load_b,STREAM_LOADING);
		event_cancel(EVENT_BUS_DEPART);
		event_schedule(sim_time+waiting_time+load_time, EVENT_BUS_DEPART);
		printf("Loading mengundur jadwal keberangkatan bus menjadi %0.3f\n", sim_time+waiting_time+load_time);
	}
	load_time = 0;
}

void unload(void) {
	load_time = 0;
	while (list_size[bus_position+3] >0) {
		load_time += uniform(unload_a, unload_b,STREAM_UNLOADING);
		list_remove(FIRST,bus_position+3);
		printf("Keluar dari bus di waktu %0.3f arrival_time: %0.3f destination: %0.3f arrival_location: %0.3f\n", load_time+sim_time, transfer[1], transfer[2], transfer[3]);
		--num_seats_taken;
		timest((double) num_seats_taken, 1);
		sampst(sim_time - transfer[1] + load_time, bus_position + 3);
	}
	event_cancel(EVENT_BUS_DEPART);
	event_schedule(sim_time+waiting_time+load_time, EVENT_BUS_DEPART);
	printf("Unloading mengundur jadwal keberangkatan bus menjadi %0.3f\n", sim_time+waiting_time+load_time);
	
}	  
void
arrive(int arrival_location)	/* Function to serve an arrival event of a person to the location arrival_location */
{
	if (arrival_location == 1) {
			event_schedule (sim_time + expon (mean_interarrival[1], STREAM_INTERARRIVAL_1), EVENT_ARRIVAL_1);
			destination = 3;
			person = 0;
	} else if (arrival_location == 2) {
			event_schedule (sim_time + expon (mean_interarrival[2], STREAM_INTERARRIVAL_2), EVENT_ARRIVAL_2);
			destination = 3;
			person = 0;
	} else if (arrival_location == 3) {
			event_schedule (sim_time + expon (mean_interarrival[3], STREAM_INTERARRIVAL_3), EVENT_ARRIVAL_3);
			prob_dest = lcgrand(STREAM_DESTINATION);
		    if (prob_dest >= prob_distrib_destination[1]) 
			  destination = 1;
		    else 
			  destination = 2;
			person = 0;
	}	
		
	if (list_size[arrival_location] == 0) {
		/* This person is the first in his location but still need to be placed at queue*/
		transfer[1] = sim_time;
		transfer[2] = destination;
		transfer[3] = person;
		printf("Arrival_location %d : %0.3f %0.3f %0.3f \n", arrival_location, transfer[1], transfer[2], transfer[3]);
		list_file (LAST, arrival_location);

		/* hanya boleh naik ketika bus sedang tidak bergerak */
		if (bus_position == arrival_location && !is_moving) {
			load();
		}		
			
	} else {
		 /* This person is not the first in his queue
		 * so place the arriving person at the end of the appropriate queue.
		 1. Time of arrival to this location.
		 2. Destination.
		 3. How many person is queueing in front of him.  */
		transfer[1] = sim_time;
		transfer[2] = destination;
		transfer[3] = list_size[arrival_location];
		printf("Arrival_location %d queue : %0.3f %0.3f %0.3f \n", arrival_location, transfer[1], transfer[2], transfer[3]);
		list_file (LAST, arrival_location);
	}
}



void bus_arrive(void) {
	int temp = bus_position; // lokasi keberangkatan bus
	is_moving = 0;
	
	/* Menentukan lokasi bus setelah pindah */
	if (bus_position == 3) {
		bus_position = 1;
	} else {
		bus_position++;
		is_loop = 1;
	}
	
	/* Menghitung waktu loop */
	if (bus_position == 1 && is_loop){
		loop_final = sim_time - distance[temp][bus_position];
		sampst(loop_final - loop_init, 10);
		loop_init = loop_final;
	}
	
	/* Menghitung waktu visit tiap location */
	sampst(sim_time - arrive_time - distance[temp][bus_position], temp + 6);
	arrive_time = sim_time;
	
	printf("Bus tiba di %d jam %0.3f\n", bus_position, sim_time);
	if (list_size[bus_position+3] > 0 && !is_moving) {
		unload();
	} 
	
	if (list_size[bus_position] > 0 && !is_moving) {
		load();
	}
	
}

void bus_move(void) {
	int temp = bus_position; // lokasi keberangkatan bus
	int dest; // lokasi tujuan bus
	is_moving = 1; // menggerakkan bus
	if (bus_position == 3) {
		dest = 1;
	} else if (bus_position == 2){
		dest = 3;
	} else if (bus_position == 1){
		dest = 2;
	}
	
    printf("Bus bergerak dari %d ke %d jam %0.3f\n", temp, dest, sim_time);
	event_schedule(sim_time+distance[temp][dest], EVENT_BUS_ARRIVE);
	printf("Menjadwalkan bus agar tiba jam %0.3f\n", sim_time+distance[temp][dest]);
	
}

void report(void){
	fprintf (outfile, "\n\na.\n");
	for (i = 1; i <= MAX_NUM_LOCATION; i++){
		filest(i);
		fprintf (outfile, "  Average number in location %d queue     %0.3f\n", i, transfer[1]);
		fprintf (outfile, "  Maximum number in location %d queue     %0.3f\n", i, transfer[2]);
	}
	
	fprintf (outfile, "\n\nb.\n");
	for (i = 1; i <= MAX_NUM_LOCATION; i++){
		sampst(0.0, -i);
		fprintf (outfile, "  Average delay in location %d queue     %0.3f\n", i, transfer[1]);
		fprintf (outfile, "  Maximum delay in location %d queue     %0.3f\n", i, transfer[3]);
	}
	
	fprintf (outfile, "\n\nc.\n");
	timest(0.0, -1);
	fprintf (outfile, "  Average number on the bus    %0.3f\n", transfer[1]);
	fprintf (outfile, "  Maximum number on the bus    %0.3f\n", transfer[2]);
	
	fprintf (outfile, "\n\nd.\n");
	for (i = 1; i <= MAX_NUM_LOCATION; i++){
		sampst(0.0, -i-6);
		fprintf (outfile, "  Average time stop in location %d     %0.3f\n", i, transfer[1]);
		fprintf (outfile, "  Maximum time stop in location %d     %0.3f\n", i, transfer[3]);
		fprintf (outfile, "  Minimum time stop in location %d     %0.3f\n", i, transfer[4]);
	}
	
	fprintf (outfile, "\n\ne.\n");
	sampst(0.0, -10);
	fprintf (outfile, "  Average time to make a loop	%0.3f\n", transfer[1]);
	fprintf (outfile, "  Maximum time to make a loop	%0.3f\n", transfer[3]);
	fprintf (outfile, "  Minimum time to make a loop	%0.3f\n", transfer[4]);
	
	fprintf (outfile, "\n\nf.\n");
	for (i = 1; i <= MAX_NUM_LOCATION; i++){
		sampst(0.0, -i-3);
		fprintf (outfile, "  Average time person in system from location %d     %0.3f\n", i, transfer[1]);
		fprintf (outfile, "  Maximum time person in system from location %d     %0.3f\n", i, transfer[3]);
		fprintf (outfile, "  Minimum time person in system from location %d     %0.3f\n", i, transfer[4]);
	}
	
}

int
main ()				/* Main function. */
{  
  /* Open input and output files. */
  infile = fopen ("car_rental.in", "r");
  outfile = fopen ("car_rental.out", "w");

  /* Banyaknya bus, banyaknya location */
  fscanf (infile, "%d %d", &num_bus, &num_location);
  total_passenger = 0;
  loop_init = 0.0;
  is_loop = 0;
  
  /* Mean interarrival rate per hour at location 1, 2, 3 */
  for (i = 1; i <= num_location; ++i) {
	fscanf (infile, "%d", &interarrival_rate[i]);
	mean_interarrival[i] = 3600.0/interarrival_rate[i]; // mean interarrival in seconds
  }
  
  /* Kapasitas per bus, kecepatan dalam mil per hour */
  fscanf (infile, "%d %lg", &num_seats, &speed);
  speed = speed / 3600.0; // replace speed menjadi miles per second
  
  /* Peluang passenger turun di destination 1, 2, dan 3 */
  for (i = 1; i <= num_location; ++i)
    fscanf (infile, "%lg", &prob_distrib_destination[i]);

  /* Jarak dalam mil dari location i ke location j */
  for (i = 1; i <= num_location; ++i) {
	for (j = 1; j <=num_location; ++j) {
	  fscanf (infile, "%lg", &distance[i][j]);
	  distance[i][j] = distance[i][j]/speed; // replace jarak in miles menjadi waktu tempuh in seconds  
    }
  }
  
  /* Batas waktu minimum dan maksimum untuk unload per orang*/
  fscanf (infile, "%d %d", &unload_a, &unload_b);

  /* Batas waktu minimum dan maksimum untuk load per orang*/
  fscanf (infile, "%d %d", &load_a, &load_b);

  /* Waktu bus menunggu adanya load/unload dalam satuan menit */
  fscanf (infile, "%d", &waiting_time);
  waiting_time = waiting_time * 60; // replace menjadi waiting time in seconds
  
  /* Length simulation in hours */
  fscanf (infile, "%d", &length_simulation);
  length_simulation = length_simulation * 3600; // simulation length in seconds
  
  //* END OF READING INPUT */
  
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
    fprintf (outfile, "\n  Location %d %0.3f seconds per interarrival", i, mean_interarrival[i]);
  fprintf (outfile, "\n\nLength of the simulation %d seconds", length_simulation);
  fprintf (outfile, "\n\nBus speed %lg miles per second\n\n", speed);
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
  
  /* Schedule the end of the simulation.  (This is needed for consistency of
     units.) HARUS DISCHEDULE PERTAMA*/
  event_schedule (length_simulation, EVENT_END_SIMULATION);
  
  /* Schedule the arrival of the first person in each location. */
  event_schedule (expon (mean_interarrival[1], STREAM_INTERARRIVAL_1), EVENT_ARRIVAL_1);
  event_schedule (expon (mean_interarrival[2], STREAM_INTERARRIVAL_2), EVENT_ARRIVAL_2);
  event_schedule (expon (mean_interarrival[3], STREAM_INTERARRIVAL_3), EVENT_ARRIVAL_3);

  /* Location awal bus */
  bus_position = 3;

  /* Schedule the first departure of bus from location. */
  event_schedule (sim_time, EVENT_BUS_DEPART);
  
  /* Setting location list to stay ordered by attribute arrival time */
  for (i = 1; i<=num_location; ++i)
  {
	list_rank[i] = 1;  
  }
  
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
	case EVENT_BUS_ARRIVE:
      bus_arrive();
      break;
	case EVENT_BUS_DEPART:
	  bus_move();
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
  printf("Total penumpang %d\n", total_passenger);
  
  // MENCETAK PENUMPANG YANG BERADA DI BUS SAAT SIMULASI BERAKHIR
  i = 1;
  while (list_size[4] >0) {
	list_remove(FIRST,4);
	printf("%d Isi bus destination %0.3f, arrival_time to terminal %0.3f, arrival_location %0.3f\n", i, transfer[2], transfer[1], transfer[3]);
	i++;
  }

  i = 1;
  while (list_size[5] >0) {
	list_remove(FIRST,5);
	printf("%d Isi bus destination %0.3f, arrival_time to terminal %0.3f, arrival_location %0.3f\n", i, transfer[2], transfer[1], transfer[3]);
	i++;
  }

  i = 1;
  while (list_size[6] >0) {
	list_remove(FIRST,6);
	printf("%d Isi bus destination %0.3f, arrival_time to terminal %0.3f, arrival_location %0.3f\n", i, transfer[2], transfer[1], transfer[3]);
	i++;
  }
  
  
  fclose (infile);
  fclose (outfile);

  printf("Simulation done. Check output file.\n");
  
  return 0;
}

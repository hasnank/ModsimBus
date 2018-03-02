
all: car_rental.c car_rental_log.c simlib.c
	gcc -o car_rental.exe car_rental.c simlib.c -lm
	gcc -o car_rental_log.exe car_rental_log.c simlib.c -lm

1: car_rental.c simlib.c
	gcc -o car_rental.exe car_rental.c simlib.c -lm

2: car_rental_log.c simlib.c
	gcc -o car_rental_log.exe car_rental_log.c simlib.c -lm

run:
	./car_rental.exe

clean:
	rm -rf car_rental.o	
	rm -rf car_rental_log.o
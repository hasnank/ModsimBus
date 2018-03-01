
all: car_rental.c car_rental2.c simlib.c
	gcc -o car_rental.exe car_rental.c simlib.c -lm
	gcc -o car_rental2.exe car_rental2.c simlib.c -lm

1: car_rental.c simlib.c
	gcc -o car_rental.exe car_rental.c simlib.c -lm

2: car_rental2.c simlib.c
	gcc -o car_rental2.exe car_rental2.c simlib.c -lm

run:
	clear
	./car_rental.exe

clean:
	rm -rf car_rental.o
	rm -rf car_rental.out
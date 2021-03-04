
CC := gcc

enc : test_bch
	$<


test_bch : bch.o bch.h 

clean :
	rm -rf *.exe *.o


CC := gcc

enc : test_bch_enc 
	$<


test_bch_enc : bch.o bch.h 

clean :
	rm -rf *.exe *.o

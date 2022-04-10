all : main.o

main.o : inline-cache.cpp
	g++ inline-cache.cpp

clean :
	rm *.o *.out
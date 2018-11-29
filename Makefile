all: do

do: main.o nodedetails.o utility.o init.o connection.o chordfunctions.o 
	g++ main.o init.o chordfunctions.o connection.o nodedetails.o utility.o -o chord -lcrypto -lpthread
	
main.o: main.cpp
		g++ -std=c++11 -c main.cpp
		
init.o: init.cpp
		g++ -std=c++11 -c init.cpp

connection.o: connection.cpp
		g++ -std=c++11 -c connection.cpp
		
chordfunctions.o: chordfunctions.cpp
				 g++ -std=c++11 -c chordfunctions.cpp
			 
nodedetails.o: nodedetails.cpp
				   g++ -std=c++11 -c nodedetails.cpp
				   
utility.o: utility.cpp
			   g++ -std=c++11 -c utility.cpp			


	

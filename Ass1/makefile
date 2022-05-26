OBJ = phase1.o phase2.o tester.o

test:$(OBJ)
	g++ $(OBJ) -o test
phase1.o:phase1.cpp
	g++ -c phase1.cpp -o phase1.o
phase2.o:phase2.cpp
	g++ -c phase2.cpp -o phase2.o
tester.o:tester.cpp
	g++ -c tester.cpp -o tester.o

.PHONY:clean

clean:
	rm *.o test out.txt output.txt
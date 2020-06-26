exe: genetic.c othello.c
	gcc genetic.c othello.c -o exe -lm
exe2: genetic02.c othello.c sort01.o
	gcc genetic02.c othello.c sort01.o -o exe2 -lm
othello: othello.c
	gcc othello.c -o othello
test002: test002.c
	gcc test002.c -o test002
sort01.o: sort01.c
	gcc sort01.c -c
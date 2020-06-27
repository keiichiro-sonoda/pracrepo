exe: genetic.c othello.c
	gcc genetic.c othello.c -o exe -lm
exe2: genetic02.c othello.c sort01.o
	gcc genetic02.c othello.c sort01.o -o exe2 -lm
exe2_win: genetic02_win.c othello_win.c sort01_win.o
	gcc genetic02_win.c othello_win.c sort01_win.o -o exe2_win -lm
othello: othello.c
	gcc othello.c -o othello
test002: test002.c
	gcc test002.c -o test002
sort01.o: sort01.c
	gcc sort01.c -c
sort01_win.o: sort01_win.c
	gcc sort01_win.c -c
win002.so: win002.c
	gcc win002.c -o win002.so -shared -fPIC
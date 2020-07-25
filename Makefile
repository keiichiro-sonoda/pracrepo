exe: genetic.c othello.c
	gcc genetic.c othello.c -o exe -lm
exe2: genetic02.c othello.c sort01.o
	gcc genetic02.c othello.c sort01.o -o exe2 -lm
exe3: genetic03.c genetic02.c othello.c sort01.o
	gcc genetic03.c genetic02.c othello.c sort01.o -o exe3 -lm
exe2_win.exe: genetic02_win.c othello_win.c sort01_win.o
	gcc genetic02_win.c othello_win.c sort01_win.o -o exe2_win.exe -lm
exe2_win.so: genetic02_win.c othello_win.c sort01_win.o
	gcc genetic02_win.c othello_win.c sort01_win.o -o exe2_win.so -shared -fPIC -lm
othello: othello.c
	gcc othello.c -o othello
othello_win.exe: othello_win.c
	gcc othello_win.c -o othello_win.exe
test002: test002.c
	gcc test002.c -o test002
sort01: sort01.c
	gcc sort01.c -o sort01
sort01.o: sort01.c
	gcc sort01.c -c
sort01_win.o: sort01_win.c
	gcc sort01_win.c -c
win002.so: win002.c
	gcc win002.c -o win002.so -shared -fPIC
win003.exe: win003.c array.c
	gcc win003.c array.c -o win003
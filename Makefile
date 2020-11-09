exe2: genetic02.c othello.c sort01.o
	gcc genetic02.c othello.c sort01.o -o exe2 -lm
exe3: genetic03.c genetic02.c othello.c sort01.o
	gcc genetic03.c genetic02.c othello.c sort01.o -o exe3 -lm
exe4: genetic04.c genetic02.c othello.c sort01.o
	gcc genetic04.c genetic02.c othello.c sort01.o -o exe4 -lm
exe5: genetic05.c genetic02.c othello.c sort01.o
	gcc genetic05.c genetic02.c othello.c sort01.o -o exe5 -lm
exe6: genetic06.c genetic02.c othello.c sort01.o
	gcc genetic06.c genetic02.c othello.c sort01.o -o exe6 -lm
exe7: genetic07.c genetic02.c othello.c sort01.o
	gcc genetic07.c genetic02.c othello.c sort01.o -o exe7 -lm
exe8: genetic08.c genetic02.c othello.c sort01.o
	gcc genetic08.c genetic02.c othello.c sort01.o -o exe8 -lm
exe2_win.exe: genetic02_win.c genetic02_win.h othello_win.c othello_win.h sort01_win.o
	gcc genetic02_win.c othello_win.c sort01_win.o -o exe2_win.exe -lm
exe2_win.so: genetic02_win.c genetic02_win.h othello_win.c othello_win.h sort01_win.o
	gcc genetic02_win.c othello_win.c sort01_win.o -o exe2_win.so -shared -fPIC -lm
othello: othello.c
	gcc othello.c -o othello
othello_win.exe: othello_win.c
	gcc othello_win.c -o othello_win.exe
test: test.c genetic02.c genetic02.h othello.c sort01.o sort01.h
	gcc test.c genetic02.c othello.c sort01.o -o test -lm
test_win.exe: test_win.c genetic02_win.c genetic02_win.h othello_win.c othello_win.h sort01_win.o sort01_win.h
	gcc test_win.c genetic02_win.c othello_win.c sort01_win.o -o test_win.exe
sort01.o: sort01.c sort01.h
	gcc sort01.c -c
sort01_win.o: sort01_win.c sort01_win.h
	gcc sort01_win.c -c
win002.so: win002.c
	gcc win002.c -o win002.so -shared -fPIC
win003: win003.c array.c
	gcc win003.c array.c -o win003
win004: win004.c array.c
	gcc win004.c array.c -o win004 -lm
win005: win005.c
	gcc win005.c -o win005
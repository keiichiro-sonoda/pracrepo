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
exe9: genetic09.c genetic02.o othello.o sort01.o
	gcc genetic09.c genetic02.o othello.o sort01.o -o exe9 -lm -Wall
exe10: genetic10.c genetic01.o othello.o sort01.o
	gcc genetic10.c genetic01.o othello.o sort01.o -o exe10 -lm -Wall
exe2_win.exe: genetic02_win.c genetic02_win.h othello_win.c othello_win.h sort01_win.o
	gcc genetic02_win.c othello_win.c sort01_win.o -o exe2_win.exe -lm
share01.so: genetic01_win.o othello_win.o sort01_win.o
	gcc genetic01_win.o othello_win.o sort01_win.o -o share01.so -shared -fPIC -lm
share02.so: genetic02_win.o  othello_win.o  sort01_win.o
	gcc genetic02_win.o othello_win.o sort01_win.o -o share02.so -shared -fPIC -lm
share02_ubu.so: genetic02_share.o genetic02.o othello.o sort01.o
	gcc genetic02_share.o genetic02.o othello.o sort01.o -o share02_ubu.so -shared -fPIC -Wall -lm
othello_win.exe: othello_win.c othello_win.h
	gcc othello_win.c -o othello_win.exe -fPIC -Wall
test01: test01.c genetic01.o othello.o sort01.o
	gcc test01.c genetic01.o othello.o sort01.o -o test01 -lm -Wall
test02: test02.c genetic02.o othello.o sort01.o
	gcc test02.c genetic02.o othello.o sort01.o -o test02 -lm -Wall
compare: compare.c genetic01.o genetic02.o othello.o sort01.o
	gcc compare.c genetic01.o genetic02.o othello.o sort01.o -o compare -lm
test_win.exe: test_win.c genetic02_win.o othello_win.o sort01_win.o
	gcc test_win.c genetic02_win.o othello_win.o sort01_win.o -o test_win.exe
othello.o: othello.c othello.h
	gcc othello.c -c -fPIC -Wall
sort01.o: sort01.c sort01.h othello.h
	gcc sort01.c -c -Wall
genetic00.o: genetic00.c genetic00.h othello.h
	gcc genetic00.c -c -Wall
genetic01.o: genetic01.c genetic01.h othello.h sort01.h
	gcc genetic01.c -c -Wall
genetic02.o: genetic02.c genetic02.h othello.h
	gcc genetic02.c -c -Wall -fPIC
genetic02_share.o: genetic02_share.c genetic02_share.h
	gcc genetic02_share.c -c -Wall -fPIC
othello_win.o: othello_win.c othello_win.h
	gcc othello_win.c -c -fPIC -Wall
genetic01_win.o: genetic01_win.c genetic01_win.h
	gcc genetic01_win.c -c -fPIC -Wall
genetic02_win.o: genetic02_win.c genetic02_win.h othello_win.h
	gcc genetic02_win.c -c -fPIC -Wall
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
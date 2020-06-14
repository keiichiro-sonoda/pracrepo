exe: genetic.c othello.c
	gcc genetic.c othello.c -o exe -lm
othello: othello.c
	gcc othello.c -o othello
test002: test002.c
	gcc test002.c -o test002
all: flame_cp

flame_cp: my_copy.c 
	gcc -Wall -o flame_cp my_copy.c

clean:
	$(RM) flame_cp
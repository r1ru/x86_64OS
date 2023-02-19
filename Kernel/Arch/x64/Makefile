CC = gcc 
CFLAGS = -Wall -g -masm=intel -fno-pie -fno-stack-protector

kbuiltin.o : kbuiltin.c 
	$(CC) $(CFLAGS) -c -IInclude -o $@ $<

clean:
	rm -f *.o 

.PHONY: clean
CFLAGS = -g -Wall -pedantic -Wextra -Wconversion -std=gnu11 #HAY QUE VOLVER A AGREGAR -Werror!!!
#La opcion -g hace que el compilador le agregue al binario informacion\
#extra que se va a utilizar para debugger, por ejemplo con GDB

#client : client_w_FIFO.o
#	gcc $(CFLAGS) client_w_FIFO.o

#server : server.o
#	gcc $(CFLAGS) server.o


get_num.o : lib/get_num.c lib/get_num.h
error_functions.o : lib/error_functions.c lib/error_functions.h lib/ename.c.inc lib/tlpi_hdr.h
server.o : ./server.c ./fifo_seqnum.h 
client_w_FIFO.o :  ./client_w_FIFO.c ./fifo_seqnum.h ./error_functions.o ./get_num.o 

	
.PHONY: clean
clean :
	rm -f *.o server client
#	ls | grep -v "\." | xargs rm #to remove all executables
# 	WARNING: The last command also removes the Makefile

.PHONY: celan
celan :
	rm -f *.o server client
#	ls | grep -v "\." | xargs rm #to remove all executables
# 	WARNING: The last command also removes the Makefile

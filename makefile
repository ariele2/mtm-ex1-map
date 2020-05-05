CC = gcc
OBJS = main.o mapIdStruct.o keyValue.o election.o map.o mapIdList.o  
EXEC = election
DEBUG_FLAG = -DNDEBUG
COMP_FLAG = -std=c99 -Wall -pedantic-errors -Werror $(DEBUG_FLAG)

$(EXEC):	$(OBJS)
	$(CC) $(DEBUG_FLAG) $(OBJS) -o $@

main.o:	main.c mapIdStruct.h mapIdList.h keyValue.h map.h election.h 
	$(CC) -c $(COMP_FLAG) $*.c
election.o:	election.c mapIdStruct.h mapIdList.h keyValue.h map.h election.h
	$(CC) -c $(COMP_FLAG) $*.c
map.o:	map.c map.h keyValue.h 
	$(CC) -c $(COMP_FLAG) $*.c
keyValue.o:	keyValue.c keyValue.h
	$(CC) -c $(COMP_FLAG) $*.c
mapIdList.o:	mapIdList.c map.h mapIdList.h mapIdStruct.h keyValue.h
	$(CC) -c $(COMP_FLAG) $*.c
mapIdStruct.o:	mapIdStruct.c mapIdStruct.h map.h keyValue.h
	$(CC) -c $(COMP_FLAG) $*.c

clean:
	rm -f $(OBJS) $(EXEC)
	

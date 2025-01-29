CC=g++ -std=c++11
CFLAGS=
DEPS = Client.h GameInstance.h Server.h Thread_Lobby.h Message.h

ODIR=obj
LIBS=-lpthread

_OBJ = Client.o Main.o GameInstance.o Server.o Message.o Thread_Output.o Thread_Lobby.o PlayerInfo.o SocketHelper.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_OBJ_CLIENT = Client.o Main_Client.o GameInstance.o Server.o Message.o Thread_Output.o
OBJ_CLIENT = $(patsubst %,$(ODIR)/%,$(_OBJ))



$(ODIR)/%.o : %.cc $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

mwordleserver: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

mwordleclient: $(OBJ_CLIENT)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)


.PHONY: clean

clean:
	rm -f *.o *~ core mwordleserver mwordleclient
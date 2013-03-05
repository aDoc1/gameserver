CC=gcc
FLAGS=-I./src/
TARGETS=MyServerMain
LIBS=-lm

main: $(TARGETS)


lib/%.o: src/%.c
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -f $(TARGETS) lib/*.o

%:lib/MyServer.o lib/%.o
	$(CC) -o $@ $^ $(LIBS)

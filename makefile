CC=g++
EXE=app
DEPS=httpserver.cpp
LINKS=-lboost_filesystem -lboost_system -pthread

$(EXE): $(DEPS)
	$(CC) $^ -o $@ $(LINKS)

.PHONY: clean

clean:
	rm -f $(EXE)

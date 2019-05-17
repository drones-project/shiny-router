CC = g++
EXE = app

MAIN = router.cpp
PATHFINDER_DIR = pathfinder
PATHFINDER_DEPS = $(wildcard $(PATHFINDER_DIR)/*.cpp)

LINKS=-lboost_filesystem -lboost_system -pthread

$(EXE): $(MAIN) $(PATHFINDER_DEPS)
	$(CC) $^ -o $@ $(LINKS)

.PHONY: clean

clean:
	rm -f $(EXE)

CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

EXEC = dalembert

SRC = main.cpp jeu.cpp affichage.cpp score.cpp
OBJ = $(SRC:.cpp=.o)

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o $(EXEC)

reset-score:
	rm -f score.txt
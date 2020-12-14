CXX = g++
CXXSTD = c++20
CXXLFLAGS = -g -std=$(CXXSTD) -static
CXXFLAGS = -g -std=$(CXXSTD)
EXEC = dist/masm++
OBJECTS = cli.o libmasm++.o

all: $(EXEC)

$(EXEC): $(OBJECTS)
	@$(CXX) $(CXXLFLAGS) $^ -o $@

%.o: src/%.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	@./$(EXEC)

crun:
	@$(MAKE) clean
	@$(MAKE) run

crunc:
	@$(MAKE) crun
	@$(MAKE) clean

clean:
	@rm -f *.o $(EXEC)

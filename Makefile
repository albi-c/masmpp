CXX = g++
CXXSTD = c++20
CXXLFLAGS = -g -std=$(CXXSTD) -static
CXXFLAGS = -g -std=$(CXXSTD)
EXEC = masm++
OBJECTS = cli.o libmasm++.o

all: $(EXEC)

release: $(EXEC)
	@tar -czvf masm++-linux.tar.gz $(EXEC)

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
	@rm -f *.o *.tar.gz $(EXEC)

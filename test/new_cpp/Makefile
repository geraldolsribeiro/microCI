%_c: %.c
	$(CC) -Wall $< -o $@

%_cpp: %.cpp
	$(CXX) -Wall $< -o $@

all:
	../../bin/microCI | bash

clean:
	$(RM) prog_c prog_cpp

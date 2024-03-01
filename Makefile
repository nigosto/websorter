targets := server.exe client.exe
CXXFlags = -std=c++2a -Wall -Werror -Wunused

all: $(targets)

%.exe: %  
	clang++ $(CXXFlags) $</*.cpp -o $@

clean:
	rm -f *.exe
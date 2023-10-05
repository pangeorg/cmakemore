$(info $(SHELL))
all:
	zig cc ./src/main.c -o ./bin/cmakemore.exe -Wall -Wextra -g

clean:
	cd bin
	del /s *
	cd ../obj
	del /s *
	cd ..

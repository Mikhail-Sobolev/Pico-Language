

S=$(wildcard src/*.cc)
O=$(patsubst src/%.cc, tmp/%.o, $(S))

F=-Iinc/ -w -c 
F_DB=-g

.PHONY: picoc clean test1 

picoc: $(O)
	g++ $^ -o picoc

debug: 
	g++ -Iinc/ -w $(S) -o picoc_db $(F_DB)
	gdb ./picoc_db

tmp/%.o: $(S)
	g++ $(F) $(patsubst tmp/%.o, src/%.cc, $@) -o $@

clean:
	rm $(O)


test1: picoc 
	./picoc tests/test1.pico
	

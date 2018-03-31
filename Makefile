TARGET=tarea
C_SRCFILES=parser.cpp lexer.cpp ast.cpp main.cpp
OBJ_FILES=${C_SRCFILES:.cpp=.o}
.PHONY: clean run

$(TARGET): $(OBJ_FILES)
	g++ --std=c++11 -o $@ $^

lexer.cpp: lexer.l
	flex -o $@ $^

parser.cpp: parser.y
	bison -rall --defines=tokens.h -o $@ $<

%.o: %.cpp tokens.h
	g++ --std=c++11 -c -o $@ $<

run: $(TARGET)
	./$(TARGET) input.tjl

run2: $(TARGET)
	./$(TARGET) ./TinyJuliaExamples/Arithmetic.jl
	./$(TARGET) ./TinyJuliaExamples/Arrays.jl
	./$(TARGET) ./TinyJuliaExamples/Bitwise.jl
	./$(TARGET) ./TinyJuliaExamples/block_comments.jl
	./$(TARGET) ./TinyJuliaExamples/BubbleSort.jl
	./$(TARGET) ./TinyJuliaExamples/comments.jl
	./$(TARGET) ./TinyJuliaExamples/Factorial.jl
	./$(TARGET) ./TinyJuliaExamples/functions.jl
	./$(TARGET) ./TinyJuliaExamples/helloworld.jl
	./$(TARGET) ./TinyJuliaExamples/if.jl
	./$(TARGET) ./TinyJuliaExamples/Relational.jl
	./$(TARGET) ./TinyJuliaExamples/while.jl

clean:
	rm -f parser.cpp lexer.cpp
	rm -f tokens.h
	rm -f $(TARGET)
	rm -f $(OBJ_FILES)
	rm -f parser.output
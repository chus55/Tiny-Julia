TARGET=project
EXPR_PARSER_SRC=parser.cpp
EXPR_LEXER_SRC=lexer.cpp
C_SRCFILES=$(EXPR_PARSER_SRC) $(EXPR_LEXER_SRC) ast.cpp main.cpp
OBJ_FILES=${C_SRCFILES:.cpp=.o}
.PHONY: clean

$(TARGET): $(OBJ_FILES)
	g++ -g -o $@ $(OBJ_FILES)

$(EXPR_LEXER_SRC): lexer.l
	flex -o $@ $^

$(EXPR_PARSER_SRC): parser.y
	bison -rall --defines=tokens.h -o $@ $<

%.o: %.cpp
	g++ -std=c++11 -g -c -o $@ $<

run: $(TARGET)
	./$(TARGET) input.tjl
	nasm -felf result.S
	gcc -o result -m32 result.o -lm
	./result

run2: $(TARGET)
	clear
	./$(TARGET) ./TinyJuliaExamples/Arithmetic.jl
	nasm -felf result.S
	gcc -o result -m32 result.o -lm
	./result
	./$(TARGET) ./TinyJuliaExamples/Arrays.jl
	nasm -felf result.S
	gcc -o result -m32 result.o -lm
	./result
	./$(TARGET) ./TinyJuliaExamples/Bitwise.jl
	nasm -felf result.S
	gcc -o result -m32 result.o -lm
	./result
	./$(TARGET) ./TinyJuliaExamples/BubbleSort.jl
	nasm -felf result.S
	gcc -o result -m32 result.o -lm
	./result
	./$(TARGET) ./TinyJuliaExamples/comments.jl
	nasm -felf result.S
	gcc -o result -m32 result.o -lm
	./result
	./$(TARGET) ./TinyJuliaExamples/Factorial.jl
	nasm -felf result.S
	gcc -o result -m32 result.o -lm
	./result
	./$(TARGET) ./TinyJuliaExamples/functions.jl
	nasm -felf result.S
	gcc -o result -m32 result.o -lm
	./result
	./$(TARGET) ./TinyJuliaExamples/helloworld.jl
	nasm -felf result.S
	gcc -o result -m32 result.o -lm
	./result
	./$(TARGET) ./TinyJuliaExamples/if.jl
	nasm -felf result.S
	gcc -o result -m32 result.o -lm
	./result
	./$(TARGET) ./TinyJuliaExamples/printFormat.jl
	nasm -felf result.S
	gcc -o result -m32 result.o -lm
	./result
	./$(TARGET) ./TinyJuliaExamples/QuickSort.jl
	nasm -felf result.S
	gcc -o result -m32 result.o -lm
	./result
	./$(TARGET) ./TinyJuliaExamples/recursion.jl
	nasm -felf result.S
	gcc -o result -m32 result.o -lm
	./result
	./$(TARGET) ./TinyJuliaExamples/Relational.jl
	nasm -felf result.S
	gcc -o result -m32 result.o -lm
	./result
	./$(TARGET) ./TinyJuliaExamples/several.jl
	nasm -felf result.S
	gcc -o result -m32 result.o -lm
	./result
	./$(TARGET) ./TinyJuliaExamples/while_break.jl
	nasm -felf result.S
	gcc -o result -m32 result.o -lm
	./result
	./$(TARGET) ./TinyJuliaExamples/while.jl
	nasm -felf result.S
	gcc -o result -m32 result.o -lm
	./result


run3: $(TARGET)
	nasm -felf result.S
	gcc -o result -m32 result.o -lm
	./result

clean:
	rm -f $(EXPR_PARSER_SRC) $(EXPR_LEXER_SRC)
	rm -f tokens.h
	rm -f $(TARGET)
	rm -f $(OBJ_FILES)
	rm -f parser.output
	rm -f result result.o result.S
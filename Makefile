all: mkdirectory mksupporddirectory build_convert build_front build_back run

FLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations 		\
		-Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion	\
		-Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers		\
		-Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel  	\
		-Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE

FRONT_DIR = frontend


run: obj/main.o obj/generals.o obj/log_errors.o assembler/assembler.exe
	g++ obj/main.o obj/generals.o obj/log_errors.o -o run

build_front: obj/frontend_main.o obj/tree.o obj/generals.o obj/log_errors.o obj/process_text.o obj/array.o 				\
			 obj/frontend.o obj/AST_draw_tree.o obj/AST_tree.o 	obj/lexer.o	obj/reader.o obj/ast_reader.o						
				

			g++ obj/frontend_main.o obj/tree.o obj/generals.o obj/log_errors.o obj/process_text.o obj/array.o			\
				obj/frontend.o obj/AST_tree.o obj/AST_draw_tree.o obj/lexer.o  obj/reader.o obj/ast_reader.o -o frontend						


build_back: obj/backend_main.o obj/tree.o obj/generals.o obj/log_errors.o obj/process_text.o obj/array.o	 			\
			obj/backend.o obj/AST_draw_tree.o obj/AST_tree.o obj/lexer.o  obj/array.o obj/name_table.o obj/ast_reader.o
				

			g++ obj/backend_main.o obj/tree.o obj/generals.o obj/log_errors.o obj/process_text.o obj/ast_reader.o		\
				obj/backend.o obj/AST_tree.o obj/AST_draw_tree.o obj/lexer.o obj/array.o obj/name_table.o 	-o backend


build_convert: obj/convert_main.o obj/tree.o obj/generals.o obj/log_errors.o obj/process_text.o obj/array.o	 			\
			obj/convert.o obj/AST_draw_tree.o obj/AST_tree.o obj/lexer.o  obj/array.o obj/name_table.o obj/ast_reader.o
				

			g++ obj/convert_main.o obj/tree.o obj/generals.o obj/log_errors.o obj/process_text.o obj/ast_reader.o		\
				obj/convert.o obj/AST_tree.o obj/AST_draw_tree.o obj/lexer.o obj/array.o obj/name_table.o 	-o convert


obj/main.o: main.cpp
	g++ main.cpp -c -o obj/main.o $(FLAGS)


obj/frontend_main.o: frontend/frontend_main.cpp
	g++ frontend/frontend_main.cpp -c -o obj/frontend_main.o $(FLAGS)


obj/frontend.o: frontend/frontend.cpp config/language_config.h	\
				frontend/frontend.h 			
	g++ frontend/frontend.cpp -c -o obj/frontend.o $(FLAGS)


obj/reader.o: frontend/reader/reader.cpp frontend/reader/reader.cpp		
	g++ frontend/reader/reader.cpp -c -o obj/reader.o $(FLAGS)




obj/AST_tree.o: AST_tree/AST_tree.cpp config/language_config.h		\
				AST_tree/AST_tree.h
	g++ AST_tree/AST_tree.cpp -c -o obj/AST_tree.o $(FLAGS)


obj/AST_draw_tree.o:  AST_tree/AST_tree.cpp AST_tree/AST_draw_tree.cpp config/language_config.h	\
				  	  AST_tree/AST_tree.h   AST_tree/AST_draw_tree.h	 
	g++ AST_tree/AST_draw_tree.cpp -c -o obj/AST_draw_tree.o $(FLAGS)

obj/ast_reader.o:  AST_tree/ast_reader/ast_reader.cpp AST_tree/ast_reader/ast_reader.h config/language_config.h	\
				  	  AST_tree/AST_tree.h   AST_tree/AST_draw_tree.h	 
	g++ AST_tree/ast_reader/ast_reader.cpp -c -o obj/ast_reader.o $(FLAGS)





obj/backend_main.o: backend/backend_main.cpp
	g++ backend/backend_main.cpp -c -o obj/backend_main.o $(FLAGS)


obj/backend.o: backend/backend.cpp config/language_config.h	\
				backend/backend.h 			
	g++ backend/backend.cpp -c -o obj/backend.o $(FLAGS)





obj/convert_main.o: convert/convert_main.cpp
	g++ convert/convert_main.cpp -c -o obj/convert_main.o $(FLAGS)


obj/convert.o: convert/convert.cpp config/language_config.h	\
				convert/convert.h 			
	g++ convert/convert.cpp -c -o obj/convert.o $(FLAGS)




obj/tree.o: src/tree/tree.cpp src/tree/tree.h 
	g++ src/tree/tree.cpp -c -o obj/tree.o $(FLAGS)

obj/array.o: src/array/array.cpp src/array/array.h 
	g++ src/array/array.cpp -c -o obj/array.o $(FLAGS)

obj/log_errors.o: src/log_info/log_errors.h src/log_info/log_errors.cpp
	g++ src/log_info/log_errors.cpp -c -o obj/log_errors.o $(FLAGS)

obj/generals.o: src/Generals_func/generals.cpp
	g++ src/Generals_func/generals.cpp -c -o obj/generals.o $(FLAGS)

obj/process_text.o: src/process_text/process_text.cpp src/process_text/process_text.h
	g++ src/process_text/process_text.cpp -c -o obj/process_text.o $(FLAGS)

obj/lexer.o: src/lexer/lexer.cpp src/lexer/lexer.cpp	
	g++ src/lexer/lexer.cpp -c -o obj/lexer.o $(FLAGS)

obj/name_table.o: src/name_table/name_table.cpp src/name_table/name_table.h
	g++ src/name_table/name_table.cpp -c -o obj/name_table.o $(FLAGS)

.PHONY: cleanup mkdirectory

mkdirectory:
	mkdir -p obj;

mksupporddirectory:
	mkdir -p temp;
	mkdir -p temp/ast_format;
	mkdir -p temp/asm_format;
	mkdir -p temp/source;

cleanup:
	rm obj/*.o
all: mkdirectory build_front build_back

FLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations 		\
		-Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion	\
		-Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers		\
		-Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel  	\
		-Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE

FRONT_DIR = frontend



build_front: obj/frontend_main.o obj/tree.o obj/generals.o obj/log_errors.o obj/process_text.o obj/array.o 	\
			 obj/frontend.o obj/AST_draw_tree.o obj/AST_tree.o 	obj/lexer.o	obj/reader.o						
				

			g++ obj/frontend_main.o obj/tree.o obj/generals.o obj/log_errors.o obj/process_text.o obj/array.o		\
				obj/frontend.o obj/AST_tree.o obj/AST_draw_tree.o obj/lexer.o  obj/reader.o -o frontend						


build_back: obj/backend_main.o obj/tree.o obj/generals.o obj/log_errors.o obj/process_text.o obj/array.o	 	\
			obj/backend.o obj/AST_draw_tree.o obj/AST_tree.o obj/lexer.o  obj/array.o obj/name_table.o
				

			g++ obj/backend_main.o obj/tree.o obj/generals.o obj/log_errors.o obj/process_text.o 				\
				obj/backend.o obj/AST_tree.o obj/AST_draw_tree.o obj/lexer.o obj/array.o obj/name_table.o -o backend



obj/frontend_main.o: frontend/frontend_main.cpp
	g++ frontend/frontend_main.cpp -c -o obj/frontend_main.o $(FLAGS)


obj/frontend.o: frontend/frontend.cpp config/language_config.h	\
				frontend/frontend.h 			
	g++ frontend/frontend.cpp -c -o obj/frontend.o $(FLAGS)



obj/AST_tree.o: config/AST_tree/AST_tree.cpp config/language_config.h		\
				config/AST_tree/AST_tree.h
	g++ config/AST_tree/AST_tree.cpp -c -o obj/AST_tree.o $(FLAGS)


obj/AST_draw_tree.o:  config/AST_tree/AST_tree.cpp config/AST_tree/AST_draw_tree.cpp config/language_config.h	\
				  	  config/AST_tree/AST_tree.h   config/AST_tree/AST_draw_tree.h	 
	g++ config/AST_tree/AST_draw_tree.cpp -c -o obj/AST_draw_tree.o $(FLAGS)


obj/reader.o: frontend/reader/reader.cpp frontend/reader/reader.cpp		
	g++ frontend/reader/reader.cpp -c -o obj/reader.o $(FLAGS)






obj/backend_main.o: backend/backend_main.cpp
	g++ backend/backend_main.cpp -c -o obj/backend_main.o $(FLAGS)


obj/backend.o: backend/backend.cpp config/language_config.h	\
				backend/backend.h 			
	g++ backend/backend.cpp -c -o obj/backend.o $(FLAGS)






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

cleanup:
	rm obj/*.o
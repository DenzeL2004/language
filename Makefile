all: mkdirectory build_front

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


obj/frontend_main.o: $(FRONT_DIR)/frontend_main.cpp
	g++ $(FRONT_DIR)/frontend_main.cpp -c -o obj/frontend_main.o $(FLAGS)


obj/frontend.o: $(FRONT_DIR)/frontend.cpp config/language_config.h	\
				$(FRONT_DIR)/frontend.h 			
	g++ $(FRONT_DIR)/frontend.cpp -c -o obj/frontend.o $(FLAGS)



obj/AST_tree.o: config/AST_tree/AST_tree.cpp config/language_config.h		\
				config/AST_tree/AST_tree.h
				
	g++ config/AST_tree/AST_tree.cpp -c -o obj/AST_tree.o $(FLAGS)


obj/AST_draw_tree.o:  config/AST_tree/AST_tree.cpp config/AST_tree/AST_draw_tree.cpp config/language_config.h	\
				  	  config/AST_tree/AST_tree.h   config/AST_tree/AST_draw_tree.h	 
					   	
	g++ config/AST_tree/AST_draw_tree.cpp -c -o obj/AST_draw_tree.o $(FLAGS)


obj/lexer.o: $(FRONT_DIR)/lexer/lexer.cpp $(FRONT_DIR)/lexer/lexer.cpp	
	
	g++ $(FRONT_DIR)/lexer/lexer.cpp -c -o obj/lexer.o $(FLAGS)


obj/reader.o: $(FRONT_DIR)/reader/reader.cpp $(FRONT_DIR)/reader/reader.cpp	
	
	g++ $(FRONT_DIR)/reader/reader.cpp -c -o obj/reader.o $(FLAGS)






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

.PHONY: cleanup mkdirectory

mkdirectory:
	mkdir -p obj;

cleanup:
	rm obj/*.o
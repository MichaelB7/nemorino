ifeq ($(OS),Windows_NT)
	EXE := nemorino_gcc.exe
else
	EXE := nemorino
endif

FILES = bbEndings.cpp board.cpp book.cpp evaluation.cpp hashtables.cpp Material.cpp \
    Nemorino.cpp position.cpp search.cpp settings.cpp test.cpp timemanager.cpp \
    uci.cpp utils.cpp tbprobe.cpp

FLAGS = -O3 -Wmain -std=c++11 -flto -pthread -DNDEBUG

FLAGS_BMI2 = -O3 -Wmain -std=c++11 -flto -pthread -DNDEBUG -DUSE_PEXT -march=native

FLAGS_CCC = -O3 -Wmain -std=c++11 -pthread -DNDEBUG -march=native


make: $(FILES)
	g++ $(FLAGS) $(FILES) -o $(EXE)

bmi2: $(FILES)
	g++ $(FLAGS_BMI2) $(FILES) -o $(EXE)

ccc: $(FILES)
	g++ $(FLAGS_CCC) $(FILES) -o $(EXE)

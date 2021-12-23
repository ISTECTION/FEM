SRC              := src
INCLUDE          := include
OUTPUT           := output
CXX              := g++
CXXFLAGS         := -std=c++20 -Wall

CXXFLAGS_DEBUG   := # -g
CXXFLAGS_RELEASE := -O2

RM := del /q /f

debug:   remove_log compile_debug   running
release: remove_log compile_release running

running:
	$(OUTPUT)/FEM.exe --input file/study-nodes-9 --debug

compile_debug:
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_DEBUG) $(SRC)/main.cpp -o $(OUTPUT)/FEM.exe -I$(INCLUDE)

compile_release:
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_RELEASE) $(SRC)/main.cpp -o $(OUTPUT)/FEM.exe -I$(INCLUDE)

clean:
	$(RM) $(OUTPUT)\*.exe

remove_log:
	$(RM) log\*.log
SRC              := src
INCLUDE          := include
OUTPUT           := output
CXX              := g++
CXXFLAGS         := -std=c++20
CXXFLAGS_RELEASE := -O2

RM := del /q /f

debug: clean remove_log compile running

release: clean remove_log compile_release running

running:
	$(OUTPUT)/main.exe file/nodes-6

compile:
	$(CXX) $(CXXFLAGS) $(SRC)/main.cpp -o $(OUTPUT)/main.exe -I$(INCLUDE)

compile_release:
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_RELEASE) $(SRC)/main.cpp -o $(OUTPUT)/main.exe -I$(INCLUDE)

clean:
	$(RM) $(OUTPUT)\*.exe

remove_log:
	$(RM) log\*.log
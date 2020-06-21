run:
	g++ -std=c++17 ./*.cpp ./*/*/*.cpp -o ./build/app && clear && ./build/app

compile:
	mkdir -p ./build
	g++ -std=c++17 ./*.cpp ./*/*/*.cpp -o ./build/app

trace:
	g++ -g -std=c++17 ./*.cpp ./*/*/*.cpp -o ./build/app
	strace ./build/app

mem:
	g++ -g -std=c++17 ./*.cpp ./*/*/*.cpp -o ./build/app
	valgrind --tool=massif ./build/app
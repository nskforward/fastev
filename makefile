run:
	g++ -g -std=c++17 ./*.cpp ./*/*/*.cpp -o ./build/app && ./build/app

compile:
	g++ -std=c++17 ./*.cpp ./*/*.cpp -o ./build/app

trace:
	g++ -g -std=c++17 ./*.cpp ./*/*.cpp -o ./build/app
	strace ./build/app

mem:
	g++ -g -std=c++17 ./*.cpp ./*/*.cpp -o ./build/app
	valgrind --tool=massif ./build/app
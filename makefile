test:
	g++ -std=c++17 ./tests/*.cpp -o ./build/test && ./build/test

run:
	g++ -g -std=c++17 ./src/*.cpp ./src/*/*/*.cpp -o ./build/app_server && clear && ./build/app_server

compile:
	mkdir -p ./build
	g++ -std=c++17 ./src/*.cpp ./src/*/*/*.cpp -o ./build/app_server

trace:
	g++ -g -std=c++17 ./*.cpp ./*/*/*.cpp -o ./build/app
	strace ./build/app

mem:
	g++ -g -std=c++17 ./*.cpp ./*/*/*.cpp -o ./build/app
	valgrind --tool=massif ./build/app
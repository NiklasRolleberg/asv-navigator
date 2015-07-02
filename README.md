# asv-navigator
compilera:
	//path to boost header       //path to boost libs
g++ -I /home/niklas/lib/include/ -L /home/niklas/lib/lib/ *.cpp -std=c++0x -pthread -lboost_system -Wl,-rpath,/home/niklas/lib/lib
all: NO_GUI

data.o: data.cpp
	g++ -c `wx-config --cxxflags --libs` data.cpp -pthread -std=c++0x
element.o: element.cpp
	g++ -c element.cpp -pthread -std=c++0x
main.o: main.cpp
	g++ -c `wx-config --cxxflags --libs` main.cpp -pthread -std=c++0x
mission.o: mission.cpp
	g++ -c `wx-config --cxxflags --libs` mission.cpp -pthread -std=c++0x
navigator.o: navigator.cpp
	g++ -c `wx-config --cxxflags --libs` navigator.cpp -pthread -std=c++0x
polygon.o_gui: polygon.cpp
	g++ -c `wx-config --cxxflags --libs` polygon.cpp -pthread -std=c++0x -D SHOW_GUI=true
polygon.o: polygon.cpp
		g++ -c `wx-config --cxxflags --libs` polygon.cpp -pthread -std=c++0x -D SHOW_GUI=false
segment.o: segment.cpp
	g++ -c segment.cpp -pthread -std=c++0x
singlebeamscanner.o: singlebeamscanner.cpp
	g++ -c `wx-config --cxxflags --libs` singlebeamscanner.cpp -pthread -std=c++0x
task.o: task.cpp
	g++ -c `wx-config --cxxflags --libs` task.cpp -pthread -std=c++0x
transmitter.o: transmitter.cpp
	g++ -c transmitter.cpp -pthread -std=c++0x
view.o: view.cpp
	g++ -c view.cpp `wx-config --cxxflags --libs` -pthread -std=c++0x


NO_GUI: data.o element.o main.o mission.o navigator.o polygon.o segment.o singlebeamscanner.o task.o transmitter.o view.o
	g++ data.o element.o main.o mission.o navigator.o polygon.o segment.o singlebeamscanner.o task.o transmitter.o view.o -o asv-navigator.out `wx-config --cxxflags --libs` -std=c++0x -pthread

GUI: data.o element.o main.o mission.o navigator.o polygon.o_gui segment.o singlebeamscanner.o task.o transmitter.o view.o
		g++ data.o element.o main.o mission.o navigator.o polygon.o segment.o singlebeamscanner.o task.o transmitter.o view.o -o asv-navigator.out `wx-config --cxxflags --libs` -std=c++0x -pthread


clean:
	rm *o

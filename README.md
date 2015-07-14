# asv-navigator

compile: g++ *.cpp -std=c++0x -pthread

stty options:
speed 9600 baud; rows 0; columns 0; line = 0;
intr = ^C; quit = ^\; erase = ^?; kill = ^U; eof = ^D; eol = <undef>;
eol2 = <undef>; swtch = <undef>; start = ^Q; stop = ^S; susp = ^Z; rprnt = ^R;
werase = ^W; lnext = ^V; flush = ^O; min = 0; time = 0;
-parenb -parodd cs8 hupcl -cstopb cread clocal -crtscts
-ignbrk -brkint -ignpar -parmrk -inpck -istrip -inlcr -igncr icrnl ixon -ixoff
-iuclc -ixany -imaxbel -iutf8
opost -olcuc -ocrnl onlcr -onocr -onlret -ofill -ofdel nl0 cr0 tab0 bs0 vt0 ff0
isig icanon iexten -echo echoe echok -echonl -noflsh -xcase -tostop -echoprt
echoctl echoke

-igncr kanske ska vara på..



compilera med boost:
	//path to boost header       //path to boost libs
g++ -I /home/niklas/lib/include/ -L /home/niklas/lib/lib/ *.cpp -std=c++0x -pthread -lboost_system -Wl,-rpath,/home/niklas/lib/lib
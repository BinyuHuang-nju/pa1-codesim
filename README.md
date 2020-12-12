#codesim

## Requirements
	- c++ version 11
	- cmake version >= 3.10

## Build
	- $ cd /[your path to codesim]/build
	- $ make

## Run
	- $ cd /[your path to codesim]/build #need not do if you are now in this path
	- $ ./codesim [-h|--help] [-v|--verbose] code1.cpp code2.cpp
	
## Help
	If you meets problems when building(actually impossible happens), just execute **$ g++ -g codesim.cpp -o codesim** and use the executable file codesim to run. ^_^

## Reference 
 	The experiment use the method of **winnowing**, which comes from <<Winnowing: Local Algorithms for Document Fingerprinting>>.

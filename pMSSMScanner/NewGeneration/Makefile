all:
	#g++ SuggestAPoint.C -o SuggestAPoint.exec `root-config --glibs --cflags` -g
	g++ src/* -o SuggestAPoint.exec -Iinclude/ `root-config --glibs --cflags` -g
	g++ IsPointAcceptable.C src/StorageLibrary.cc -Iinclude/ -o IsPointAcceptable.exec `root-config --glibs --cflags` -g
	g++ StoreThisPoint.C -Iinclude/ -o StoreThisPoint.exec `root-config --glibs --cflags` -g
	g++ LogToRoot.C -o LogToRoot.exec `root-config --glibs --cflags` -g
	g++ SLHA2StartingPoint.C -Iinclude/ -o SLHA2StartingPoint.exec `root-config --glibs --cflags` -g
	g++ PlotScanResult.C -o PlotScanResult.exec  `root-config --glibs --cflags` -g
clean:
	rm -f SuggestAPoint.exec && rm -f IsPointAcceptable.exec && rm -f StoreThisPoint.exec && rm -f LogToRoot.exec && rm -f SLHA2StartingPoint.exec
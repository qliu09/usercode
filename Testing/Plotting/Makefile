all:
	g++ Selective_Plot_Generator.C -o Selective_Plot_Generator.exec `root-config --glibs --libs --cflags`  -lFoam -lTreePlayer 
	g++ various_studies.C -o various_studies.exec `root-config --glibs --libs --cflags` -lFoam -lTreePlayer

various_studies.exec:
	g++ various_studies.C -o various_studies.exec `root-config --glibs --libs --cflags` -lFoam -lTreePlayer

Selective_Plot_Generator.exec:
	g++ Selective_Plot_Generator.C -o Selective_Plot_Generator.exec `root-config --glibs --libs --cflags`  -lFoam -lTreePlayer
	
clean:
	rm -f Create_All_Plots.exec && rm -f various_studies.exec

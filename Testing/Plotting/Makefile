all:
	g++ Create_All_Plots.C -o Create_All_Plots.exec  `root-config --glibs --libs --cflags`  -lFoam -lTreePlayer 
	g++ Selective_Plot_Generator.C -o Selective_Plot_Generator.exec  `root-config --glibs --libs --cflags`  -lFoam -lTreePlayer 
	g++ various_studies.C -o various_studies.exec  `root-config --glibs --libs --cflags` -lFoam -lTreePlayer

various_studies.exec: various_studies.C Modules/StudyModule.C
	g++ various_studies.C -o various_studies.exec  `root-config --glibs --libs --cflags` -lFoam -lTreePlayer

Create_All_Plots.exec: Create_All_Plots.C
	g++ Create_All_Plots.C -o Create_All_Plots.exec  `root-config --glibs --libs --cflags`  -lFoam -lTreePlayer
	
clean:
	rm -f Create_All_Plots.exec && rm -f various_studies.exec

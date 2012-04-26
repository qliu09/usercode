  function isEmpty(obj) {
    for (a in obj) {
       return false;
    };
    return true;
  };
  
  function process_histo(histogram) {
     if(histogram==undefined) return -123; // error code for undefined histogram
     var average=0
     var nevents=0
     for (var lineentry in histogram) {
        average+=histogram[lineentry].average*histogram[lineentry].nEvents
        nevents+=histogram[lineentry].nEvents
     }
     average/=nevents
     return average;
  }
   
  
  var data = {};
  var Errors = {};

  function SimplifiedTaskName(taskname,taskidentifier) {
     return taskname.substr(taskidentifier.length+2,taskname.length-taskidentifier.length-1)
  }
  

  var CMSSWrandomVersion=123;
  function GetCMSSWVersion(identifier) {
//CMSSWrandomVersion++
alert("Not known yet how to retrieve the CMSSW version (or site!) - filling in a random version ("+CMSSWrandomVersion+") right now.");
    return CMSSWrandomVersion;
  }

  function AODDQM(filename) {
    return filename.substr(filename.lastIndexOf("/")+1,filename.length-filename.lastIndexOf("/"));
  }

  function add_Job(identifier,JSON) {
     data[identifier] = {};
     var dataExist = false;
     
     data[identifier]["CMSSW"]={};
     data[identifier]["CMSSW"]["Configuration"]={}
     data[identifier]["CMSSW"]["Configuration"]["Version"]=GetCMSSWVersion(identifier);
     // we could store some more info here, such as NEvents or whatever.

     data[identifier]["output"] = {}
     for (var file in JSON.output) {
	data[identifier]["output"][AODDQM(file)] = {}
	for (var stat in JSON.output[file]) {
	  data[identifier]["output"][AODDQM(file)][stat] = JSON.output[file][stat];
	}
     }

     for (var task in JSON.performance) {
        data[identifier][SimplifiedTaskName(task,identifier)] = {};
        for (var step in JSON.performance[task]) {
            data[identifier][SimplifiedTaskName(task,identifier)][step] = {};
            for (var stat in JSON.performance[task][step]) {
                 if(JSON.performance[task][step][stat].average==undefined) {
                      data[identifier][SimplifiedTaskName(task,identifier)][step][stat] = process_histo(JSON.performance[task][step][stat].histogram)
                 } else {
                      data[identifier][SimplifiedTaskName(task,identifier)][step][stat] = JSON.performance[task][step][stat].average
                 }
            };
            if (isEmpty(data[identifier][SimplifiedTaskName(task,identifier)][step])) {delete data[identifier][SimplifiedTaskName(task,identifier)][step]};
        };
        if (isEmpty(data[identifier][SimplifiedTaskName(task,identifier)])) {delete data[identifier][SimplifiedTaskName(task,identifier)]};
     }
     
     
     Errors[identifier] = {}
     for (var task in JSON.errors) {
       Errors[identifier][SimplifiedTaskName(task,identifier)] = {}
       for (var step in JSON.errors[task]) {
         Errors[identifier][SimplifiedTaskName(task,identifier)][step] = {};
         for (var code in JSON.errors[task][step]) {
             document.write("Oops there seem to have been errors ... and I don't know how to handle them, yet")
         }
       }
     }
  }
  
  function fuse(additionalvalue, previousvalue, nentries) {
    return ((nentries-1)*previousvalue+additionalvalue)/nentries;
  }

  function SortData() {
  var isFirstTableEntry=true;
  var isFirstId=true;
  for (id in data) {
       if(!isFirstId) continue
       else isFirstId=false
       for (var stask in data[id]) {
       alloptions[stask]={}
         for (var step in data[id][stask]) {
         alloptions[stask][step]={}
           for (var stat in data[id][stask][step]) {
           alloptions[stask][step][stat]=""
           }
         }
       }
    }
  }

  function ReadDataBack() {
    for (identifier in data) {
       for (var stask in data[identifier]) {
         for (var step in data[identifier][stask]) {
           for (var stat in data[identifier][stask][step]) {
             document.write(identifier+"   ::    "+stask+"  ::  "+step+"  ::  "+stat+" : "+data[identifier][stask][step][stat]+"<br>");
           }
         }
       }
    }
  }
  
  isSetup=false

  function prepare_data(first,second,third) {
   table={}
   tableentries={}
   NEntries=0
   title="['Version'"
   for (identifier in data) {
     NEntries++
     version=data[identifier]["CMSSW"]["Configuration"]["Version"]
     value=data[identifier][first][second][third]
     if(title.length>0) title=title+" , '"+identifier+"'"
     else title=identifier

     if(table[version]==undefined) { 
       table[version]=" ";
       tableentries[version]=0;
       }

     for (var tversion in table) {
       if (tversion == version) {
         if (table[tversion].length>0) { 
           table[tversion]=table[tversion]+" , "+value
           tableentries[tversion]=tableentries[tversion]+1
         } 
     } else {
         if ((table[tversion]).length>0) { 
           table[tversion]=table[tversion]+" , 0 " 
           tableentries[tversion]=tableentries[tversion]+1
         }
     }
   }
  }
  title=title+"], ";
  var ivar=0
  finaltable="Gdata = google.visualization.arrayToDataTable(["+title
  for( var vers in table ) {
     ivar++
     while(tableentries[vers]<NEntries) {
       tableentries[vers]++
       table[vers]=", 0 "+table[vers]
     }
     if(ivar<NEntries) {table[vers]="['"+vers+"'"+table[vers]+"],"} 
     else { table[vers]="['"+vers+"'"+table[vers]+"]"} 
     finaltable=finaltable+table[vers]
     
  }
  finaltable=finaltable+"]);"

  eval(finaltable)
  drawVisualization(isSetup,first+":"+second+":"+third);
  isSetup=true;
  }

  alloptions={}
  
  function InitializeOptions() {
    for(var toplevel in alloptions) {
       var elOptNew = document.createElement('option');
       elOptNew.text = toplevel;
       elOptNew.value = toplevel;
       try {
         document.anyform.TopLevel.add(elOptNew, null); // standards compliant; doesn't work in IE
       }
       catch(ex) {
         document.anyform.TopLevel.add(elOptNew); // IE only
       }
    }
    LoadOptionsOne()
  }

  function LoadOptionsOne() {
    newval=document.anyform.TopLevel.value
    while (document.anyform.SecondLevel.length > 0){document.anyform.SecondLevel.remove(document.anyform.SecondLevel.length - 1);}
    while (document.anyform.ThirdLevel.length > 0) {document.anyform.ThirdLevel.remove(document.anyform.ThirdLevel.length - 1);}

    for(var secondlevel in alloptions[newval]) {
       var elOptNew = document.createElement('option');
       elOptNew.text = secondlevel;
       elOptNew.value = secondlevel;
       try { document.anyform.SecondLevel.add(elOptNew, null); // standard compliant (so it won't work in IE)
       } catch(ex) { document.anyform.SecondLevel.add(elOptNew); // IE only
       }
    }
    LoadOptionsTwo()
  }

  function LoadOptionsTwo() {
    topval= document.anyform.TopLevel.value
    newval=document.anyform.SecondLevel.value
    while (document.anyform.ThirdLevel.length > 0) {document.anyform.ThirdLevel.remove(document.anyform.ThirdLevel.length - 1);}

    for(var thirdlevel in alloptions[topval][newval]) {
       var elOptNew = document.createElement('option');
       elOptNew.text = thirdlevel;
       elOptNew.value = thirdlevel;
       try {
         document.anyform.ThirdLevel.add(elOptNew, null); // standard compliant (so it won't work in IE)
       } catch(ex) {
         document.anyform.ThirdLevel.add(elOptNew); // IE only
       }
    }
    LoadOptionsThree()
  }

  function LoadOptionsThree() {
    newval=document.anyform.ThirdLevel.value
    prepare_data(document.anyform.TopLevel.value,document.anyform.SecondLevel.value,document.anyform.ThirdLevel.value)
  }

<html>
<head>
<title>Testing php js JSON reading</title>
    <meta http-equiv="content-type" content="text/html; charset=utf-8"/>
    <script type="text/javascript" src="http://www.google.com/jsapi"></script>
    <script type="text/javascript">
      google.load('visualization', '1.1', {packages: ['controls']});
    </script>
    <script type="text/javascript">
    var Gdata
    var categoryPicker
    var ComboChart
    var options
    var dashboard
    isSetup=false;
    
    function drawVisualization(HasBeenSetup,VariableName) {
       if(!HasBeenSetup) {
        categoryPicker = new google.visualization.ControlWrapper({
          'controlType': 'CategoryFilter',
          'containerId': 'control1',
          'options': {
            'title': 'CMSSW Version',
            'filterColumnLabel': 'Version',
            'ui': {
              'allowTyping': false,
              'allowMultiple': true,
              'selectedValuesLayout': 'belowStacked'
            }
          },
          // Define an initial state, i.e. a set of metrics to be initially selected.
          'state': {'selectedValues': ['5.0', '5.1']} //////////////////////////////////////////////////////////////  Releases[0], if(count(Releases)>0) Releases[1] (only two)
        });
        }
        // Define a gauge chart.
        ComboChart = new google.visualization.ChartWrapper({
          'chartType': 'ComboChart',
          'containerId': 'chart1',
          'options': {
            'width': 1000,
            'height': 600,
            'title' : VariableName,
            'vAxis' : 
            {
	      'title': VariableName
	      },
	    'hAxis' : 
	    {
	      'title': "CMSSW Version"
	      },
	    'seriesType': "bars",
	    'series': {6: {'type': "line"}}  //////////////////////////////////////////////////////////////  NSites
          }
        });
      
        options = {
          title : VariableName,
          vAxis: {title: VariableName},
          hAxis: {title: "CMSSW Version"},
          seriesType: "bars",
          series: {6: {type: "line"}}
        };
        // Create the dashboard.
        dashboard = new google.visualization.Dashboard(document.getElementById('dashboard')).
          
          bind(categoryPicker, ComboChart).
          // Draw the dashboard
          draw(Gdata,options);
      }
      
    </script>
</head>
  <body style="font-family: Arial;border: 0 none;background-image:url(7628_1_other_wallpapers_simple.jpg)">
  Necessary steps: 
  <ul>
  <li> Obtain list of jobs to be processed (to do!)</li>
  <li> Load data from JSON and arrange it in a processable fashion for each job (done! [see below])</li>
  <li> Group jobs and obtain bottom line data per CMSSW release for comparison (done!)</li>
  <li> Illustrate results (first draft done, see <a href="Interface.html">here</a>). DONE! [see below]</li>
  </ul>
  
  <form name="anyform">
  <textarea name="anytext" cols=180 rows=10></textarea>
  <select name="TopLevel" onchange="LoadOptionsOne()"></select>
  <select name="SecondLevel" onchange="LoadOptionsTwo()"></select>
  <select name="ThirdLevel" onchange="LoadOptionsThree()"></select>
  </form>


  <style type="text/css">
a:link { font-weight:bold; color:white; text-decoration:none; }
a:visited { font-weight:bold; color:silver; text-decoration:none; }
a:focus { font-weight:bold; color:red; text-decoration:underline; }
a:hover { font-weight:bold; color:yellow; text-decoration:none; }
a:active { font-weight:bold; color:lime; text-decoration:underline; }

h1:focus { background-color:red; }
h1:hover { background-color:silver; }
h1:active { background-color:green; }
</style>
  
  <p align=center><font color="#FFFFFF">Pick your variable!</font></p>
  <table width="100%"><tr valign=middle align=center border=2 bordercolor=black>
  <tr>
  <td align=center> <a href="javascript:startwith('Runtime')">Runtime</a> </td>
  <td align=center><a href="javascript:startwith('CPUEfficiency')">CPU Efficiency</a></td>
  
  <td align=center><a href="javascript:notyet()">Time per Event</a></td>
  <td align=center><a href="javascript:notyet()">Memory Usage</a></td>
  <td align=center><a href="javascript:notyet()">Swap Usage</a></td>
  </tr></table>
  <table><tr><td align=center>
    <div id="dashboard">
      <table>
        <tr style='vertical-align: top'>
          <td style='width: 200px; font-size: 0.9em;'>
            <div id="control1"></div>
            <div id="control2"></div>
            <div id="control3"></div>
          </td>
          <td style='width: 400px'>
            <div style="float: left;" id="chart1"></div>
            <div style="float: left;" id="chart2"></div>
            <div style="float: left;" id="chart3"></div>
          </td>
        </tr>
      </table>
    </div>
    </tr></td></table>
      
  <script type="text/javascript">
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

  var cdata = {};
  var cErrors = {};
  
  function SimplifiedTaskName(taskname,taskidentifier) {
     return taskname.substr(taskidentifier.length+2,taskname.length-taskidentifier.length-1)
  }
  

  var CMSSWrandomVersion=123;
  function GetCMSSWVersion(identifier) {
CMSSWrandomVersion++
    document.write("<font color=#FF0000> Not known yet how to retrieve the CMSSW version - filling in a random version ("+CMSSWrandomVersion+") right now.</font><br>");
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
//        document.anyform.anytext.value=document.anyform.anytext.value+"\n Processing task: "+task      
//       document.write("Task name:"+task+"<br>")
        for (var step in JSON.performance[task]) {
            data[identifier][SimplifiedTaskName(task,identifier)][step] = {};
//            document.write("~~~~~~~~"+step+"<br>")
            for (var stat in JSON.performance[task][step]) {
                 if(JSON.performance[task][step][stat].average==undefined) {
                      data[identifier][SimplifiedTaskName(task,identifier)][step][stat] = process_histo(JSON.performance[task][step][stat].histogram)
                 } else {
                      data[identifier][SimplifiedTaskName(task,identifier)][step][stat] = JSON.performance[task][step][stat].average
                 }
//                 document.write("~~~~~~~~~~~~~~~~"+stat+" : "+data[identifier][SimplifiedTaskName(task,identifier)][step][stat]+"<br>")
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
    for (var identifier in data) {
       // reading out the CMSSW version of this task - if no data is available about it we just fill it in, otherwise we need to average
       var version = data[identifier]["CMSSW"]["Configuration"]["Version"]
//       document.write(" task "+identifier+" has CMSSW version "+version+"<br>\n");

       IsFirstTask = false
       if (isEmpty(cdata[version])) {
           IsFirstTask=true;
           cdata[version] = {}
//           document.write("<font color=#FF0000>task "+identifier+" is noticing: CMSSW version not yet present</font><br>");
           cdata[version]["NEntries"]=1;
       } else {
           cdata[version]["NEntries"]=cdata[version]["NEntries"]+1;
//           document.write("<font color=#00FF00>task "+identifier+" is noticing: CMSSW version is present</font><br>");
       }

       for (var stask in data[identifier]) {
         for (var step in data[identifier][stask]) {
           if(isEmpty(cdata[version][stask])) {
              cdata[version][stask] = {}
           }
           for (var stat in data[identifier][stask][step]) {
             if(isEmpty(cdata[version][stask][step])) {cdata[version][stask][step] = {};}
             if(IsFirstTask) {
               cdata[version][stask][step][stat]= data[identifier][stask][step][stat];
             } else {
               if(cdata[version][stask][step][stat]==undefined) {cdata[version][stask][step][stat] = data[identifier][stask][step][stat]}
               else {cdata[version][stask][step][stat] = fuse(data[identifier][stask][step][stat],cdata[version][stask][step][stat],cdata[version]["NEntries"]);}
             } 
           }
         }
       }
    }

/*    document.write("<h1>Going to read data per version back to you : </h1><br>");
    for (version in cdata) {
       for (var stask in cdata[version]) {
         for (var step in cdata[version][stask]) {
           for (var stat in cdata[version][stask][step]) {
             document.write(version+"   ::    "+stask+"  ::  "+step+"  ::  "+stat+" : "+cdata[version][stask][step][stat]+"<br>");
           }
         }
       }
    }*/

  var isFirstTableEntry=true;
  var isFirstId=true;
  for (id in cdata) {
       if(!isFirstId) continue
       else isFirstId=false
       for (var stask in cdata[id]) {
//document.write("------>"+id+" (id) "+stask+" (stask)<br>");
       if(stask=="NEntries") {continue;}
       alloptions[stask]={}
         for (var step in cdata[id][stask]) {
         alloptions[stask][step]={}
           for (var stat in cdata[id][stask][step]) {
           alloptions[stask][step][stat]=""
//             if(isFirstTableEntry) {
//                document.write("'"+stask+"::"+step+"::"+stat+"'");
//                isFirstTableEntry=false;
//             }
//             else {document.write(", '"+stask+"::"+step+"::"+stat+"'");}
           }
         }
       }
    }
  //and now let's write the actual data
/*
  isFirstId=true;
  document.write("],<br><br>");
  for (id in cdata) {
       isFirstTableEntry=true;
       if(isFirstId) {
            document.write("[");
            isFirstId=false;
       } 
       else {document.write(",\n [");} 
       for (var stask in cdata[id]) {
         for (var step in cdata[id][stask]) {
           for (var stat in cdata[id][stask][step]) {
             if(isFirstTableEntry) {
                document.write(cdata[id][stask][step][stat]);
                isFirstTableEntry=false;
             }
             else {document.write(","+cdata[id][stask][step][stat]);}
           }
         }
       }
    document.write("]<br>");
    }
  document.write("<br>]);<br> }");
*/

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
    document.write("<br>______________________________________________________________<br>");
    }
    document.write("<br><br>Done reading data back to you. bye bye.");
  }
  
  isSetup=false

  function prepare_data(first,second,third) {
//    alert("Prepping data");
//    document.write("data = google.visualization.arrayToDataTable([");
    // "title line"
//    alert("Need to continue with prepare_data")
//   alert("We need to load everything for "+first+" :: "+second+" :: "+third)
   document.anyform.anytext.value="\n\n"+document.anyform.anytext.value
   table={}
   tableentries={}
   NEntries=0
   title="['Version'"
   for (identifier in data) {
     NEntries++
     version=data[identifier]["CMSSW"]["Configuration"]["Version"]
     value=data[identifier][first][second][third]
     document.anyform.anytext.value="\n"+identifier+" (V "+version+") : "+value+document.anyform.anytext.value
     if(title.length>0) title=title+" , '"+identifier+"'"
     else title=identifier

//     if(table[version]==undefined) table[version]=" [' "+version+" '";
     if(table[version]==undefined) { 
       table[version]=" ";
       tableentries[version]=0;
       }

     for (var tversion in table) {
       if (tversion == version) {
         if (table[tversion].length>0) { 
           table[tversion]=table[tversion]+" , "+value
           tableentries[tversion]=tableentries[tversion]+1
//           document.write("Added value "+value+" to tversion="+tversion+"<br>");
         } 
     } else {
         if ((table[tversion]).length>0) { 
//           document.write("Looking for version="+version+" but found tversion "+tversion+"; therefore appending ' , '<br>");
           table[tversion]=table[tversion]+" , 0 " 
           tableentries[tversion]=tableentries[tversion]+1
         }
     }
   }
  }
  title=title+"], ";
//  for( var vers in table) {
//    table[tversion]
  document.anyform.anytext.value="title="+title+"\n";
  var ivar=0
  finaltable="Gdata = google.visualization.arrayToDataTable(["+title
  for( var vers in table ) {
     ivar++
//     document.write("Version "+vers+" contains "+tableentries[vers]+" entries")
     while(tableentries[vers]<NEntries) {
       tableentries[vers]++
       table[vers]=", 0 "+table[vers]
//       document.write("Beefed it up for version "+vers)
     }
     if(ivar<NEntries) {table[vers]="['"+vers+"'"+table[vers]+"],"} 
     else { table[vers]="['"+vers+"'"+table[vers]+"]"} 
     finaltable=finaltable+table[vers]
     
  }
  finaltable=finaltable+"]);"

  document.anyform.anytext.value=  document.anyform.anytext.value+"\n \n"+finaltable
  eval(finaltable)
  drawVisualization(isSetup,first+":"+second+":"+third);
  isSetup=true;


/*    document.write("<h1>Going to read data per version back to you : </h1><br>");
    for (version in cdata) {
       for (var stask in cdata[version]) {
         for (var step in cdata[version][stask]) {
           for (var stat in cdata[version][stask][step]) {
             document.write(version+"   ::    "+stask+"  ::  "+step+"  ::  "+stat+" : "+cdata[version][stask][step][stat]+"<br>");
           }
         }
       }
    }*/
    
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
//    alert(newval)
    while (document.anyform.SecondLevel.length > 0){document.anyform.SecondLevel.remove(document.anyform.SecondLevel.length - 1);}
    while (document.anyform.ThirdLevel.length > 0) {document.anyform.ThirdLevel.remove(document.anyform.ThirdLevel.length - 1);}

    for(var secondlevel in alloptions[newval]) {
       var elOptNew = document.createElement('option');
       elOptNew.text = secondlevel;
       elOptNew.value = secondlevel;
       try {
         document.anyform.SecondLevel.add(elOptNew, null); // standards compliant; doesn't work in IE
       }
       catch(ex) {
         document.anyform.SecondLevel.add(elOptNew); // IE only
       }
    }
    LoadOptionsTwo()
  }

  function LoadOptionsTwo() {
    topval= document.anyform.TopLevel.value
    newval=document.anyform.SecondLevel.value
//    alert(newval)
    while (document.anyform.ThirdLevel.length > 0) {document.anyform.ThirdLevel.remove(document.anyform.ThirdLevel.length - 1);}

    for(var thirdlevel in alloptions[topval][newval]) {
       var elOptNew = document.createElement('option');
       elOptNew.text = thirdlevel;
       elOptNew.value = thirdlevel;
       try {
         document.anyform.ThirdLevel.add(elOptNew, null); // standards compliant; doesn't work in IE
       }
       catch(ex) {
         document.anyform.ThirdLevel.add(elOptNew); // IE only
       }
    }
    LoadOptionsThree()
  }

  function LoadOptionsThree() {
    newval=document.anyform.ThirdLevel.value
    prepare_data(document.anyform.TopLevel.value,document.anyform.SecondLevel.value,document.anyform.ThirdLevel.value)
  }

  </script>

  
  <?php
  
  function process_job($job_identifier) {
//    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~<br>";
//    echo "Processing $job_identifier <br><br>";
    $lines = file($job_identifier);
    $json_content="";
    foreach ($lines as $line_num => $line) {
      if(strlen($line)>5) $json_content=$line; 
      }
   
   if(strlen($json_content)<4) {
      echo "Encountered a problem while attempting to load the workflow information for $job_identifier";
      return;
   }
   echo "\n<script>add_Job(\"$job_identifier\",$json_content);</script>";
   
  }
  
   process_job("etorassa_BPH-Summer12_DR52X-00001_T1_TW_ASGC_MSS_batch1_v1__120322_185648");
   process_job("etorassa_EXO-Fall11_R4-01282_T1_DE_KIT_MSS_batch19_v1__120301_222349");
   
  ?>
  
  
  <br><br>
<!--  <h2> Going to read task (!) data back to you</h2>-->
  <script>
//  ReadDataBack();

  SortData();
  InitializeOptions();

  </script>
  
  </body>
</html>
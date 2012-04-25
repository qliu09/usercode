<html>
<head>
<title>Testing php js JSON reading</title>
</head>
  <body style="font-family: Arial;border: 0 none;background-image:url(Plain_green_background.jpg)">
  
  
  <form name="anyform">
  <textarea name="anytext" cols=200 rows=30></textarea>
  </form>
  
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
  
  function SimplifiedTaskName(taskname,taskidentifier) {
     return taskname.substr(taskidentifier.length+1,taskname.length-taskidentifier.length)
  }
  
  function GetCMSSWVersion(identifier) {
    document.write("<font color=#FF0000> Not known yet how to retrieve the CMSSW version - filling in 123 right now.</font><br>");
    return 123;
  }
  function add_Job(identifier,JSON) {
     data[identifier] = {};
     var dataExist = false;
     
     for (var task in JSON.performance) {
        data[identifier][SimplifiedTaskName(task,identifier)] = {};
        document.anyform.anytext.value=document.anyform.anytext.value+"\n Processing task: "+task      
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
     
     data[identifier]["CMSSW"]={};
     data[identifier]["CMSSW"]["Configuration"]={}
     data[identifier]["CMSSW"]["Configuration"]["Version"]=GetCMSSWVersion(identifier);
     // we could store some more info here, such as NEvents or whatever.
     
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
    document.write("<br><br>Done reading data back to you. bye bye.");
  }
  
  </script>

  
  <?php
  
  function process_job($job_identifier) {
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~<br>";
    echo "Processing $job_identifier <br><br>";
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
   
//   process_job("etorassa_EXO-Fall11_R4-01282_T1_DE_KIT_MSS_batch19_v1__120301_222349");
   process_job("etorassa_EXO-Fall11_R4-01282_T1_DE_KIT_MSS_batch19_v1__120301_222349");
//   process_job("etorassa_EXO-Fall11_R4-01282_T1_DE_KIT_MSS_batch19_v1__120301_222349");
   
  ?>
  
  
  <br><br>
  <h2> Going to read data back to you</h2>
  <script>
  ReadDataBack();
  </script>
  
  </body>
</html>
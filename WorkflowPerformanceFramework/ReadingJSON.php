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
	    'series': {6: {'type': "line"}}
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


  <form  method="POST" action="ReadingJSON_Interface2.php"> 
  <table border=2 bordercolor=red>
  <tr><td> <font color=white>Jobs to process (separated by commas, please)</font></td><td>
  <textarea cols=100 rows=10 name="JobsToProcess">
etorassa_BPH-Summer12_DR52X-00001_T1_TW_ASGC_MSS_batch1_v1__120322_185648,etorassa_EXO-Fall11_R4-01282_T1_DE_KIT_MSS_batch19_v1__120301_222349
</textarea>
  </textarea>
  </td></tr>
  <tr><td></td><td>
  <input type="submit" value="Process these jobs">
  </td></tr>
  </form>
  </table>
  <font color=white>
  Necessary steps: 
  <ul>
  <li> Obtain list of jobs to be processed (done!)</li>
  <li> Load data from JSON and arrange it in a processable fashion for each job (done! [see below])</li>
  <li> Group jobs and obtain bottom line data per CMSSW release for comparison (done!)</li>
  <li> Illustrate results (done! [see below])</li>
  </ul>
  </font>


  <form name="anyform">
<!--  <textarea name="anytext" cols=180 rows=10></textarea>-->
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
  
  <table width="100%" border=2 bordercolor=red><tr valign=middle align=center border=2 bordercolor=black>
  <tr>
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
      
  <script type="text/javascript" src="DataProcessing.js">
  </script>

  
  <?php
  function cleanJob($jobname) {
     $jobname=preg_replace( '/\r\n/', ' ', trim($jobname) );
     $jobname=str_replace(" ","",$jobname);
     return $jobname;
  }

  function readJobArray($jobstring) {
     //this function splits up a string like this: job1,job2,job3 ... but we also have to be prepared for job1 , job2 ,job3, job4
     $jobs=explode(",",$jobstring);
     for($ijob=0;$ijob<count($jobs);$ijob++) {
        $jobs[$ijob]=cleanJob($jobs[$ijob]);
     }
     return $jobs;
  }

  function process_job($job_identifier) {
    $lines = file($job_identifier);
    $json_content="";
    foreach ($lines as $line_num => $line) {
      if(strlen($line)>5) $json_content=$line; 
      }
   
   if(strlen($json_content)<4) {
      echo '<script>alert("Encountered a problem while attempting to load the workflow information for '.$job_identifier.'");</script>';
      return;
   }
   echo "\n<script>add_Job(\"$job_identifier\",$json_content);</script>";
  }
  
  $NJobs=0;
  if(isset($_POST["JobsToProcess"])) {
     $jobs=readJobArray($_POST["JobsToProcess"]);
     for($ijob=0;$ijob<count($jobs);$ijob++) {
           if(strlen($jobs[$ijob])>1) {
              process_job($jobs[$ijob]);
              $NJobs++;
           }
     }
   }
  ?>
  
  
  <br><br>
  <script>
//  ReadDataBack();
  SortData();
  InitializeOptions();
  </script>
  </body>
</html>
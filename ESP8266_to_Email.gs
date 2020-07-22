// https://script.google.com/macros/s/AKfycbwz6w47rvzRFmHqavmB-9PChecR6tBEAUDzCfDYcM5gtaKzAAgu/exec
// Code source: https://create.arduino.cc/editor/LogMaker360/1104a08f-fb8b-47ff-8253-1cdc5c11581f/preview
// https://script.google.com/macros/s/AKfycbwz6w47rvzRFmHqavmB-9PChecR6tBEAUDzCfDYcM5gtaKzAAgu/exec?LDR=100&Button=Press
// http://api.pushingbox.com/pushingbox?devid=vDBD436D217B61B2&LDR=100&Button=Press
/*
//-----------------------------------------------
// Author: Trieu Le
// Email: lethanhtrieuk36@gmail.com
// Publish date: 07-Oct-2015
// Description: This code for demonstration send data from ESP8266 into Google Spreadsheet
// GET request syntax:
// https://script.google.com/macros/s/<gscript id>/exec?header_here=data_here
// Modifyed by Moz for Youtube changel logMaker360 for this video: https://youtu.be/fS0GeaOkNRw 24-02-2018
//-----------------------------------------------
/**
* Function doGet: Parse received data from GET request, 
  get and store data which is corresponding with header row in Google Spreadsheet
*/
function doGet(e) { 
  Logger.log( JSON.stringify(e) );  // view parameters
  var result = 'Ok'; // assume success
  if (e.parameter == 'undefined') {
    result = 'No Parameters';
  }
  else {
    var sheet_id = '1_ipT1JZ4SvCdiwp-9KWoFpZkusHwvTYm3QfrTfRjvjY'; 		// Spreadsheet ID
    var sheet = SpreadsheetApp.openById(sheet_id).getActiveSheet();		// get Active sheet
    var newRow = sheet.getLastRow();						
    var rowData = [];
	var Curr_Date = new Date(); 
    rowData[0] = Curr_Date; 											// Date in column A
	var Curr_Time = Utilities.formatDate(Curr_Date, "Asia/Jakarta", 'HH:mm:ss');
	rowData[1] = Curr_Time; 											// Time in column B
    for (var param in e.parameter) {
      Logger.log('In for loop, param=' + param);
      var value = stripQuotes(e.parameter[param]);
      Logger.log(param + ':' + e.parameter[param]);
      switch (param) {
        case 'LDR': //Parameter
          rowData[2] = value; //Value in column C
          break;
        case 'Button': //Parameter
          rowData[3] = value; //Value in column D
          break;  
        default:
          result = "unsupported parameter";
      }
    }
    Logger.log(JSON.stringify(rowData));
    // Write new row below
    var newRange = sheet.getRange(1, 1, 1, 3);
    newRange.setValues([rowData]);
  }
  // Return result of operation
  return ContentService.createTextOutput(result);
}
/**
* Remove leading and trailing single or double quotes
*/
function stripQuotes(value) {
  return value.replace(/^["']|['"]$/g, "");
}

function emailTest() {
  var sheet_id = '1_ipT1JZ4SvCdiwp-9KWoFpZkusHwvTYm3QfrTfRjvjY'; 		// Spreadsheet ID
  var sheet = SpreadsheetApp.openById(sheet_id).getActiveSheet();		// get Active sheet
  var latitude = sheet.getRange(2,3).getValue();
  var longitude = sheet.getRange(2,2).getValue();
  Logger.log(latitude);
  if (latitude > 1) {
    MailApp.sendEmail("4065701515@vtext.com", "Help", "Latitude: " + latitude + "  Longitude: " + longitude);
    sheet.deleteRow(2)
  }
}


//-----------------------------------------------
// End of file
//-----------------------------------------------

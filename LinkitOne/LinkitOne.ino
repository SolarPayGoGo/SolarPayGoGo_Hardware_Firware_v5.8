////////////////////////////////////////SYSTEM VARIABLES//////////////////////////////////////////////////
// Solar Charger Controller
int SCC_PV_MaxVoltage = 20;
int SCC_PV_MaxCurrent = 2;
int SCC_BAT_MaxVoltage = 15;
int SCC_MaxCurrent = 10;
int SCC_MinCurrent = SCC_MaxCurrent * -1; 

// update & Logging Interval
int internal_update_interval = 30000;
int logging_update_interval = 5;
// update & Logging Interval

// system call
#include "vmpwr.h"
// system call

// SD Card
#include <LTask.h>
#include <LFlash.h>
#include <LSD.h>
#include <LStorage.h>
#define Drv_SD LSD
#define Drv_Flash LFlash
LFile CurrentFile;
// SD card

// Main System Status
boolean connected_Battery = false;
boolean connected_Load = false;
// Main System Status

// Data Reacording
int Next_DataRecordTime [5];
char Data_FileName [29];
String fileName;
float dataLoggingValues [7];
// Data Reacording

// Date Time
#include <Time.h>
#include <LDateTime.h>
datetimeInfo t;
int NewDate[5];
int CurrentTime [5];
int TimeZone;
int activation_code_expiryTime [5];
int temp_activation_code_expiryTime [5];
int activation_code_issueTime [5];
int temp_activation_code_issueTime [5];
// Date Time

// GPS
#include <LGPS.h>
gpsSentenceInfoStruct info;
float latitude;
float longitude;
int GPSTime [5];
boolean GPSFixed;
// GPS

// Battery
#include <LBattery.h>
// Battery

// Activation_Code
const int SerialCode [15] = {1,9,4,2,4,7,3,6,4,0,7,1,6,5,4};
int Private_Key [10][10];
int temp_activation_code[16];
int temp_activation_code_data [6];
int activation_code[16];
int activation_code_data [6];
int Decrypted_Code [15];
// AC status
#define COM_error           0
#define AC_accepted         1
#define AC_error            2
#define AC_oldCode          3
#define PK_entered          4
#define APN_entered         5
#define FTP_entered         6
#define RES_entered         7
#define STA_entered         8
// Activation_Code

// GSM
#include <LGSM.h>
int SMSData[160];
char SMSRespondNumber [20];
// GSM

// GPRS
#include <LGPRS.h>
#include <LGPRSClient.h>
char APN [32];
char APN_Username [32];
char APN_Password [32];
LGPRSClient client;
LGPRSClient data_client;
// GPRS

// FTP
int UploadTime [5];
char Data_FileNameToUpload [29];
char FTP_server[32];
int FTP_port;
String FTP_username;
String FTP_password;
String FTP_parentPath;
String FTP_path = "194247364071654" ;
char outBuf[128];
char outCount;
// FTP

// LCD
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
// LCD

//RS485
unsigned long timeToGetDatafromSCC = 0;
const int load_connect = 2;
const int data_request = 3;
const int rs485_mode = 4;
//RS485

// User Interface

int lcd_backlightTimeOut;

int lcd_key     = 0;
int adc_key_in  = 0;
int last_lcd_key = 6;

int menu_item_key = 0;
int last_menu_item_key = -1;
int menu_value_key = 0;
boolean menu_select = false;

int menu_key = 0;
#define menuMain        0
#define menuEnterCode   1
#define menuViewCode    2
#define menuMaintenance 3
#define menuStatus      4

int menu_confirm_key = 0;
#define menu_confirm_amend   0
#define menu_confirm_true    1
#define menu_confirm_cancel  2

int code_entered_key = 0;
#define code_editing        0
#define code_entered        1
#define code_error          2
#define code_correct        3

#define subMenu_SerialCode  1
#define subMenu_CurrentTime 2
#define subMenu_BatStats    3
#define subMenu_SolarStats  4
#define subMenu_OutputStats 5
 
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5


// User Interface

// System stats
float SolarPanelVoltage;
float SolarPanelCurrent;
float BatteryVoltage;
float BatteryCurrent;
float OutputVoltage;
float OutputCurrent;
int SolarPanelPower;
int BatteryPercentage;
int OutputPower;
boolean MainBatteryCharging;
// System stats

////////////////////////////////////////SYSTEM VARIABLES//////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////SYSTEM METHODS///////////////////////////////////////////////////////

// System Call
boolean reboot(void* userdata) {
  vm_reboot_normal_start();//vm fuction to reboot
  return true; 
}
// System Call

// User Interface
int read_LCD_buttons(){
    adc_key_in = analogRead(A0);
    if (adc_key_in > 1000) return btnNONE; 
    if (adc_key_in < 50)   return btnLEFT;  
    if (adc_key_in < 195)  return btnDOWN; 
    if (adc_key_in < 380)  return btnUP; 
    if (adc_key_in < 555)  return btnRIGHT; 
    if (adc_key_in < 790)  return btnSELECT;   
    return btnNONE;
}

void respond_LCD_buttons(){
  lcd_key = read_LCD_buttons();
  menu_select = false;
  if (lcd_key != last_lcd_key) {
    lcd_backlightTimeOut = millis ()+ 5000;
    lcd.clear();
    switch (lcd_key){
        case btnRIGHT:{ menu_item_key ++; break;}
        case btnLEFT:{ menu_item_key --; break;}
        case btnUP:{ menu_value_key ++; break;}
        case btnDOWN:{ menu_value_key --; break;}
        case btnSELECT:{ menu_select = true; break;}
      }
    }
  last_lcd_key = lcd_key;    
}

void reset_respond_LCD_buttons(){
  menu_value_key = 0;
  menu_item_key = 0;
  menu_select = false;
}

void MainUserInterface (){
  respond_LCD_buttons();
  switch (menu_key){
    case menuMain:{
      if (menu_item_key > 4) { menu_item_key = 1;}
      if (menu_item_key < 1) { menu_item_key = 4;}
      if (menu_select == true) {menu_key = menu_item_key; reset_respond_LCD_buttons();}
      switch (menu_item_key){
        case menuEnterCode: {lcd.setCursor(0,0); lcd.print("<  Enter Code  >"); lcd.setCursor(0,1); lcd.print("                "); break;}
        case menuViewCode: {lcd.setCursor(0,0); lcd.print("<  View  Code  >"); lcd.setCursor(0,1); lcd.print("                "); break;}
        case menuMaintenance:{lcd.setCursor(0,0); lcd.print("<  Maintenance >"); lcd.setCursor(0,1); lcd.print("                "); break;}
        case menuStatus:{screen_SysStatus();break;}
      }
      break;
    }
    case menuEnterCode:{
      screen_enter_code();
      break;
    }
    case menuViewCode:{
      screen_viewCode();
      break;
    }
    case menuMaintenance:{
      screen_maintenance();
      break;
    }
    case menuStatus:{
      menu_key = menuMain;
      reset_respond_LCD_buttons();
      break;
    }
  }
}

void screen_enter_code(){
  switch (code_entered_key) {
        case code_editing:{
            if (menu_item_key > 15) { menu_item_key = 0;}
            if (menu_item_key < 0) { menu_item_key = 15;}
            if (last_menu_item_key != menu_item_key) {menu_value_key = temp_activation_code [menu_item_key]; last_menu_item_key = menu_item_key;} 
            if (menu_value_key > 9) { menu_value_key = 0;}
            if (menu_value_key < 0) { menu_value_key = 9;}

           for (int Current_Digit = 0; Current_Digit < 16 ; Current_Digit++){
             lcd.setCursor(Current_Digit,0);
             if (Current_Digit != menu_item_key) {lcd.print(" ");} else {lcd.print("@");}
             lcd.setCursor(Current_Digit,1);
             lcd.print(temp_activation_code [Current_Digit]);
          }       
          temp_activation_code [menu_item_key] = menu_value_key;
          if (menu_select == true) {code_entered_key = code_entered; reset_respond_LCD_buttons();}
          break;
        }
        case code_entered:{
          if (menu_item_key > 2) { menu_item_key = 0;}
          if (menu_item_key < 0) { menu_item_key = 2;}
          menu_confirm_key = menu_item_key;
          switch (menu_confirm_key){
            case menu_confirm_amend: {lcd.setCursor(0,0); lcd.print("<  Amend Code  >");lcd.setCursor(0,1); lcd.print("                "); if (menu_select == true) {code_entered_key = code_editing; reset_respond_LCD_buttons();} break;}
            case menu_confirm_cancel: {lcd.setCursor(0,0); lcd.print("<Cancel Changes>"); lcd.setCursor(0,1); lcd.print("                "); if (menu_select == true) {memset(temp_activation_code,0,sizeof(temp_activation_code));menu_key = menuMain; code_entered_key = code_editing; reset_respond_LCD_buttons(); } break;}
            case menu_confirm_true: {lcd.setCursor(0,0); lcd.print("< Confirm Code >"); lcd.setCursor(0,1); lcd.print("                "); if (menu_select == true) {reset_respond_LCD_buttons(); if (ActivationCode_Decryption()==AC_accepted){code_entered_key = code_correct;Write_ActivationCodeFile();}else{code_entered_key = code_error;} memset(temp_activation_code,0,sizeof(temp_activation_code));} break;}
          }
         break;
        }
        case code_error:{
          lcd.setCursor(0,0); lcd.print("Input Code Error"); lcd.setCursor(0,1); lcd.print("                ");
          if (menu_select == true) {reset_respond_LCD_buttons(); menu_key = menuMain; code_entered_key = code_editing;}
          break;
        }
        case code_correct:{
          lcd.setCursor(0,0); lcd.print("Input  Code  OK "); lcd.setCursor(0,1); lcd.print("                ");
          if (menu_select == true) {reset_respond_LCD_buttons(); menu_key = menuMain; code_entered_key = code_editing;}
          break;
        }
  }
}

void screen_SysStatus(){
  lcd.setCursor(0,0); lcd.print("B: ");
  lcd.setCursor(3,0); lcd.print(BatteryPercentage);
  lcd.setCursor(6,0); lcd.print("% @ ");
  lcd.setCursor(9,0); lcd.print(BatteryVoltage);
  lcd.setCursor(15,0); lcd.print("V");
  lcd.setCursor(0,1); lcd.print("EXP on: ");
  lcd.setCursor(8,1); lcd.print(activation_code_expiryTime[2]);
  lcd.setCursor(10,1); lcd.print("/");
  lcd.setCursor(11,1); lcd.print(activation_code_expiryTime[1]);
  lcd.setCursor(13,1); lcd.print("/");
  lcd.setCursor(14,1); lcd.print(activation_code_expiryTime[0]-2000);
}

void screen_viewCode(){
  String ActivationCodeToView;
  for (int k=0; k<16;k++){ActivationCodeToView += String(activation_code[k]);}
  String ExpiryTimeToView;
  for (int k=0; k<5;k++){if(activation_code_expiryTime[k]<10){ExpiryTimeToView += "0" + String(activation_code_expiryTime[k]);}else{ExpiryTimeToView += String(activation_code_expiryTime[k]);} if (k<4){ExpiryTimeToView += "/";}}
  lcd.setCursor(0,0); lcd.print(ActivationCodeToView); lcd.setCursor(0,1); lcd.print(ExpiryTimeToView);
  if (menu_select == true) {menu_key = menuMain; reset_respond_LCD_buttons();}
}

void screen_maintenance(){
  if (menu_item_key > 5) { menu_item_key = 1;}
      if (menu_item_key < 1) { menu_item_key = 5;}
      if (menu_select == true) {menu_key = menuMain; reset_respond_LCD_buttons();}
      switch (menu_item_key){
        case(subMenu_SerialCode):{lcd.setCursor(0,0);lcd.print(" Serial Number: ");lcd.setCursor(1,1); for (int k =0;k<15;k++){lcd.print(SerialCode[k]);}break;}
        case(subMenu_CurrentTime):{
          String CurrentTimeToView;
          for (int k=0; k<5;k++){if(CurrentTime[k]<10){CurrentTimeToView += "0" + String(CurrentTime[k]);}else{CurrentTimeToView += String(CurrentTime[k]);} if (k<4){CurrentTimeToView += "/";}}
          lcd.setCursor(0,0);lcd.print(" Current  Time: ");lcd.setCursor(0,1); lcd.print(CurrentTimeToView);break;}
        case(subMenu_BatStats):{
            lcd.setCursor(0,0); lcd.print("BAT:");
            lcd.setCursor(4,0); lcd.print(BatteryPercentage);
            lcd.setCursor(7,0); lcd.print("% @");
            lcd.setCursor(10,0); lcd.print(BatteryVoltage);
            lcd.setCursor(15,0); lcd.print("V");
            lcd.setCursor(0,1);
            if (MainBatteryCharging == true) {lcd.print("Charging ");}else{lcd.print("Draining ");}
            lcd.setCursor(10,1); lcd.print(abs(BatteryCurrent));
            lcd.setCursor(14,1); lcd.print(" A");
            break;}
        case(subMenu_SolarStats):{
            lcd.setCursor(0,0); lcd.print("Solar PV @");
            lcd.setCursor(10,0); lcd.print(SolarPanelVoltage);
            lcd.setCursor(15,0); lcd.print("V");
            lcd.setCursor(0,1); lcd.print(" @");
            lcd.setCursor(2,1); lcd.print(SolarPanelCurrent);
            lcd.setCursor(7,1); lcd.print("A @");
            lcd.setCursor(11,1); lcd.print(SolarPanelPower);
            lcd.setCursor(14,1); lcd.print("W ");
            break;}
        case(subMenu_OutputStats):{
            lcd.setCursor(0,0); lcd.print("Output   @");
            lcd.setCursor(10,0); lcd.print(OutputVoltage);
            lcd.setCursor(15,0); lcd.print("V");
            lcd.setCursor(0,1); lcd.print(" @");
            lcd.setCursor(2,1); lcd.print(OutputCurrent);
            lcd.setCursor(7,1); lcd.print("A @");
            lcd.setCursor(11,1); lcd.print(OutputPower);
            lcd.setCursor(14,1); lcd.print("W ");
            break;}
      }
}

// User Interface

// System Configuration
void Read_APNSysConfigFile(){
  CurrentFile = Drv_Flash.open("APNSys_Config.txt");
  if (CurrentFile){
    CurrentFile.seek(0);
    for (int k = 0; k < 5; k++){
      String CurrentWord;
      while (CurrentFile.available() != 0){
        if (CurrentFile.peek()== ','){CurrentFile.read();break;}
        CurrentWord += char(CurrentFile.read());
      }
      switch(k){
        case 0:{memset(APN,0,32);CurrentWord.toCharArray(APN,32);}
        case 1:{memset(APN_Username,0,32);CurrentWord.toCharArray(APN_Username,32);}
        case 2:{memset(APN_Password,0,32);CurrentWord.toCharArray(APN_Password,32);}
        case 3:{memset(SMSRespondNumber,0,20);CurrentWord.toCharArray(SMSRespondNumber,20);}
        case 4:{TimeZone=CurrentWord.toInt();}
      }
    }
    CurrentFile.close();
    Serial.println("Successfully opened APNSys_Config.txt file");
  }else{
    CurrentFile.close();
    CurrentFile = Drv_Flash.open("APNSys_Config.txt", FILE_WRITE);
    CurrentFile.print(" ");
    Serial.println("Successfully created APNSys_Config.txt file");
    CurrentFile.close();
  }
}

void Read_FTPConfigFile(){
  CurrentFile = Drv_Flash.open("FTP_Config.txt");
  if (CurrentFile){
    CurrentFile.seek(0);
    for (int k = 0; k < 7; k++){
      String CurrentWord;
      while (CurrentFile.available() != 0){
        if (CurrentFile.peek()== ','){CurrentFile.read();break;}
        CurrentWord += char(CurrentFile.read());
      }
      switch(k){
        case 0:{memset(FTP_server,0,32);CurrentWord.toCharArray(FTP_server,32);}
        case 1:{FTP_port=CurrentWord.toInt();}
        case 2:{FTP_username=CurrentWord;}
        case 3:{FTP_password=CurrentWord;}
        case 4:{FTP_parentPath=CurrentWord;}
        case 5:{UploadTime [3] = CurrentWord.toInt();}
        case 6:{UploadTime [4] = CurrentWord.toInt();}
      }
    }
    CurrentFile.close();
    Serial.println("Successfully opened FTP_Config.txt file");
  }else{
    CurrentFile.close();
    CurrentFile = Drv_Flash.open("FTP_Config.txt", FILE_WRITE);
    CurrentFile.print(" ");
    Serial.println("Successfully created FTP_Config.txt file");
    CurrentFile.close();
  }
}

void Write_APNSysConfigFile(){
  Drv_Flash.remove("APNSys_Config.txt");
  CurrentFile = Drv_Flash.open("APNSys_Config.txt", FILE_WRITE);
  if (CurrentFile){
    CurrentFile.seek(0);
    CurrentFile.print(String(APN));
    CurrentFile.print(",");
    CurrentFile.print(String(APN_Username));
    CurrentFile.print(",");
    CurrentFile.print(String(APN_Password));
    CurrentFile.print(",");
    CurrentFile.print(String(SMSRespondNumber));
    CurrentFile.print(",");
    CurrentFile.print(String(TimeZone));
    Serial.println("Successfully written APNSys_Config.txt file");
  }else{
    Serial.println("Error when writing APNSys_Config.txt file");
  }
  CurrentFile.close();
}

void Write_FTPConfigFile(){
  Drv_Flash.remove("FTP_Config.txt");
  CurrentFile = Drv_Flash.open("FTP_Config.txt", FILE_WRITE);
  if (CurrentFile){
    CurrentFile.seek(0);
    CurrentFile.print(String(FTP_server));
    CurrentFile.print(",");
    CurrentFile.print(String(FTP_port));
    CurrentFile.print(",");
    CurrentFile.print(String(FTP_username));
    CurrentFile.print(",");
    CurrentFile.print(String(FTP_password));
    CurrentFile.print(",");
    CurrentFile.print(String(FTP_parentPath));
    CurrentFile.print(",");
    CurrentFile.print(String(UploadTime [3]));
    CurrentFile.print(",");
    CurrentFile.print(String(UploadTime [4]));
    Serial.println("Successfully written FTP_Config.txt file");
  }else{
    Serial.println("Error when writing FTP_Config.txt file");
  }
  CurrentFile.close();
}
// System Configuration

// Activation Code
void Write_ActivationCodeFile(){
  CurrentFile = Drv_Flash.open("Activation_Code.txt", FILE_WRITE);
  if (CurrentFile){
    CurrentFile.seek(0);
    for (int k = 0; k < 16; k++){
      CurrentFile.print(activation_code[k]);
    }
    Serial.println("Successfully written Activation_Code.txt file");
  }else{
    Serial.println("Error when writing Activation_Code.txt file");
  }
  CurrentFile.close();
}

void Read_ActivationCodeFile(){
  CurrentFile = Drv_Flash.open("Activation_Code.txt");
  if (CurrentFile){
    CurrentFile.seek(0);
    for (int k = 0; k < 16; k++){
      temp_activation_code[k]= CurrentFile.read() - '0';
    }
    CurrentFile.close();
    Serial.println("Successfully opened Activation_Code.txt file");
  }else{
    CurrentFile.close();
    CurrentFile = Drv_Flash.open("Activation_Code.txt", FILE_WRITE);
    CurrentFile.print(" ");
    Serial.println("Successfully created Activation_Code.txt file");
    CurrentFile.close();
  }
}

void Write_PrivateKeyFile(){
  CurrentFile = Drv_Flash.open("Private_Key.txt", FILE_WRITE);
  if (CurrentFile){
    CurrentFile.seek(0);
    for (int j = 0; j < 10; j++){
      for (int i = 0; i < 10; i++){
        CurrentFile.print(Private_Key[i][j]);
      }
    }
    Serial.println("Successfully written Private_Key.txt file");
  }else{
    Serial.println("Error when writing Private_Key.txt file");
  }
  CurrentFile.close();
}

void Read_PrivateKey(){
  CurrentFile = Drv_Flash.open("Private_Key.txt");
  if (CurrentFile){
    CurrentFile.seek(0);
    for (int j = 0; j < 10; j++){
      for (int i = 0; i < 10; i++){
        Private_Key[i][j]= CurrentFile.read() - '0';
      }
    }
    CurrentFile.close();
    Serial.println("Successfully opened Private_Key.txt file");
  }else{
    CurrentFile.close();
    CurrentFile = Drv_Flash.open("Private_Key.txt", FILE_WRITE);
    CurrentFile.print(" ");
    Serial.println("Successfully created Private_Key.txt file");
    CurrentFile.close();
  }
}

int ActivationCode_Decryption(){
  int checksum = 0;
  boolean dataValidation = false;
  Serial.print("Activation Code : ");
  for (int k = 0; k < 15 ; k++){
    Serial.print(temp_activation_code[k]);
    checksum += temp_activation_code[k];
  }
  Serial.print(temp_activation_code[15]);
  Serial.println("");
  checksum = checksum % 10;
  if (checksum == temp_activation_code [15]){
    Serial.println("Activation Code Checksum : OK");
    checksum = 0;
    for (int k = 0; k < 15 ; k++){
      Decrypted_Code [k] = Private_Key[temp_activation_code[k]][SerialCode[k]];
      checksum += Decrypted_Code [k];
    }
    checksum = checksum - Decrypted_Code [14];
    checksum = checksum % 10;
    if (checksum == Decrypted_Code [14]){
      Serial.println("Decryption Checksum : OK");
    dataValidation = ActivationCode_ExtractData();
    if (dataValidation == true){
      Serial.println("Data Validation : OK");
      ActivationCode_ExpireTime();
      if ( !ActivationCode_Expired (temp_activation_code_issueTime, activation_code_issueTime)){
        for ( int k = 0; k < 6; k++){activation_code_data [k] = temp_activation_code_data [k];}
        for ( int k = 0; k < 16; k++){activation_code [k] = temp_activation_code [k];}
        for ( int k = 0; k < 5; k++){activation_code_issueTime [k] = temp_activation_code_issueTime [k]; activation_code_expiryTime [k] = temp_activation_code_expiryTime [k];}
        memset(temp_activation_code,0,sizeof(temp_activation_code));
        Serial.println("NEW Activation Code is accepted ");
        return AC_accepted;
        update_SystemStatus();
      } else {Serial.println("Old Activation Code entered");return AC_oldCode;}
    } else {Serial.println("Data Validation : Error"); return AC_error;}
  } else {Serial.println("Decryption Checksum : Error"); return AC_error;}
 }else {Serial.println("Activation Code Checksum : Error"); return AC_error;}
}

int ActivationCode_ExtractData(){
  temp_activation_code_data [0] = Decrypted_Code [0] * 10 + Decrypted_Code [1] + 2000;  //YYYY
  temp_activation_code_data [1] = Decrypted_Code [2] * 10 + Decrypted_Code [3];  //MM
  temp_activation_code_data [2] = Decrypted_Code [4] * 10 + Decrypted_Code [5];  //DD
  temp_activation_code_data [3] = Decrypted_Code [6] * 10 + Decrypted_Code [7];  //hh
  temp_activation_code_data [4] = Decrypted_Code [8] * 10 + Decrypted_Code [9];  //mm
  temp_activation_code_data [5] = Decrypted_Code [10] * 1000 + Decrypted_Code [11] * 100 + Decrypted_Code [12] * 10 + Decrypted_Code [13]; //hhhh
  if (temp_activation_code_data [0] > 2099 || temp_activation_code_data [0] < 2000){return false;}
  else if (temp_activation_code_data [1] > 12 || temp_activation_code_data [1] < 1){return false;}
  else if (temp_activation_code_data [2] > 31 || temp_activation_code_data [2] < 1){return false;}
  else if (temp_activation_code_data [3] > 23 || temp_activation_code_data [3] < 0){return false;}
  else if (temp_activation_code_data [4] > 59 || temp_activation_code_data [4] < 0){return false;}
  else if (temp_activation_code_data [5] > 9999 || temp_activation_code_data [5] < 0){return false;}
  else {return true;};
}

int LeapYearCheck (int year){
  if ((year % 4) == 0) {return true;}
  else {return false;}
}

void ActivationCode_ExpireTime (){
  int hours = temp_activation_code_data [5];
  int days = hours / 24;
  hours = hours % 24;
  for (int k = 0; k < 5; k++){ temp_activation_code_expiryTime [k] = temp_activation_code_data [k]; temp_activation_code_issueTime [k] = temp_activation_code_data[k];}
  temp_activation_code_expiryTime [3] += hours;
  if (temp_activation_code_expiryTime [3] > 23) { temp_activation_code_expiryTime [3] -= 24; days ++;}
  add_daysToDate(temp_activation_code_expiryTime,days);
  for (int k = 0; k < 5; k++){temp_activation_code_expiryTime [k] = NewDate[k];}
  Serial.print ("Activation Code ExpiryTime : ");
  for (int k = 0;k<5;k++){
    Serial.print(temp_activation_code_expiryTime[k]);
    Serial.print(" ");
  }
  Serial.println(" ");
}

void add_daysToDate(int inputDate[5], int days){
  boolean isLeapYear = false;
  while (days != 0){
    if (days > 0){
      inputDate [2] += days;
      if (inputDate [1] == 2){
      isLeapYear = LeapYearCheck (inputDate [0]);
      if (isLeapYear){
        if (inputDate [2] > 29) { days = days - (29 - (inputDate [2] - days)); inputDate [2] = 0; inputDate [1] ++;} else {days = 0;}
      } else {
        if (inputDate [2] > 28) { days = days - (28 - (inputDate [2] - days)); inputDate [2] = 0; inputDate [1] ++;} else {days = 0;}
      }
      } else if (inputDate [1] == 12){
        if (inputDate [2] > 31) { days = days - (31 - (inputDate [2] - days)); inputDate [2] = 0; inputDate [1] = 1; inputDate [0]++; } else {days = 0;}
      } else if (inputDate [1] == 4 || inputDate [1] == 6 || inputDate [1] == 9 || inputDate [1] == 11)  {
        if (inputDate [2] > 30) { days = days - (30 - (inputDate [2] - days)); inputDate [2] = 0; inputDate [1] ++;} else {days = 0;}
      } else {
        if (inputDate [2] > 31) { days = days - (31 - (inputDate [2] - days)); inputDate [2] = 0; inputDate [1] ++;} else {days = 0;}
      }
  } else {
    inputDate [2] += days;
    if (inputDate [1] == 3){
      isLeapYear = LeapYearCheck (inputDate [0]);
      if (isLeapYear){
        if (inputDate [2] < 1) { days = days + (inputDate [2] - days); inputDate [2] = 29; inputDate [1] --;} else {days = 0;}
      } else {
        if (inputDate [2] < 1) { days = days + (inputDate [2] - days); inputDate [2] = 28; inputDate [1] --;} else {days = 0;}
      }
    } else if (inputDate [1] == 1){
       if (inputDate [2] < 1) { days = days + (inputDate [2] - days); inputDate [2] = 31; inputDate [1] = 12; inputDate [0]--; } else {days = 0;}
    } else if (inputDate [1] == 2 || inputDate [1] == 4 || inputDate [1] == 6 || inputDate [1] == 8 || inputDate [1] == 9 || inputDate [1] == 11)  {
       if (inputDate [2] < 1) { days = days + (inputDate [2] - days); inputDate [2] = 31; inputDate [1] --;} else {days = 0;}
    } else {
       if (inputDate [2] < 1) { days = days + (inputDate [2] - days); inputDate [2] = 30; inputDate [1] --;} else {days = 0;}
    }
    }
  }
  for (int k = 0; k < 5; k++){NewDate [k]= inputDate[k];}
}

void update_SystemStatus(){
  if (ActivationCode_Expired (activation_code_expiryTime, CurrentTime)){
    connected_Load = false; 
  }else{
    connected_Load = true; 
  }
}
// Activation Code

// Date Time

void getCurrentTime(){
  LDateTime.getTime(&t);
  CurrentTime [0] = (t.year);
  CurrentTime [1] = (t.mon);
  CurrentTime [2] = (t.day);
  CurrentTime [3] = (t.hour);
  CurrentTime [4] = (t.min);
}

int ActivationCode_Expired (int Time_1 [5], int Time_2 [5]) {
  if (Time_1 [0] < Time_2 [0]){
    return true;
  } else if (Time_1 [0] == Time_2 [0]) {
    if (Time_1 [1] < Time_2 [1]){
      return true;
    } else if (Time_1 [1] == Time_2 [1]) {
      if (Time_1 [2] < Time_2 [2]){
        return true;
      } else if (Time_1 [2] == Time_2 [2]) {
        if (Time_1 [3] < Time_2 [3]){
          return true;
        } else if (Time_1 [3] == Time_2 [3]) {
          if (Time_1 [4] < Time_2 [4]){
            return true;
          } else {return false;}
        } else {return false;}
      } else {return false;}
    } else {return false;}
  } else {return false;}
}

// Date Time

// GPS

static unsigned char getComma(unsigned char num,const char *str){
  unsigned char i,j = 0;
  int len=strlen(str);
  for(i = 0;i < len;i ++)
  {
     if(str[i] == ',')
      j++;
     if(j == num)
      return i + 1; 
  }
  return 0; 
}

static double getDoubleNumber(const char *s){
  char buf[10];
  unsigned char i;
  double rev;
  i=getComma(1, s);
  i = i - 1;
  strncpy(buf, s, i);
  buf[i] = 0;
  rev=atof(buf);
  return rev; 
}

void extract_GPSData(const char* GPSData){
  int tmp;
  if(GPSData[0] == '$')
  {
    tmp = getComma(1, GPSData);
    GPSTime [3] = (GPSData[tmp + 0] - '0') * 10 + (GPSData[tmp + 1] - '0');
    GPSTime [4] = (GPSData[tmp + 2] - '0') * 10 + (GPSData[tmp + 3] - '0');
    tmp = getComma(2, GPSData);
    if (GPSData[tmp] == 'A'){ GPSFixed = true;} else {GPSFixed = false;}
    tmp = getComma(3, GPSData);
    latitude = getDoubleNumber(&GPSData[tmp]);
    tmp = getComma(5, GPSData);
    longitude = getDoubleNumber(&GPSData[tmp]);
    tmp = getComma(9, GPSData);
    GPSTime [0] = (GPSData[tmp + 4] - '0') * 10 + (GPSData[tmp + 5] - '0') + 2000;
    GPSTime [1] = (GPSData[tmp + 2] - '0') * 10 + (GPSData[tmp + 3] - '0');
    GPSTime [2] = (GPSData[tmp + 0] - '0') * 10 + (GPSData[tmp + 1] - '0');
  }
  else
  {
    Serial.println("GPS not geting data"); 
  }
}

void update_CurrentTime(){
  lcd.setCursor(0,1);
  lcd.print("GPS TIME SYN....");
  int TempTime[5];
  LGPS.powerOn();
  Serial.println("GPS being powered on..."); 
  delay(3000);
  GPSFixed = false;
  Serial.println("Waiting GPS to be fixed ..."); 
  while (GPSFixed == false){ 
    LGPS.getData(&info);
    //Serial.println((char*)info.GPRMC); 
    extract_GPSData((const char*)info.GPRMC);
  }
  LGPS.powerOff();
  for (int k = 0; k<5; k++){TempTime[k] = GPSTime[k];}
  TempTime[3] += TimeZone;
  if (TempTime[3] > 23) {TempTime[3] -= 24;add_daysToDate(TempTime,1);for (int k = 0; k<5; k++){TempTime[k] = NewDate[k];}}
  if (TempTime[3] < 0) {TempTime[3] += 24;add_daysToDate(TempTime,-1);for (int k = 0; k<5; k++){TempTime[k] = NewDate[k];}}
  t.year = TempTime[0];
  t.mon = TempTime[1];
  t.day = TempTime[2];
  t.hour = TempTime[3];
  t.min = TempTime[4];
  LDateTime.setTime(&t);
  Serial.print("GPS Time : ");
  for (int k = 0; k < 5; k++){
    Serial.print(GPSTime[k]);
    Serial.print(" ");
  }
  Serial.println(" ");
}

// GPS

// Back up Battery
void check_BatteryStatus(){
  Serial.print("Battery Level : ");
  Serial.print( LBattery.level());
  Serial.println(" ");
  Serial.print("Battery charging : ");
  Serial.print( LBattery.isCharging());
  Serial.println(" ");
}
// Back up Battery

// GSM
void check_GSMStatus(){
  Serial.println("GSM being powered on...");
  while(LSMS.ready() != true){
    delay(100);
  }
  Serial.println("GSM Ready");
}

void check_NewSMS(){
  if(LSMS.available()){
    memset(SMSData,0,sizeof(SMSData));
    for (int k = 0; k<160; k++){ SMSData [k] = LSMS.read(); if (SMSData [k]<0){break;} }
    char Responds_no[20];
    LSMS.remoteNumber(Responds_no, 20);
    LSMS.flush();
    extract_SMSData(Responds_no);
  } 
}

void extract_SMSData(char senderNumber [20]){
    int SMSRespondStatus;
    if (char(SMSData[0]) == '*'&&char(SMSData[17]) == '*'){
      Serial.println("New Activation Code entered via SMS");
      for (int k = 1; k<17 ; k++){ temp_activation_code [k-1] = SMSData[k] - '0';}
      SMSRespondStatus = ActivationCode_Decryption();
      if (SMSRespondStatus == AC_accepted){Write_ActivationCodeFile();}
    } else if (char(SMSData[0]) == '*' && char(SMSData[1]) == '#' &&char(SMSData[102]) == '#'){
      Serial.println("New Private Key entered via SMS");
      int tmp = 2;
      for (int j = 0; j < 10; j++){
        for (int i = 0; i < 10; i++){
        Private_Key[i][j]= SMSData[tmp] - '0';
        tmp ++;
        }
      }
      SMSRespondStatus = PK_entered;
      Write_PrivateKeyFile();
    } else if (char(SMSData[0]) == '*'&& char(SMSData[1]) == '#'&& char(SMSData[2]) == 'A'&& char(SMSData[3]) == 'P'&& char(SMSData[4]) == 'N'&& char(SMSData[5]) == '#') {
      Serial.println("New APN & System setting entered via SMS");
      SMSRespondStatus = APN_entered;
      int CurrentChar = 6;
      for (int k = 0; k < 5; k++){
        String CurrentWord;
        while (CurrentChar != 159){
          if (SMSData[CurrentChar]== ',' || SMSData[CurrentChar]=='#'){CurrentChar++;break;}
          else{CurrentWord += char(SMSData[CurrentChar]);
          CurrentChar++;}
        }
        switch(k){
          case 0:{memset(APN,0,32);CurrentWord.toCharArray(APN,32);}
          case 1:{memset(APN_Username,0,32);CurrentWord.toCharArray(APN_Username,32);}
          case 2:{memset(APN_Password,0,32);CurrentWord.toCharArray(APN_Password,32);}
          case 3:{memset(SMSRespondNumber,0,20);CurrentWord.toCharArray(SMSRespondNumber,20);}
          case 4:{TimeZone=CurrentWord.toInt();}
        }
      }
      Write_APNSysConfigFile();
      update_CurrentTime();
      setup_GPRS();
    } else if (char(SMSData[0]) == '*'&& char(SMSData[1]) == '#'&& char(SMSData[2]) == 'F'&& char(SMSData[3]) == 'T'&& char(SMSData[4]) == 'P'&& char(SMSData[5]) == '#') { 
      Serial.println("New FTP setting entered via SMS");
      SMSRespondStatus = FTP_entered;
      int CurrentChar = 6;
      for (int k = 0; k < 7; k++){
        String CurrentWord;
        while (CurrentChar != 159){
          if (SMSData[CurrentChar]== ',' || SMSData[CurrentChar] == '#'){CurrentChar++;break;}
          else{CurrentWord += char(SMSData[CurrentChar]);
          CurrentChar++;}
        }
        switch(k){
          case 0:{memset(FTP_server,0,32);CurrentWord.toCharArray(FTP_server,32);}
          case 1:{FTP_port=CurrentWord.toInt();}
          case 2:{FTP_username=CurrentWord;}
          case 3:{FTP_password=CurrentWord;}
          case 4:{FTP_parentPath=CurrentWord;}
          case 5:{UploadTime [3] = CurrentWord.toInt();}
          case 6:{UploadTime [4] = CurrentWord.toInt();}
        }
      }
      Write_FTPConfigFile();
    }else if (char(SMSData[0]) == '*'&& char(SMSData[1]) == '#'&& char(SMSData[2]) == 'R'&& char(SMSData[3]) == 'E'&& char(SMSData[4]) == 'S'&& char(SMSData[5]) == '#'){
      Serial.println("Reset Command entered via SMS");
      SMSRespondStatus = RES_entered;
      send_SMSRespond(senderNumber,SMSRespondStatus);
      LTask.remoteCall(reboot, NULL);
    }else if (char(SMSData[0]) == '*'&& char(SMSData[1]) == '#'&& char(SMSData[2]) == 'S'&& char(SMSData[3]) == 'T'&& char(SMSData[4]) == 'A'&& char(SMSData[5]) == '#'){
      Serial.println("Status request entered via SMS");
      SMSRespondStatus = STA_entered;
    }else{Serial.println("Error when Activation Code entered via SMS");SMSRespondStatus = COM_error;}
    send_SMSRespond(senderNumber,SMSRespondStatus);
}

void send_SMSWarning (){
  if ((activation_code_expiryTime [0] == CurrentTime [0]) && (activation_code_expiryTime [1] == CurrentTime [1]) && (activation_code_expiryTime [3] == CurrentTime [3]) && (activation_code_expiryTime [4] == CurrentTime [4]) ){
    int daysDifference = (activation_code_expiryTime [2] - CurrentTime [2]);
    LSMS.beginSMS(SMSRespondNumber);
    LSMS.print("Serial No : ");
    for (int k = 0; k < 15; k++){LSMS.print(SerialCode[k]);}
    LSMS.print('\n');
    switch (daysDifference){
      case 7: {LSMS.print("Current Activation Code is expiring in 7 days");addActivationCodeInfoToSMS();break;}
      case 3: {LSMS.print("Current Activation Code is expiring in 3 days");addActivationCodeInfoToSMS();break;}
      case 1: {LSMS.print("Current Activation Code is expiring in 1 day");addActivationCodeInfoToSMS();break;}
      case 0: {LSMS.print("Current Activation Code is expired");addActivationCodeInfoToSMS();break;}
    }
    if(LSMS.endSMS()){Serial.println("SMS Warning is sent");}
    else{Serial.println("SMS Warning is not sent");}
  }
}

void addActivationCodeInfoToSMS (){
  LSMS.print('\n');
  LSMS.print("Activation Code : ");
  for (int k = 0; k < 16; k++){LSMS.print(activation_code[k]);}
  LSMS.print('\n');
  LSMS.print("Issue Time : ");
  for (int k = 0; k < 5; k++){LSMS.print(activation_code_issueTime[k]);LSMS.print(" ");}
  LSMS.print('\n');
  LSMS.print("Expire Time : ");
  for (int k = 0; k < 5; k++){LSMS.print(activation_code_expiryTime[k]);LSMS.print(" ");}
}

void send_SMSRespond( char phone_no [20],int Status){
  LSMS.beginSMS(phone_no);
  for (int k = 0; k < 15; k++){LSMS.print(SerialCode[k]);}
  LSMS.print(",");
  switch (Status){
    case COM_error:{LSMS.print("COM,ERR");break;}
    case AC_accepted:{LSMS.print("ATC,ACC");break;}
    case AC_error: {LSMS.print("ATC,ERR");break;}
    case AC_oldCode: {LSMS.print("ATC,REJ");break;}
    case PK_entered: {LSMS.print("PRK,ACC");break;}
    case APN_entered: {LSMS.print("APN,ACC");break;}
    case FTP_entered: {LSMS.print("FTP,ACC");break;}
    case RES_entered: {LSMS.print("RES,ACC");break;}
    case STA_entered: {LSMS.print("STA,ACC");addSystemStatusToSMS();break;}
  }
  if(LSMS.endSMS()){Serial.println("SMS Responds is sent");}
  else{Serial.println("SMS Responds is not sent");}
  if (Status == AC_accepted) {send_SMS_ATC_ACCRespond();}
}

void send_SMS_ATC_ACCRespond(){
  LSMS.beginSMS(SMSRespondNumber);
  LSMS.print("Serial No : ");
  for (int k = 0; k < 15; k++){LSMS.print(SerialCode[k]);}
  LSMS.print('\n');
  LSMS.print("NEW Activation code is accepted");
  addActivationCodeInfoToSMS();
  if(LSMS.endSMS()){Serial.println("SMS Confitmation Respond is sent");}
  else{Serial.println("SMS Confitmation Respond is not sent");}
}

void addSystemStatusToSMS(){
  LSMS.print(",");
  LSMS.print(latitude);
  LSMS.print(",");
  LSMS.print(longitude);
  LSMS.print(",");
  for (int k = 0; k < 5; k++){LSMS.print(CurrentTime[k]);LSMS.print(",");}
  LSMS.print(SolarPanelVoltage);
  LSMS.print(",");
  LSMS.print(SolarPanelCurrent);
  LSMS.print(",");
  LSMS.print(BatteryVoltage);
  LSMS.print(",");
  LSMS.print(BatteryCurrent);
  LSMS.print(",");
  LSMS.print(BatteryPercentage);
  LSMS.print(",");
  LSMS.print(OutputVoltage);
  LSMS.print(",");
  LSMS.print(OutputCurrent);
}

// GSM

// ADC

void setup_Next_DataRecordTime(){
  getCurrentTime();
  for (int k = 0; k<5; k++){Next_DataRecordTime [k] = CurrentTime[k];}
}

int check_Next_DataRecordTime(){
  getCurrentTime();
  if (((Next_DataRecordTime[3] == CurrentTime[3]) && (Next_DataRecordTime[4] == CurrentTime[4]))||(ActivationCode_Expired(Next_DataRecordTime,CurrentTime)==true)){
    for (int k=0; k<5; k++){Next_DataRecordTime [k] = CurrentTime[k];}
    Next_DataRecordTime [4] += logging_update_interval;
    if (Next_DataRecordTime [4] == 60) {
      Next_DataRecordTime [4] = 0;
      Next_DataRecordTime [3] ++;
      if (Next_DataRecordTime [3] == 24) {
        Next_DataRecordTime [3] = 0;
        add_daysToDate(Next_DataRecordTime,1);
        for (int k = 0; k<5; k++){Next_DataRecordTime[k] = NewDate[k];}
      }
    }
    return true;
  } else {return false;}
}

void getDataFromSCC (){
  float previousValue[3];
  byte recived_message [10] [49];
  unsigned long timeOut;
  //while(Serial1.available()){Serial.read();}
  digitalWrite(data_request,HIGH);
  delay(1);
  digitalWrite(data_request,LOW);
  for (int k = 0; k < 10 ; k++){
    timeOut = millis() + 300;
    while(Serial1.available() == 0){
      if (timeOut < millis()){ break;}
    }
    int index = 0;
    while(Serial1.available()){
      recived_message[k][index] = Serial1.read();
      index++;
    } 
  }

  previousValue [0] = SolarPanelVoltage;
  previousValue [1] = SolarPanelCurrent;
  
  SolarPanelVoltage = recived_message[5][7] * 256;
  SolarPanelVoltage += recived_message[5][8];
  SolarPanelVoltage = SolarPanelVoltage / 100;
  SolarPanelCurrent = recived_message[5][9] * 256;
  SolarPanelCurrent += recived_message[5][10];
  SolarPanelCurrent = SolarPanelCurrent / 100;
  if ((SolarPanelVoltage > SCC_PV_MaxVoltage)||(SolarPanelVoltage < 0)) {SolarPanelVoltage = previousValue [0];}
  if ((SolarPanelCurrent > SCC_PV_MaxCurrent)||(SolarPanelCurrent < 0)) {SolarPanelCurrent = previousValue [1];}
  SolarPanelPower = SolarPanelVoltage * SolarPanelCurrent;
  
  previousValue [0] = BatteryVoltage;
  previousValue [1] = BatteryCurrent;
  previousValue [2] = BatteryPercentage;
  
  BatteryVoltage = recived_message[7][3] * 256;
  BatteryVoltage += recived_message[7][4];
  BatteryVoltage = BatteryVoltage /100;
  BatteryCurrent = recived_message[7][5] * 256;
  BatteryCurrent += recived_message[7][6];
  if (BatteryCurrent > 32767) {BatteryCurrent = (65536-BatteryCurrent)*(-1);};
  BatteryCurrent = BatteryCurrent /100;
  BatteryPercentage = recived_message[5][46];

  if ((BatteryVoltage > SCC_BAT_MaxVoltage)||(BatteryVoltage < 0)) {BatteryVoltage = previousValue [0];}
  if ((BatteryCurrent > SCC_MaxCurrent)||(BatteryCurrent < SCC_MinCurrent)) {BatteryCurrent = previousValue [1];}
  if ((BatteryPercentage > 100)||(BatteryVoltage < 0)) {BatteryPercentage = previousValue [2];}

  previousValue [0] = OutputVoltage;
  previousValue [1] = OutputCurrent;

  OutputVoltage = recived_message[5][23] * 256;
  OutputVoltage += recived_message[5][24];
  OutputVoltage = OutputVoltage /100;
  OutputCurrent = recived_message[5][25] * 256;
  OutputCurrent += recived_message[5][26];
  OutputCurrent = OutputCurrent /100;
  if ((OutputVoltage > SCC_BAT_MaxVoltage)||(OutputVoltage < 0)) {OutputVoltage = previousValue [0];}
  if ((OutputCurrent > SCC_MaxCurrent)||(OutputCurrent < 0)) {OutputCurrent = previousValue [1];}
  OutputPower = OutputVoltage * OutputCurrent;

  if (BatteryCurrent > 0) {MainBatteryCharging = true;} else {MainBatteryCharging = false;}

  dataLoggingValues[0] = SolarPanelVoltage;
  dataLoggingValues[1] = SolarPanelCurrent;
  dataLoggingValues[2] = BatteryVoltage;
  dataLoggingValues[3] = BatteryCurrent;
  dataLoggingValues[4] = BatteryPercentage;
  dataLoggingValues[5] = OutputVoltage;
  dataLoggingValues[6] = OutputCurrent;

  Serial.print("SolarPanelVoltage : ");
  Serial.println (SolarPanelVoltage);
  Serial.print("SolarPanelCurrent : ");
  Serial.println (SolarPanelCurrent);
  Serial.print("SolarPanelPower : ");
  Serial.println (SolarPanelPower);
  Serial.print("BatteryVoltage : ");
  Serial.println (BatteryVoltage);
  Serial.print("BatteryCurrent : ");
  Serial.println (BatteryCurrent);
  Serial.print("BatteryPercentage : ");
  Serial.println (BatteryPercentage);
  Serial.print("OutputVoltage : ");
  Serial.println (OutputVoltage);
  Serial.print("OutputCurrent : ");
  Serial.println (OutputCurrent);
  Serial.print("OutputPower : ");
  Serial.println (OutputPower);

}

// ADC

// Data Logging
void Generate_fileName(int date[5]){
  fileName = "";
  for (int k = 0; k <15; k++){fileName += String(SerialCode[k]);}
  fileName += ('_');
  for (int k = 0; k <3; k++){if (date[k] < 10){fileName += ("0"+String(date[k]));} else {fileName += date[k];}}
  fileName += ".csv";
  fileName.toCharArray(Data_FileName,29);
}

void Write_DataFileHeader(){
  CurrentFile = Drv_SD.open(Data_FileName, FILE_WRITE);
  if (CurrentFile){
    CurrentFile.seek(0);
    for (int k = 0; k <15; k++){CurrentFile.print(SerialCode[k]);}
    CurrentFile.print(",");
    CurrentFile.print(longitude);
    CurrentFile.print(",");
    CurrentFile.print(latitude);
    CurrentFile.print(",");
    for (int k = 0; k <15; k++){CurrentFile.print(activation_code[k]);}
    CurrentFile.print(",");
    for (int k = 0; k <5; k++){if (activation_code_issueTime[k] < 10){CurrentFile.print("0" + String(activation_code_issueTime[k]));} else {CurrentFile.print(activation_code_issueTime[k]);}}
    CurrentFile.print(",");
    for (int k = 0; k <5; k++){if (activation_code_expiryTime[k] < 10){CurrentFile.print("0" + String(activation_code_expiryTime[k]));} else {CurrentFile.print(activation_code_expiryTime[k]);}}
    CurrentFile.print(",");
    for (int k = 0; k <3; k++){if (CurrentTime[k] < 10){CurrentFile.print("0"+String(CurrentTime[k]));} else {CurrentFile.print(CurrentTime[k]);}}
    CurrentFile.println("");
    Serial.println("Successfully Created "+ fileName +" file");
    Generate_FTPfileName();
  }else{
    Serial.println("Error when writing " + fileName + " file");
  }
  CurrentFile.close();
}

void Write_DataFile(){
  CurrentFile = Drv_SD.open(Data_FileName, FILE_WRITE);
  if (CurrentFile){
    for (int k = 3; k<5 ; k++){
      CurrentFile.print(CurrentTime[k]);
      CurrentFile.print(",");
    }
    for (int k = 0; k<7 ; k++){
      CurrentFile.print(String(dataLoggingValues[k]));
      if (k !=6){CurrentFile.print(",");}
    }
    CurrentFile.println("");
    Serial.println("Successfully Written "+ fileName +" file");
  }else{
    Serial.println("Error when writing " + fileName + " file");
  }
  CurrentFile.close();
}

void Open_DataFile(){
  getCurrentTime();
  Generate_fileName(CurrentTime);
  CurrentFile = Drv_SD.open(Data_FileName);
  if (!CurrentFile){
    CurrentFile.close();
    Write_DataFileHeader();
    Generate_FTPfileName();
  }
  CurrentFile.close();
}

// Data Logging

// GPRS
void setup_GPRS(){
  Serial.println("Connecting to GPRS network by pre-enetered APN setting");
  while(!LGPRS.attachGPRS(APN,APN_Username,APN_Password))
  {
    delay(500);
  }
}
// GPRS

// FTP
boolean upload_DataToFTP(char FTP_fileName []){
  CurrentFile = Drv_SD.open(FTP_fileName);
  if (CurrentFile){CurrentFile.seek(0);} else { Serial.println ("Error when reading Data File"); CurrentFile.close(); return false;}
  if (client.connect(FTP_server,FTP_port)) { Serial.println ("Connected to FTP Server");} else { Serial.println ("Fail to connect FTP Server"); CurrentFile.close(); return false;}
  if(!eRcv()) return false;
  client.println("USER " + FTP_username);
  if(!eRcv()) return false;
  client.println("PASS " + FTP_password);
  if(!eRcv()) return false;
  client.println("SYST");
  if(!eRcv()) return false;
  client.println("Type I");
  if(!eRcv()) return false;
  client.println("PASV");
  if(!eRcv()) return false;
  char *tStr = strtok(outBuf,"(,");
  int array_pasv[6];
  for ( int i = 0; i < 6; i++) {
    tStr = strtok(NULL,"(,");
    array_pasv[i] = atoi(tStr);
  }
  unsigned int hiPort,loPort;
  hiPort = array_pasv[4] << 8;
  loPort = array_pasv[5] & 255;
  hiPort = hiPort | loPort;
  if (data_client.connect(FTP_server,hiPort)) {Serial.println("Connected to FTP Data Server");} 
  else {Serial.println("Fail to connect FTP Data Server"); client.stop(); CurrentFile.close(); return false;}
  client.print("CWD ");
  client.println("/"+FTP_parentPath);
  if(!eRcv()) return false;
  client.print("CWD ");
  client.println("/"+FTP_parentPath+"/"+FTP_path);
  if(!eRcv_path()){client.print("MKD ");  client.println(FTP_path);if(!eRcv()) {return false;} client.print("CWD ");client.println("/"+FTP_parentPath+"/"+FTP_path);}
  String target_fileName = String(FTP_fileName);
  Serial.println(target_fileName);
  client.print("STOR ");
  client.println(target_fileName);
  if(!eRcv()){data_client.stop(); return false;}
  Serial.println("Uploading Data File to FTP Server");
  byte clientBuf[64];
  int clientCount = 0;
  while(CurrentFile.available()){
    clientBuf[clientCount] = CurrentFile.read();
    clientCount++;
    if(clientCount > 63){ data_client.write(clientBuf,64);clientCount = 0;}
  }
  if(clientCount > 0) data_client.write(clientBuf,clientCount);
  data_client.stop();
  Serial.println("FTP Data Server disconnected");
  if(!eRcv()) return false;
  client.println("QUIT");
  if(!eRcv()) return false;
  client.stop();
  Serial.println("FTP Server disconnected");
  CurrentFile.close();
  return true;
}

byte eRcv(){
  byte respCode;
  byte thisByte;
  unsigned long FTP_timeout;
  FTP_timeout = millis()+5000;
  while(!client.available()) {
    delay(1);
    if (FTP_timeout < millis()){efail(); return 0;}
  }
  respCode = client.peek();
  outCount = 0;
  while(client.available()){  
    thisByte = client.read();    
    Serial.write(thisByte);
    if(outCount < 127){outBuf[outCount] = thisByte; outCount++; outBuf[outCount] = 0;}
  }
  if(respCode >= '4'){efail(); return 0;}
  return 1;
}

byte eRcv_path(){
  byte respCode;
  byte thisByte;
  while(!client.available()) delay(1);
  respCode = client.peek();
  outCount = 0;
  while(client.available()){  
    thisByte = client.read();    
    Serial.write(thisByte);
    if(outCount < 127){outBuf[outCount] = thisByte; outCount++; outBuf[outCount] = 0;}
  }
  if(respCode >= '4'){return 0;}
  return 1;
}

void efail(){
  byte thisByte = 0;
  client.println("QUIT");
  while(!client.available()) delay(1);
  while(client.available()){thisByte = client.read(); Serial.write(thisByte);}
  client.stop();
  Serial.println("Error occured FTP disconnected");
  CurrentFile.close();
}

void Generate_FTPfileName(){
  int PreviousDate [5];
  getCurrentTime();
  add_daysToDate(CurrentTime,-1);
  for (int k = 0; k<5 ; k++){PreviousDate[k] = NewDate[k];}
  Generate_fileName(PreviousDate);
  for (int k = 0; k<29 ; k++){Data_FileNameToUpload[k] = Data_FileName[k];}
  getCurrentTime();
  Generate_fileName(CurrentTime);
}

boolean check_TimeToUpload(){
  if (UploadTime[3] == CurrentTime[3] && UploadTime [4] == CurrentTime [4]){return true;}
  return false; 
}
// FTP

////////////////////////////////////////SYSTEM METHODS///////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////SYSTEM MAIN LOOP///////////////////////////////////////////////////////


void setup() {
  pinMode(data_request, OUTPUT);
  pinMode(load_connect, OUTPUT);
  pinMode(rs485_mode, OUTPUT);
  digitalWrite(data_request,LOW);
  digitalWrite(load_connect,LOW);
  digitalWrite(rs485_mode,LOW);
  
  lcd.init(); 
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("SYS BOOTING UP");
  Serial.begin(9600);
  Serial.println("Serial Connection Ready");
  Serial.print("Device's Serial Code is : ");
  for (int k = 0; k <15; k++){Serial.print(SerialCode[k]);}
  Serial.println(" ");
  LTask.begin();
  Drv_Flash.begin();
  Drv_SD.begin();
  Read_APNSysConfigFile();
  Read_FTPConfigFile();
  Read_ActivationCodeFile();
  Read_PrivateKey();
  check_BatteryStatus();
  check_GSMStatus();
  setup_GPRS();
  update_CurrentTime();
  getCurrentTime();
  Serial.print ("Current Time : ");
  for (int k = 0;k<5;k++){
    Serial.print(CurrentTime[k]);
    Serial.print(" ");
  }
  Serial.println(" ");
  Wire.begin();
  ActivationCode_Decryption();
  Generate_fileName(CurrentTime);
  setup_Next_DataRecordTime();
  update_SystemStatus();
  
  Serial1.begin(115200);
}

void loop() {
  check_NewSMS();
  getCurrentTime();
  update_SystemStatus();
  if (connected_Load == true) {digitalWrite(load_connect, HIGH);} else {digitalWrite(load_connect, LOW);}
  if (timeToGetDatafromSCC < millis()){ timeToGetDatafromSCC = millis() + internal_update_interval; getDataFromSCC();}
  if (lcd_backlightTimeOut < millis()){lcd.noBacklight(); delay(2000);} else {lcd.backlight();}
  if (check_Next_DataRecordTime()){
       send_SMSWarning();
       Open_DataFile();
       Write_DataFile();
       if(check_TimeToUpload()){Serial.println("Going to upload");Generate_FTPfileName();upload_DataToFTP(Data_FileNameToUpload);} //update_CurrentTime();
  }
  MainUserInterface();
}

////////////////////////////////////////SYSTEM MAIN LOOP///////////////////////////////////////////////////////


#include "GSM.h"
#include <SoftwareSerial.h>

void setup(){
  Serial.begin(9600);
  delay(1000);
  GSM my_gsm;
  delay(2000);
  my_gsm.check_SMS_command("~1?CMD_TYPE:USDD=*544#,98,15,1,3,5,5,r;TAR=07123930223;EXP_PARTERN=Safaricom;CMD_END");

}

void loop(){}

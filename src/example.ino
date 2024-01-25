#include "GSM.h"

GSM my_gsm;

void setup()
{
  Serial.begin(9600);
  delay(2000);
  // my_gsm.check_SMS_command("~1?CMD_TYPE:USDD=*544#,98,15,1,3,5,5,r;TAR=07123930223;EXP_PARTERN=Safaricom;CMD_END");
  my_gsm.HTTP_GET("www.google.com");
}

void loop()
{

  delay(2000);
  while (Serial.available() > 0)
  {
    String ANSWER = my_gsm.handle_AT_CMD(Serial.readString());
    }
}

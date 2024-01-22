#include "GSM.h"
#include <SoftwareSerial.h>

#include "GSM_config.h"

// Function declarations
void run_USSD(String info);
void check_SMS_command(String msg_str);

SoftwareSerial GSM_Serial(GSM_TX, GSM_RX); // Connect TX and RX pins of GSM to RX and TX pins of microcontroller respectively

GSM::GSM()
{

  GSM_Serial.begin(9600);
  // init();
}

// SEND RAW_AT_COMMANDS

String GSM::handle_AT_CMD(String cmd)
{
  while (Serial.available() > 0)
  {
    Serial.read();
  }
  String RESPONSE = "";
  GSM_Serial.println(cmd);
  delay(100); // Avoid putting any code that might delay the receiving all contents from the serial buffer as it is quickly filled up

  while (GSM_Serial.available() > 0)
  {
    RESPONSE += GSM_Serial.readString();
  }
  Serial.println();
  Serial.println("GSM RESPONSE:");
  Serial.println("-------");
  Serial.print(RESPONSE);
  Serial.println("-----");

  return RESPONSE;
}

bool GSM::is_GSM_Module()
{
  String res = handle_AT_CMD("AT");

  if (res != "OK")
    return false;

  return true;
}

bool GSM::SIM_Setup()
{
  String res = handle_AT_CMD("AT+CCID");
  res.trim();
  //  Serial.print(res);
  if (!res.endsWith("OK"))
  {
    Serial.println("SIM NOT DETECTED");
    return false;
  }

  else
  {
    SIM_INSERTED = true;
    Serial.println("SIM DETECTED!");
    Serial.println("Checking PIN ....");
    bool PIN_READY = pin_status();
    if (!PIN_READY)
    {
      Serial.println("PIN NOT SET");
      return false;
    };

    Serial.println("PIN READY!");
    SIM_Available = true;
    return true;
  }
}

bool GSM::pin_status()
{

  Serial.println("PIN STATUS\n\n");
  String res = handle_AT_CMD("AT+CPIN?");
  int start_index = res.indexOf(":");
  res = res.substring(start_index + 1);
  res.trim();
  Serial.println(res);
  if (res.startsWith("READY"))
    return true;

  else if (res.startsWith("SIM PIN"))
  { // ToDo: Attempt to set PIN;
    return false;
  }
  else if (res.startsWith("SIM PUK"))
  { // // ToDo: Attempt to set PUK;
    return false;
  }

  else
  {
    return false;
  }
}

String *GSM::string_separator(String str, char delimiter)
{ // note: pass delimiter using single quotes
  int arr_size = 12;
  String *separated_str = new String[arr_size];
  int arr_index = 0;

  while (str.indexOf(delimiter) != -1)
  {
    int delimiter_index = str.indexOf(delimiter);
    String extracted_code = str;
    extracted_code = extracted_code.substring(0, delimiter_index);
    str = str.substring(delimiter_index + 1);

    separated_str[arr_index] = extracted_code;
    arr_index++;
  }

  // separated_str[arr_index] = '\0';  // Does not seem to have an effect
  return separated_str;
}

void GSM::check_SMS_command(String msg_str)
{
  int index_start = msg_str.indexOf("CMD_TYPE:");
  int index_end = msg_str.indexOf("CMD_END");
  int check = index_start * index_end;

  if (index_start == -1 || index_end == -1)
  {
    Serial.println("No command found or Wrong command formart");
    return;
  }

  else
  {

    String start_str = "CMD_TYPE:";
    msg_str = msg_str.substring(msg_str.indexOf(start_str) + start_str.length(), index_end);
    Serial.println(msg_str);
    String CMD_TYPE = msg_str;                               // copy message string
    CMD_TYPE = CMD_TYPE.substring(0, CMD_TYPE.indexOf("=")); // Extract the string that bears the command just before the first '='
    // Serial.println(CMD_TYPE);

    if (CMD_TYPE = "USSD")
    {
      // run USSD code
      Serial.println("Attempt to run USSD code");
      run_USSD(msg_str);
      return;
    }

    else
    {
      Serial.print("Command ");
      Serial.print(CMD_TYPE);
      Serial.println(" not found or supported");
      return;
    }
  }
}

void GSM::run_USSD(String info)
{
  Serial.print("Received info: ");
  Serial.println(info);

  String *SEPARATED_CMD = string_separator(info, ';');
  Serial.print("Size of SEPARATED CMD: ");
  Serial.println(sizeof(SEPARATED_CMD));

  for (byte i = 0; i < arr_size(SEPARATED_CMD); i++)
  {
    Serial.println(SEPARATED_CMD[i]);
  }

  String USSD = SEPARATED_CMD[0].substring(SEPARATED_CMD[0].indexOf("=") + 1);
  String target = SEPARATED_CMD[1].substring(SEPARATED_CMD[1].indexOf("=") + 1);
  String pattern = SEPARATED_CMD[2].substring(SEPARATED_CMD[2].indexOf("=") + 1);

  delete[] SEPARATED_CMD;
  Serial.println("Extrated info from USSD message");
  Serial.println(USSD);
  Serial.println(target);
  Serial.println(pattern);

  String *USSD_seq = string_separator(USSD, ',');

  for (int i = 0; i <= arr_size(USSD_seq); i++)
  {
    // handle AT commands here
    Serial.println(USSD_seq[i]);
  }

  delete[] USSD_seq;

  // TODO: Check message with matching pattern and send to target number
}

int GSM::arr_size(String *str)
{
  int count = 0;
  //  Serial.println("Calculating size of array ...\n\n");
  while (str[count] != '\0')
  {
    count++;

    /**** Check if the index ahead of the loop has a null terminatinating character *****/
    // if(str[count] == '\0'){
    //   Serial.print("\nNull terminator found at index");
    //   Serial.println(count);
    // }
  }

  return count;
}
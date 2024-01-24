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
  while (str[count] != "\0")
  {
    count++;

    /**** Check if the index ahead of the loop has a null terminatinating character *****/
    // if(str[count] == '\0'){
    //   Serial.print("\nNull terminator found at index");
    //   Serial.println(count);
    // }
  }
  // Serial.println("Size of string array: ");
  // Serial.println(count);
  return count;
}

/***************
 * Check if GSM is connected to network
 *
 ************/

bool GSM::is_network_connected()
{
  // AT+CREG? should return an OK response if connected to a network

  // ToDO: Test the different CREG modes: 0,1 and 3

  String res = handle_AT_CMD("AT+CREG?");
  res.trim();

  int OK = res.indexOf("OK");

  if (OK == -1)
    return false;

  return true;
}

/****************
 * Get registered network / service prodiver name
 * AT+COPS? return a network name if module is registered to a network.
 * Example of a response
 *    AT+COPS?
 *    +COPS: 0,0,"Safaricom"
 *
 *    OK
 ****************/

String GSM::network_name()
{

  if (!is_network_connected())
    return "Not registered to network";

  String res = handle_AT_CMD("AT+COPS");
  int OK = res.indexOf("OK");

  if (OK == -1)
  {
    String msg = "Network name not found";
    // Serial.println(msg);
    return msg;
  };

  // Check for first and last instances of a double quote
  int start_index = res.indexOf("\"");
  int last_index = res.lastIndexOf("\"");

  if (start_index == -1 || last_index == -1)
  {
    Serial.println("Message string does not satisfy all subtring constraints");
  }
  // Otherwise, still send the message string
  res = res.substring(start_index + 1, last_index);
  // Serial.println(res);
  return res;
}

/*******
 *
 * Config GPRS
 */

bool GSM::gprs_config()
{
  String BEARER_SETTING = "AT+SAPBR=3,1,";
  String contype = BEARER_SETTING + "CONTYPE,GPRS";
  String res = handle_AT_CMD(contype);
  if (res.indexOf("OK") == -1)
    return false;

  // Set APN settings

  if (GSM_APN != "")
  {
    String set_APN = BEARER_SETTING + "APN," + GSM_APN;
    String res = handle_AT_CMD(set_APN);
    if (res.indexOf("OK") == -1)
    {
      Serial.println("Error setting bearer APN");
      return false;
    };
  }

  if (GSM_USER != "")
  {
    String set_USER = BEARER_SETTING + "USER," + GSM_USER;
    String res = handle_AT_CMD(set_USER);
    if (res.indexOf("OK") == -1)
    {
      Serial.println("Error setting bearer USER");
      return false;
    };
  }

  if (GSM_PWD != "")
  {
    String set_PWD = BEARER_SETTING + "PWD," + GSM_PWD;
    String res = handle_AT_CMD(set_PWD);
    if (res.indexOf("OK") == -1)
    {
      Serial.println("Error setting bearer PASSWORD");
      return false;
    };
  }

  return true;
}

// GPRS INIT

void GSM::GPRS_INIT()
{

  handle_AT_CMD("AT+SAPBR=1,1"); // Enable GPRS
  handle_AT_CMD("AT+HTTPINIT");
  handle_AT_CMD("AT+HTTPPARA=CID,1"); // CARRIER PROFILE

  _GPRS_ACTIVE = true;
}

//  HTTP GET REQUEST
void GSM::HTTP_GET(String URL)
{
  if (!_GPRS_ACTIVE)
    GPRS_INIT();

  handle_AT_CMD("AT+HTTPPARA=URL," + URL);
  handle_AT_CMD("AT+HTTPACTION=0"); // GET
  handle_AT_CMD("AT+HTTPREAD");
}

// GPRS TERMINATE

void GSM::GPRS_TERM()
{

  handle_AT_CMD("AT+HTTPTERM");  // Terminate HTTP service
  handle_AT_CMD("AT+SAPBR=1,1"); // Terminate GPRS service

  _GPRS_ACTIVE = false;
}

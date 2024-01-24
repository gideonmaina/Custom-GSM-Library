#ifndef GSM_H
#define GSM_H

#include "Arduino.h"

class GSM
{

public:
      GSM();
      int arr_size(String *str);
      String *string_separator(String str, char delimiter);
      void check_SMS_command(String msg_str);
      void run_USSD(String info);
      String handle_AT_CMD(String cmd);
      String network_name();
      void HTTP_GET(String URL);
      void GPRS_TERM();

private:
      void init();
      bool is_GSM_Module();
      bool SIM_Setup();
      bool pin_status();
      bool is_network_connected();
      bool gprs_config();
      void GPRS_INIT();
      bool _GPRS_ACTIVE = false;
};

#endif
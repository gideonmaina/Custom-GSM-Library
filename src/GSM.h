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

private:
      void init();
      bool is_GSM_Module();
      bool SIM_Setup();
      bool pin_status();
};

#endif
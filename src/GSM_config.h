
#define GSM_TX 0             // Pin connected to TX pin of GSM module
#define GSM_RX 2             // PIn connected to RX pin of GSM module
#define GSM_SERIAL_BAUD 9600 // baud rate for Software Serial communication with GSM module

// Bearer Settings
String GSM_APN = "";
String GSM_USER = "";
String GSM_PWD = "";

bool SIM_INSERTED = false;
bool SIM_Available = false;

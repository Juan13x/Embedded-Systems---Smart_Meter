#ifndef INTERNET_H
#define INTERNET_H

#include "internet/ethernet/enc28j60/my_enc28j60.h"

#define ENC28J60 0
#define EMBEDDED_WIFI 1

#define INTERNET_INTERFACE ENC28J60

#if INTERNET_INTERFACE==EMBEDDED_WIFI
//WIFI CONFIGURATION
#elif INTERNET_INTERFACE==ENC28J60
void internetInit(void);
#else
#endif

#endif //INTERNET_H
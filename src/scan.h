#ifndef _SCAN_H_
#define _SCAN_H_

#include "globals.h"

/* follow standard C identifier length */
#define MAXTOKENLEN 31

/* token_string array stores the lexeme of each token */
extern char token_string[MAXTOKENLEN+1];
/* token_string array stores the lexeme of identifier token */
extern char id_string[MAXTOKENLEN+1];

#endif

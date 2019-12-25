#ifndef IO_H
#define IO_H

#include "Button.h"

#define pin1 10
#define pin2 11
#define pin3 12

extern Button button1;
extern Button button2;
extern Button button3;

void setupIO();
void updateIO();

#endif
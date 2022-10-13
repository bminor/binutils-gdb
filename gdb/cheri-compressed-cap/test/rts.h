#pragma once

#include <stdbool.h>
#include <stdlib.h>

/* ***** Setup and cleanup functions for RTS ***** */
static int process_arguments(int argc, char** argv) { abort(); }
void setup_rts(void);
void cleanup_rts(void);

// this is needed for some reason:
static bool have_exception = false;

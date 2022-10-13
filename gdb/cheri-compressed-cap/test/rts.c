#include "rts.h"
#include "sail.h"
#include <stdbool.h>

void setup_rts(void) {
    // Avoid double initialization of sail.c data
    static int done_setup = false;
    if (!done_setup) {
        setup_library();
        done_setup = true;
    }
}

void cleanup_rts(void) {
    // Avoid double free of sail.c data
    static int done_cleanup = false;
    if (!done_cleanup) {
        cleanup_library();
        done_cleanup = true;
    }
}

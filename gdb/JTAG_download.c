/* Target dependent code for ARC700, for GDB, the GNU debugger.

   Copyright 2008, 2009 Free Software Foundation, Inc.

   Contributed by ARC International (www.arc.com)

   Authors:
      Richard Stuckey <richard.stuckey@arc.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/******************************************************************************/
/*                                                                            */
/* Outline:                                                                   */
/*     This module contains a test driver for the JTAG operations module of   */
/*     the ARC port of gdb.  It measures the speed at which data can be       */
/*     downloaded to the target.                                              */
/*                                                                            */
/* Usage:                                                                     */
/*           <driver>  [ -c ] [ -d ] [ -r <count> ]                           */
/*                                                                            */
/*           where -c specifies target connection & disconnection only        */
/*                 -d switches on JTAG operation debuggging                   */
/*                 -r specifies the JTAG operation retry count                */
/*                                                                            */
/******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include "arc-jtag-ops.h"


/* -------------------------------------------------------------------------- */
/*                               local data                                   */
/* -------------------------------------------------------------------------- */

#define MB                       1024 * 1024
#define DATA_AREA                0x00001000
#define DATA_AREA_SIZE           1 * MB


static ARC_Byte data[DATA_AREA_SIZE];


static Boolean test = TRUE;


/* -------------------------------------------------------------------------- */
/*                               externally visible data                      */
/* -------------------------------------------------------------------------- */

/* global debug flag */
Boolean arc_debug_target;


/* -------------------------------------------------------------------------- */
/*                               local functions                              */
/* -------------------------------------------------------------------------- */

static void failed(const char* fmt, ...)
{
    va_list ap;

    printf("*** FAILED: ");
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");

//  exit(EXIT_FAILURE);
}


static void run_test(void)
{
    unsigned int   i;
    unsigned int   bytes;
    struct timeval start_time, end_time;
    long long int  time_count;

    for (i = 0; i < DATA_AREA_SIZE; i++)
        data[i] = (ARC_Byte) i;

    gettimeofday (&start_time, NULL);

    bytes = arc_jtag_ops.memory_write_chunk(DATA_AREA, data, DATA_AREA_SIZE);

    gettimeofday (&end_time, NULL);

    if (bytes != DATA_AREA_SIZE)
        failed("memory write: %d", bytes);

    /* Compute the elapsed time in milliseconds, as a tradeoff between
       accuracy and overflow.  */
    time_count =  (end_time.tv_sec  - start_time.tv_sec)  * 1000;
    time_count += (end_time.tv_usec - start_time.tv_usec) / 1000;

    if (time_count > 0)
    {
        unsigned long rate = (bytes * 1000) / time_count;

        printf("0x%x bytes downloaded in %lld milliseconds\n", bytes, time_count);
        printf("transfer rate: %lu bytes/sec\n", rate);
    }


    gettimeofday (&start_time, NULL);

    bytes = arc_jtag_ops.memory_write_pattern(DATA_AREA, 0, DATA_AREA_SIZE);

    gettimeofday (&end_time, NULL);

    if (bytes != DATA_AREA_SIZE)
        failed("memory fill zero: %d", bytes);

    /* Compute the elapsed time in milliseconds, as a tradeoff between
       accuracy and overflow.  */
    time_count =  (end_time.tv_sec  - start_time.tv_sec)  * 1000;
    time_count += (end_time.tv_usec - start_time.tv_usec) / 1000;

    if (time_count > 0)
    {
        unsigned long rate = (bytes * 1000) / time_count;

        printf("0x%x bytes zeroed in %lld milliseconds\n", bytes, time_count);
        printf("zero rate: %lu bytes/sec\n", rate);
    }

}


static void process_options(int argc, char** argv)
{
    int c;

    while ((c = getopt (argc, argv, "dcr:")) != -1)
    {
        switch (c)
        {
            case 'd':
                arc_jtag_ops.state_machine_debug = TRUE;
                break;
            case 'r':
                arc_jtag_ops.retry_count = atoi(optarg);
                break;
            case 'c':
                test = FALSE;
                break;
            default:
                fprintf(stderr, "Usage: %s [ -d ]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
}


/* -------------------------------------------------------------------------- */
/*                               externally visible functions                 */
/* -------------------------------------------------------------------------- */

extern void _initialize_arc_jtag_ops(void);


int main(int argc, char** argv)
{
    Boolean opened;

    printf("Starting test of ARC JTAG download...\n");

    _initialize_arc_jtag_ops();

    process_options(argc, argv);

    opened = arc_jtag_ops.open();

    if (opened)
    {
        printf("ARC processor is connected\n");

        if (test)
            run_test();

        arc_jtag_ops.close();
    }

    printf("Finished test of ARC JTAG download\n");

    return 0;
}

/******************************************************************************/

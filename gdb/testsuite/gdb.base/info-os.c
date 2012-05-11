/* This testcase is part of GDB, the GNU debugger.

   Copyright 2011 Free Software Foundation, Inc.

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

#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <stdio.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *
thread_proc (void *args)
{
  pthread_mutex_lock (&mutex);
  pthread_mutex_unlock (&mutex);
}

int
main (void)
{
  const int flags = IPC_CREAT | 0666;
  int shmid, semid, msqid;
  FILE *fd;
  pthread_t thread;
  struct sockaddr_in sock_addr;
  int sock;
  unsigned short port;
  socklen_t size;
  int status;

  if ((shmid = shmget (3925, 4096, flags | IPC_EXCL)) < 0)
    {
      /* Attempt to delete the existing shared-memory region, then
	 recreate it.  */
      shmctl (shmget (3925, 4096, flags), IPC_RMID, NULL);
      if ((shmid = shmget (3925, 4096, flags | IPC_EXCL)) < 0)
	{
	  printf ("Cannot create shared-memory region.\n");
	  return 1;
	}	  
    }

  semid = semget (7428, 1, flags);
  msqid = msgget (5294, flags);
  fd = fopen ("/dev/null", "r");

  /* Lock the mutex to prevent the new thread from finishing immediately.  */
  pthread_mutex_lock (&mutex);
  pthread_create (&thread, NULL, thread_proc, 0);
 
  sock = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock < 0)
    {
      printf ("Cannot create socket.\n");
      return 1;
    }
 
  sock_addr.sin_family = AF_INET;
  sock_addr.sin_port = 0; /* Bind to a free port.  */
  sock_addr.sin_addr.s_addr = htonl (INADDR_ANY);

  status = bind (sock, (struct sockaddr *) &sock_addr, sizeof (sock_addr));
  if (status < 0)
    {
      printf ("Cannot bind socket.\n");
      return 1;
    }

  /* Find the assigned port number of the socket.  */
  size = sizeof (sock_addr);
  status = getsockname (sock, (struct sockaddr *) &sock_addr, &size);
  if (status < 0)
    {
      printf ("Cannot find name of socket.\n");
      return 1;
    }
  port = ntohs (sock_addr.sin_port);

  status = listen (sock, 1);
  if (status < 0)
    {
      printf ("Cannot listen on socket.\n");
      return 1;
    }

  /* Set breakpoint here.  */

  shmctl (shmid, IPC_RMID, NULL);
  semctl (semid, 0, IPC_RMID, NULL);
  msgctl (msqid, IPC_RMID, NULL);
  fclose (fd);
  close (sock);

  pthread_mutex_unlock (&mutex);
  pthread_join (thread, NULL);

  return 0;
}

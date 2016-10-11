//========================================================== file = mm1.c =====
//=  A simple M/M/1 queue simulation using SMPL                               =
//=============================================================================
//=  Notes: This program is adapted from "Figure 1.7" in Simulating Computer  =
//=         Systems, Techniques and Tools by M. H. MacDougall (1987)          =
//=---------------------------------------------------------------------------=
//=  Build: bcc32 mm1.c smpl.c bmeans.c rand.c                                =
//=         gcc mm1.c smpl.c bmeans.c rand.c -lm                              =
//=---------------------------------------------------------------------------=
//=  History: KJC (10/04/98) - Genesis                                        =
//=           KJC (02/18/05) - Cleaned-up for better understanding            =
//=============================================================================
//----- Include files ---------------------------------------------------------
#include <stdio.h>      // Needed for printf()
#include <stdlib.h>
#include "smpl.h"       // Needed for SMPL
#define TIME_Q_SIZE 1048576

struct TimeQ {
  int cur;
  float *queue;
};

//===== Main program ==========================================================
void main(void)
{
  // real Ta = 200;        // Mean interarrival time (seconds)
  real Ta;
  real Ts = 100;        // Mean service time
  real te = 1.0e6;      // Total simulation time
  int customer = 1;     // Customer id (always '1' for this simulation)
  int event;            // Event (1 = arrival, 2 = request, 3 = completion)
  int server;           // Handle for server facility
  int iterate = 10;     // Iterate time
  int i;
  float rho;

  for (rho = 0.0; rho < 0.96; rho += 0.025) {
    Ta = Ts / rho;
    double B_sum = 0.0;
    double Lq_sum = 0.0;
    double Ls_sum = 0.0;
    double Wq_sum = 0.0;
    for (i = 0; i < iterate; i++) {
      // Adding start time saving queue
      struct TimeQ aTimeQ;
      struct TimeQ sTimeQ;
      aTimeQ.cur = 0;
      sTimeQ.cur = 0;
      aTimeQ.queue = calloc(TIME_Q_SIZE, sizeof(float));
      sTimeQ.queue = calloc(TIME_Q_SIZE, sizeof(float));

      // Initialize SMPL subsystem
      smpl(0, "M/M/1 Queue");

      // Initialize server facility (single server)
      server=facility("server", 1);

      // Schedule arrival event at time 0 to kick-off simulation
      schedule(1, 0.0, customer);

      // Loop while simulation time is less than te
      while (time() < te)
      {
        // "Cause" the next event on the event list
        cause(&event,&customer);

        // Process the event
        switch(event)
        {
          case 1:  // *** Arrival
	    if (aTimeQ.cur != TIME_Q_SIZE) {
	      *(aTimeQ.queue + aTimeQ.cur) = time();
	      aTimeQ.cur ++;
	    }
	    schedule(2, 0.0, customer);
            schedule(1, expntl(Ta), customer);
            break;

          case 2:  // *** Request Server
            if (request(server, customer, 0) == 0) {
	      if (sTimeQ.cur != TIME_Q_SIZE) {
	        *(sTimeQ.queue + sTimeQ.cur) = time();
	        sTimeQ.cur ++;
              }
              schedule(3, expntl(Ts), customer);
	    }
            break;

          case 3:  // *** Release server
            release(server, customer);
            break;
        }
      }

      // Output standard SMPL report
      // report();
      B_sum += B(1);
      Lq_sum += Lq(1);
      Ls_sum += U(1);
      int j;
      double Wq = 0.0;
      for (j = 0; j < sTimeQ.cur; j++) {
        Wq += *(sTimeQ.queue + j) - *(aTimeQ.queue + j);
      }
      Wq_sum += Wq / sTimeQ.cur;
      free(aTimeQ.queue);
      free(sTimeQ.queue);
    }
    printf("B %10.3f Lq %10.3f L %10.3f, Wq %10.3f\n", B_sum / iterate, Lq_sum / iterate, (Lq_sum + Ls_sum) / iterate, Wq_sum / iterate);
  }
}

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
#define POOL_SIZE 4194304

struct TknInfo {
  real enter;
  real start;
  int server;
};

struct TknPool {
  int cur;
  int full;
  struct TknInfo *pool;
};

void initTokenPool(struct TknPool *tknPool) {
  tknPool->cur = 0;
  tknPool->full = 0;
  tknPool->pool = calloc(POOL_SIZE, sizeof(struct TknInfo));
}

void freeTokenPool(struct TknPool *tknPool) {
  free(tknPool->pool);
}

int getToken(struct TknPool *tknPool, int server) {
  struct TknInfo *tknInfo;
  tknInfo = tknPool->pool + tknPool->cur;
  if (tknPool->cur != POOL_SIZE) {
    tknInfo->server = server;
    tknInfo->enter = time();
  } else {
    tknPool->full = 1;
  }
  return tknPool->cur++;
}

void startToken(struct TknInfo *tknInfo) {
  tknInfo->start = time();
}

//===== Main program ==========================================================
void main(void)
{
  // real Ta = 200;        // Mean interarrival time (seconds)
  real Te;			// Mean arrival time of external input
  real Ts[4] = {1 / 4.0001, 1 / 1.0001, 1 / 1.0001 , 1 / 1.0001};	// Mean service time of server
  float p[4] = {0.25, 0.25, 0.25, 0.25};
  real te = 5.0e4;      // Total simulation time
  int customer = -1;    // Customer id
  int event;            // Event (1 = external arrival, 2 = request, 3 = completion)
  int servers[4];       // Handle for server facilitys
  int iterate = 10;     // Iterate time
  int i;
  float lambda;
  struct TknPool tknPool;

  for (lambda = 0.05; lambda < 1.01; lambda += 0.05) {
    Te = 1 / lambda;
    double W_sum[4] = {0.0, 0.0, 0.0, 0.0};	// W for facility 
    double L_sum[4] = {0.0, 0.0, 0.0, 0.0};	// L for facility 
    double U_sum[4] = {0.0, 0.0, 0.0, 0.0};	// Ls for facility 
    int full = 0;

    for (i = 0; i < iterate; i++) {
      // printf("iterate %d\n", i);
      // Initialize SMPL subsystem
      smpl(0, "M/M/1 Queue");
      initTokenPool(&tknPool);
      int tkn, server, target;
      double rand_d;

      // Initialize server facilitys (single server)
      servers[0] = facility("server1", 1);
      servers[1] = facility("server2", 1);
      servers[2] = facility("server3", 1);
      servers[3] = facility("server4", 1);

      // Schedule arrival event at time 0 to kick-off simulation
      schedule(1, 0.0, customer);

      // Loop while simulation time is less than te
      while (time() < te)
      {
        // "Cause" the next event on the event list
        cause(&event, &customer);

        // Process the event
        switch(event)
        {
          case 1:  // *** External Arrival
	    // printf("external arrival, token %d\n", customer);
	    tkn = getToken(&tknPool, 0);
	    schedule(2, 0.0, tkn);
            schedule(1, expntl(Te), -1);
            break;

          case 2:  // *** Request Server
	    server = (tknPool.pool + customer)->server;
	    //printf("request server %d, token %d\n", server, customer);
            if (request(servers[server], customer, 0) == 0) {
	      startToken(tknPool.pool + customer);
              schedule(3, expntl(Ts[server]), customer);
	    }
            break;

          case 3:  // *** Release server
	    server = (tknPool.pool + customer)->server;
            release(servers[server], customer);
	    switch(server) {
	      case 0:	// *** Release server 1
		rand_d = ranf();
		if (rand_d < p[0]) {
		  break;
		} else if (rand_d < p[0] + p[1]) {
		  target = 1;
		} else if (rand_d < p[0] + p[1] + p[2]) {
		  target = 2;
		} else if (rand_d < 1) {
		  target = 3;
		}
		tkn = getToken(&tknPool, target);
		schedule(2, 0.0, tkn);
	        break;
	      default:
	        tkn = getToken(&tknPool, 0);
	        schedule(2, 0.0, tkn);
	        break;
	    }

            break;
        }
      }

      // Output standard SMPL report
      // report();
      int j;
      for (j = 0; j < 4; j ++) {
        W_sum[j] += B(servers[j]);
        U_sum[j] += U(servers[j]);
        L_sum[j] += U(servers[j]) + Lq(servers[j]);
      }
      struct TknInfo *info;
      int count[4] = {0};
      double Wq_total[4] = {0};
      for (j = 0; j < tknPool.cur; j++) {
        info = tknPool.pool + j;
	server = info->server;
	if (info->start > 0) {
	  count[server]++;
          Wq_total[server] += info->start - info->enter;
	}
      }
      for (j = 0; j < 4; j++) {
        W_sum[j] += Wq_total[j] / count[j];
      }

      //printf("W_sum[0] %f, W_sum[1] %f, W_sum[2] %f, W_sum[3] %f\n", W_sum[0], W_sum[1], W_sum[2], W_sum[3]);
      if (tknPool.full) {
        full = 1;
      }
      freeTokenPool(&tknPool);
    }
    //printf("%3.2f %10.3f %10.3f %10.3f %10.3f %10.3f\n", lambda, W_sum[0] / iterate, W_sum[1] / iterate, W_sum[2] / iterate, W_sum[3] / iterate, (W_sum[0] + W_sum[1] + W_sum[2] + W_sum[3]) / iterate);
    //printf("%3.2f %10.3f %10.3f %10.3f %10.3f %10.3f\n", lambda, L_sum[0] / iterate, L_sum[1] / iterate, L_sum[2] / iterate, L_sum[3] / iterate, (L_sum[0] + L_sum[1] + L_sum[2] + L_sum[3]) / iterate);
    printf("%3.2f %10.3f %10.3f %10.3f %10.3f\n", lambda, U_sum[0] / iterate, U_sum[1] / iterate, U_sum[2] / iterate, U_sum[3] / iterate);
    //printf("Lambda %3.2f, W1 %10.3f, W2 %10.3f, W3 %10.3f, W4 %10.3f, W %10.3f\n", lambda, W_sum[0] / iterate, W_sum[1] / iterate, W_sum[2] / iterate, W_sum[3] / iterate, (W_sum[0] + W_sum[1] + W_sum[2] + W_sum[3]) / iterate);
    //printf("Lambda %3.2f, L1 %10.3f, L2 %10.3f, L3 %10.3f, L4 %10.3f, L %10.3f\n", lambda, L_sum[0] / iterate, L_sum[1] / iterate, L_sum[2] / iterate, L_sum[3] / iterate, (L_sum[0] + L_sum[1] + L_sum[2] + L_sum[3]) / iterate);
    //printf("Lambda %3.2f, U1 %10.3f, U2 %10.3f, U3 %10.3f, U4 %10.3f\n", lambda, U_sum[0] / iterate, U_sum[1] / iterate, U_sum[2] / iterate, U_sum[3] / iterate);
    if (full) {
      printf("Pool is full, lambda is %f\n", lambda);
    }
  }
}

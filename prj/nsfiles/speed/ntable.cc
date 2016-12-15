/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/*
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the Computer Systems
 *	Engineering Group at Lawrence Berkeley Laboratory.
 * 4. Neither the name of the University nor of the Laboratory may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/* Ported from CMU/Monarch's code, nov'98 -Padma.*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "ntable.h"

static int ntent_trich(const void *a, const void *b) {
  nsaddr_t ia = ((const ntable_ent *) a)->dst;
  nsaddr_t ib = ((const ntable_ent *) b)->dst;
  if (ia > ib) return 1;
  if (ib > ia) return -1;
  return 0;
}

NeighborTable::NeighborTable() {
  // Let's start with a ten element maxelts.
  elts = 0;
  maxelts = 10;
  ntab = new ntable_ent[maxelts];
}

void
NeighborTable::AddEntry(const ntable_ent &ent)
{
  ntable_ent *it;
  //DEBUG
  assert(ent.metric <= BIG);

  if ((it = (ntable_ent*) bsearch(&ent, ntab, elts, sizeof(ntable_ent), 
				 ntent_trich))) {
    bcopy(&ent,it,sizeof(ntable_ent));
    return;
    /*
    if (it->seqnum < ent.seqnum || it->metric > (ent.metric+em)) {
      bcopy(&ent,it,sizeof(ntable_ent));
      it->metric += em;
      return NEW_ROUTE_SUCCESS_OLDENT;
    } else {
      return NEW_ROUTE_METRIC_TOO_HIGH;
    }
    */
  }

  if (elts == maxelts) {
    ntable_ent *tmp = ntab;
    maxelts *= 2;
    ntab = new ntable_ent[maxelts];
    bcopy(tmp, ntab, elts*sizeof(ntable_ent));
    delete tmp;
  }
  
  int max;
  
  for (max=0;max<elts;max++) {
	  if (ent.dst < ntab[max].dst) {
		  break;
	  }
  }
  // Copy all the further out guys out yet another.
  // bcopy does not seem to quite work on sunos???
  
  //bcopy(&ntab[max], &ntab[max+1], sizeof(ntable_ent)*(elts-max));
  
  //if (elts) {
  int i = elts-1;
  while (i >= max) {
	  ntab[i+1] = ntab[i];
	  i--;
  }
  //}
  bcopy(&ent, &ntab[max], sizeof(ntable_ent));
  elts++;

  return;
}

void 
NeighborTable::InitLoop() {
  ctr = 0;
}

ntable_ent *
NeighborTable::NextLoop() {
  if (ctr >= elts)
    return 0;

  return &ntab[ctr++];
}

// Only valid (duh) as long as no new routes are added
int 
NeighborTable::RemainingLoop() {
  return elts-ctr;
}

ntable_ent *
NeighborTable::GetEntry(nsaddr_t dest) {
  ntable_ent ent;
  ent.dst = dest;
  return (ntable_ent *) bsearch(&ent, ntab, elts, sizeof(ntable_ent), 
				ntent_trich);
}

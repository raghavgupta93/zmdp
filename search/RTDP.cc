/********** tell emacs we use -*- c++ -*- style comments *******************
 $Revision: 1.4 $  $Author: trey $  $Date: 2006-02-11 22:38:10 $
   
 @file    RTDP.cc
 @brief   No brief

 Copyright (c) 2006, Trey Smith. All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are
 met:

 * The software may not be sold or incorporated into a commercial
   product without specific prior written permission.
 * The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 ***************************************************************************/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>

#include <iostream>
#include <fstream>
#include <queue>

#include "zmdpCommonDefs.h"
#include "zmdpCommonTime.h"
#include "MatrixUtils.h"
#include "Pomdp.h"
#include "RTDP.h"

using namespace std;
using namespace sla;
using namespace MatrixUtils;

#define OBS_IS_ZERO_EPS (1e-10)

namespace zmdp {

RTDP::RTDP(AbstractBound* _initUpperBound) :
  RTDPCore(_initUpperBound)
{}

void RTDP::updateInternal(MDPNode& cn, int* maxUBActionP)
{
  int maxUBAction = -1;
  double ubVal;
  double maxUBVal = -99e+20;
  FOR (a, cn.getNumActions()) {
    MDPQEntry& Qa = cn.Q[a];
    ubVal = 0;
    FOR (o, Qa.getNumOutcomes()) {
      MDPEdge* e = Qa.outcomes[o];
      if (NULL != e) {
	MDPNode& sn = *e->nextState;
	double oprob = e->obsProb;
	ubVal += oprob * sn.ubVal;
      }
    }
    ubVal = Qa.immediateReward + problem->getDiscount() * ubVal;
    Qa.ubVal = ubVal;

    if (ubVal > maxUBVal) {
      maxUBVal = ubVal;
      maxUBAction = a;
    }
  }
  cn.ubVal = std::min(cn.ubVal, maxUBVal);

  if (NULL != maxUBActionP) *maxUBActionP = maxUBAction;
}

void RTDP::trialRecurse(MDPNode& cn, double pTarget, int depth)
{
  // check for termination
  if (cn.isTerminal) {
#if USE_DEBUG_PRINT
    printf("trialRecurse: depth=%d ubVal=%g terminal node (terminating)\n",
	   depth, cn.ubVal);
#endif
    return;
  }

  // update to ensure cached values in cn.Q are correct
  int maxUBAction;
  update(cn, &maxUBAction);

  // simulate outcome
  double r = unit_rand();
  int simulatedOutcome = 0;
  MDPQEntry& Qbest = cn.Q[maxUBAction];
  FOR (o, Qbest.getNumOutcomes()) {
    MDPEdge* e = Qbest.outcomes[o];
    if (NULL != e) {
      r -= e->obsProb;
      if (r <= 0) {
	simulatedOutcome = o;
	break;
      }
    }
  }

#if USE_DEBUG_PRINT
  printf("  trialRecurse: depth=%d a=%d o=%d ubVal=%g\n",
	 depth, maxUBAction, simulatedOutcome, cn.ubVal);
  printf("  trialRecurse: s=%s\n", sparseRep(cn.s).c_str());
#endif

  // recurse to successor
  MDPNode& bestSuccessor = *cn.Q[maxUBAction].outcomes[simulatedOutcome]->nextState;
  double pNextTarget = pTarget / problem->getDiscount();
  trialRecurse(bestSuccessor, pNextTarget, depth+1);

  update(cn, NULL);
}

void RTDP::doTrial(MDPNode& cn, double pTarget)
{
#if USE_DEBUG_PRINT
  printf("-*- doTrial: trial %d\n", (numTrials+1));
#endif

  trialRecurse(cn, pTarget, 0);
  numTrials++;
}

}; // namespace zmdp

/***************************************************************************
 * REVISION HISTORY:
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2006/02/10 20:14:33  trey
 * standardized fields in bounds.plot
 *
 * Revision 1.2  2006/02/10 19:33:32  trey
 * chooseAction() now relies on upper bound as it should (since the lower bound is not even calculated in vanilla RTDP!
 *
 * Revision 1.1  2006/02/09 21:59:04  trey
 * initial check-in
 *
 *
 ***************************************************************************/
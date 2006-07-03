/********** tell emacs we use -*- c++ -*- style comments *******************
 $Revision: 1.3 $  $Author: trey $  $Date: 2006-07-03 21:20:21 $
   
 @file    genTargetLocationsl.cc
 @brief   No brief

 Copyright (c) 2006, Trey Smith. All rights reserved.

 Licensed under the Apache License, Version 2.0 (the "License"); you may
 not use this file except in compliance with the License.  You may
 obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 implied.  See the License for the specific language governing
 permissions and limitations under the License.

 ***************************************************************************/

/***************************************************************************
 * INCLUDES
 ***************************************************************************/

#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <getopt.h>

#include <iostream>

#include "LSModelFile.h"

using namespace std;
using namespace zmdp;

static double unitRand(void)
{
  return ((double) rand()) / RAND_MAX;
}

void usage(const char* binaryName) {
  cerr <<
    "usage: " << binaryName << " OPTIONS <foo.lifeSurvey>\n"
    "  -h or --help   Display this help\n"
    "  -s or --seed   Set random seed\n"
    "  -n or --noise  Set noise factor\n";
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  static char shortOptions[] = "hs:n:";
  static struct option longOptions[]={
    {"help",          0,NULL,'h'},
    {"seed",          1,NULL,'s'},
    {"noiseFactor",         1,NULL,'n'},
    {NULL,0,0,0}
  };

  int seed = -1;
  double noiseFactor = 0;
  while (1) {
    char optchar = getopt_long(argc,argv,shortOptions,longOptions,NULL);
    if (optchar == -1) break;

    switch (optchar) {
    case 'h': // help
      usage(argv[0]);
      break;

    case 's': // seed
      seed = atoi(optarg);
      break;

    case 'n': // noise
      noiseFactor = atof(optarg);
      break;

    case '?': // unknown option
    case ':': // option with missing parameter
      // getopt() prints an informative error message
      cerr << endl;
      usage(argv[0]);
      break;
    default:
      abort(); // never reach this point
    }
  }
  if (argc-optind != 1) {
    cerr << "ERROR: wrong number of arguments (should be 1)" << endl << endl;
    usage(argv[0]);
  }

  const char* modelFileName = argv[optind++];

  if (-1 == seed) {
    seed = time(NULL) % RAND_MAX;
  }
  printf("noise=%lf seed=%d\n", noiseFactor, seed);
  srand(seed);

  LSModelFile m;

  // read the map in
  m.readFromFile(modelFileName);

  // add noise to priors (if any)
  std::vector<double> noisyPriors = m.regionPriors;
  if (noiseFactor > 0) {
    for (int r=0; r < (int)noisyPriors.size(); r++) {
      double noise = (2 * unitRand() * noiseFactor) - noiseFactor;
      noisyPriors[r] *= (1.0 + noise);
    }
  }

  // set locations of targets
  for (int y=0; y < (int)m.grid.height; y++) {
    for (int x=0; x < (int)m.grid.width; x++) {
      unsigned char r = m.grid.getCell(LSPos(x,y));
      if (LS_OBSTACLE != r) {
	double prob = noisyPriors[r];
	bool hasTarget = (unitRand() < prob);
	m.grid.setCell(LSPos(x,y),hasTarget);
      }
    }
  }

  // write the resulting target map back out to stdout
  m.grid.writeToFile(stdout, /* binaryMap = */ true);
}


/***************************************************************************
 * REVISION HISTORY:
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2006/06/27 16:04:40  trey
 * refactored so outside code can access the LifeSurvey model using -lzmdpLifeSurvey
 *
 * Revision 1.2  2006/06/26 21:34:18  trey
 * now use zmdp namespace
 *
 * Revision 1.1  2006/06/16 14:45:20  trey
 * added ability to generate target map
 *
 *
 ***************************************************************************/
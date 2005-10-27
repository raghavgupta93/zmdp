/********** tell emacs we use -*- c++ -*- style comments *******************
 * $Revision: 1.1 $  $Author: trey $  $Date: 2005-10-27 21:27:10 $
 *  
 * @file    pomdpCassandraWrapper.h
 * @brief   A wrapper that provides access to the pomdp read in by
 *          Tony Cassandra's file reading code, without requiring you
 *          to (1) directly reference global variables or (2) include
 *          all of the headers like sparse-matrix.h.  Also uses notation
 *          I am more familiar with.
 ***************************************************************************/

#ifndef INCpomdpCassandraWrapper_h
#define INCpomdpCassandraWrapper_h

using namespace std;

#include <string>

// Exception class
class InputError { };

typedef int StateType;
typedef int ActionType;
typedef int ObsType;
typedef double ProbType;
typedef double ValueType;

struct PomdpCassandraWrapper {
  PomdpCassandraWrapper(void) {}

  int getNumStates(void) const;
  int getNumActions(void) const;
  int getNumObservations(void) const;
  ValueType getDiscount(void) const;
  ValueType getInitialBelief(StateType s) const;
  bool isTerminalState(StateType s) const;

  // rewards
  ValueType R(StateType s, ActionType a) const;

  // transition probabilities
  ProbType T(StateType s, ActionType a, StateType sp) const;

  // observation probabilities
  ProbType O(StateType sp, ActionType a, ObsType o) const;

  void readFromFile(const string& fileName);
};


#endif // INCpomdpCassandraWrapper_h

/***************************************************************************
 * REVISION HISTORY:
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2004/11/09 16:18:56  trey
 * imported hsvi into new repository
 *
 * Revision 1.3  2003/07/16 16:12:44  trey
 * added support for terminal states
 *
 * Revision 1.2  2003/04/02 17:13:56  trey
 * minor changes to enable compilation under gcc 3.0.1
 *
 * Revision 1.1.1.1  2003/01/07 19:19:41  trey
 * Imported sources
 *
 *
 ***************************************************************************/
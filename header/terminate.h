#ifndef __terminate_h__
#define __terminate_h__

#include "connector.h"

class Terminate : public Connector {
 private:
 	//TODO: Determine if data members are needed.
 public:
 	Terminate(Base*, Base*);
	bool execute();
};

#endif
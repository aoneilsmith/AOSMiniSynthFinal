#pragma once
#include "oscillator.h"

class CLFO : public COscillator
{
public:
	CLFO(void);
	~CLFO(void);
	
	// virtual overrides
	virtual void reset();
	virtual void startOscillator();
	virtual double doOscillate();
	virtual void stopOscillator();
};

/*
	CCombFilter: implements a comb filter of length D with
				 feedback gain m_fComb_g

	Can be used alone or as a base class.


	Copyright (c) 2010 Will Pirkle
	Free for academic use.
*/

// Inherited Base Class functions:
/*
	void init(int nDelayLength);
	void resetDelay();
	void setDelay_mSec(float fmSec);
	void setOutputAttenuation_dB(float fAttendB);

	// NEED TO OVERRIDE
	bool processAudio(float* pInput, float* pOutput);
*/
#pragma once
#include "delay.h"


// derived class: CDelay does most of the work
class CCombFilter : public CDelay
{
public:
	// constructor/destructor
	CCombFilter(void);
	~CCombFilter(void);

	// members
protected:
	float m_fComb_g; // one coefficient

public:
	// set our g value directly
	void setComb_g(float fCombg){m_fComb_g = fCombg;}

	// set gain using RT60 time
	void setComb_g_with_RTSixty(float fRT)
	{
		float fExponent = -3.0*m_fDelayInSamples*(1.0/m_nSampleRate);
		fRT /= 1000.0; // RT is in mSec!

		m_fComb_g = pow((float)10.0, fExponent/fRT);
	}

	// do some audio processing
	bool processAudio(float* pInput, float* pOutput);
};

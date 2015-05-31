#pragma once
#include "oscillator.h"

class CAlgorithmicOscillator : public COscillator
{
public:
	CAlgorithmicOscillator(void);
	~CAlgorithmicOscillator(void);

	// PolyBLEP
	double doSawtooth(double dModulo, double dInc);
	double doSquare(double dModulo, double dInc, bool* pRisingEdge);
	double doHardSyncSaw(bool bWrap);
	
	// DPW
	double doTriangle(double dModulo, double dInc, double dFo, double dSquareModulator, double* pZ_register);

	// virtual overrides
	virtual void reset();
	virtual void startOscillator();
	virtual void stopOscillator();

	virtual double doOscillate();
};

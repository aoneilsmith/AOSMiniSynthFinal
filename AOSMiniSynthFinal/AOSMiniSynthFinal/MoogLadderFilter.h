#pragma once
#include "filter.h"
#include "VAOnePoleFilter.h"

class CMoogLadderFilter : public CFilter
{
public:
	CMoogLadderFilter(void);
	~CMoogLadderFilter(void);

	CVAOnePoleFilter m_LPF1;
	CVAOnePoleFilter m_LPF2;
	CVAOnePoleFilter m_LPF3;
	CVAOnePoleFilter m_LPF4;

	// -- CFilter Overrides --
	virtual void reset();
	virtual void setQControl(double dQControl);
	virtual void updateFilter();
	virtual double doFilter(double xn);

	// variables
	double m_dK;		// K, set with Q
	double m_dGamma;	// see block diagram
	double m_dAlpha_0;	// see block diagram

	// Oberheim Xpander variations
	double m_dA;
	double m_dB;
	double m_dC;
	double m_dD;
	double m_dE;
};

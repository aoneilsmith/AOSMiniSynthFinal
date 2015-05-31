#pragma once
#include "filter.h"
#include "VAOnePoleFilter.h"

class CDiodeLadderFilter : public CFilter
{
public:
	CDiodeLadderFilter(void);
	~CDiodeLadderFilter(void);

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
	double m_dK;			// K, set with Q
	double m_dGamma;		// needed for final calc and update
	double m_dSG1; 
	double m_dSG2; 
	double m_dSG3; 
	double m_dSG4; 
};

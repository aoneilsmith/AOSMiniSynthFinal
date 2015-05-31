#pragma once

#include "pluginconstants.h"
#include "fastapprox.h"
#include "synthfunctions.h"

// CFilter Abastract Base Class for all filters
class CFilter
{
public:
	CFilter(void);
	~CFilter(void);

protected:
	// common variables
	double m_dSampleRate;	/* sample rate*/

	// Fc is modulatable, so we need a variable to store both
	// the current fc value but also the control's location 
	// as modulation is based off of the control
	double m_dFc;			/* cutoff frequency variable */
	double m_dFcControl;	/* cutoff frequency control position */

	// our cutoff frequency modulation input; 
	double m_dFcMod;		/* modulation input -1 to +1 */
	double m_dFcEGMod;		/* modulation input for EG 0 to +1 */
	double m_dFcLowLimit;	/* low limit on fc */
	double m_dFcHighLimit;	/* high limit on fc */
	
	double m_dFcModIntensity;/* control the Mod intensity -1 to +1*/
	double m_dEGModIntensity;/* control the EG intensity -1 to +1*/

	// Q is not modulatable (it's a Challenge in the book)
	double m_dQ;				/* Q */

	// for an aux filter specific like SEM BSF control
	double m_dAuxControl;

	// for NLP - Non Linear Procssing
	UINT m_uNLP;
	enum{OFF,ON}; 

	// for more grit
	double m_dSaturation;

	// NOTE: these are shared; even though some filters won't use some of them
	//       need to maintain the indexing
	/* filter selection */
	UINT m_uFilterType;		

	/* one short string for each */
	enum{LPF1,HPF1,LPF2,HPF2,BPF2,BSF2,LPF4,HPF4,BPF4};

public:
	// mutators
	//
	inline void setFcMod(double d){m_dFcMod = d;}
	inline void setFcEGMod(double d){m_dFcEGMod = d;}
	inline void setFcControl(double d){m_dFcControl = d;}
	inline void setSampleRate(double d){m_dSampleRate = d;}
	inline void setAuxControl(double d){m_dAuxControl = d;}
	inline void setNLP(UINT u){m_uNLP = u;}
	inline void setSaturation(double d){m_dSaturation = d;}
	inline void setFilterType(UINT u){m_uFilterType = u;}
	inline void setFcModIntensity(double d){m_dFcModIntensity = d;}
	inline void setEGModIntensity(double d){m_dEGModIntensity = d;}

	// flush buffers, reset filter
	virtual void reset();

	// decode the Q value
	virtual void setQControl(double dQControl);

	// recalculate the coeffs
	virtual void updateFilter();

	// pure virtual; derived classes MUST implement
	virtual double doFilter(double xn) = 0;
};

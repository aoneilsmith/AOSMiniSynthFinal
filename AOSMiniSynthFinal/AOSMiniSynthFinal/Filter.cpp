#include "Filter.h"

CFilter::CFilter(void)
{
	// defaults
	m_dSampleRate = 44100;

	m_dFc = 1000.0;
	m_dFcControl = 1000.0;

	// NOTE: 10 octaves
	m_dFcLowLimit = 20.0;
	m_dFcHighLimit = 20480;
	m_dFcMod = 0.0;
	m_dEGModIntensity = 0.0;
	m_dFcModIntensity = 0.0;
	m_dQ = 0.707;
	m_dAuxControl = 0.5;
	m_uNLP = OFF;
	m_dSaturation = 1.0;
}

CFilter::~CFilter(void)
{
}

// the base class will apply the modulation
void CFilter::updateFilter()
{
	// add the modulation sources together
	double dModTotal = m_dFcModIntensity*m_dFcMod + m_dEGModIntensity*m_dFcEGMod;

	// Exponential modulationn of Fc for filters
	// last arg true = the user is adjusting a volt/octave exponential control
	m_dFc = calcModulatedValueVoltOctaveExp(m_dFcLowLimit, m_dFcHighLimit, m_dFcControl, dModTotal, true);

	// other modulation (e.g. Q) here
	// 
}

// flush buffers
void CFilter::reset()
{
	// do nothing
}

// optional depending on filter type
void CFilter::setQControl(double dQControl)
{
	// do nothing
}

#include "EnvelopeGenerator.h"

CEnvelopeGenerator::CEnvelopeGenerator(void)
{	
	m_dAttackTime_mSec = 1000.0;
	m_dDecayTime_mSec = 1000.0;
	m_dReleaseTime_mSec = 1000.0;
	
	m_dSustainLevel = 0.707;
	m_dSampleRate = 0.0;
	m_dEnvelopeOutputLin = 0.0;
	m_dEnvelopeOutputExp = 0.0;

	m_dLinearIncAttack = 0.0;
	m_dLinearIncDecay = 0.0;
	m_dLinearIncRelease = 0.0;
	m_dLinearIncShutdown = 0.0;
	m_dExpoIncShutdown = 0.0;
	
	// user normally not allowed to set the time, this initilizes it
	m_dShutdownTime_mSec = 5.0;

	// states and flags
	m_uState = off;
	m_uEGMode = analog;
	setEGMode(m_uEGMode);
	m_bResetToZero = false;
	m_bLegatoMode = false; // change this! 
}

CEnvelopeGenerator::~CEnvelopeGenerator(void)
{
}

void CEnvelopeGenerator::setEGMode(UINT u)
{
	// save it
	m_uEGMode = u;

	// analog - use e^-5x
	if(m_uEGMode == analog)
	{
		// these are the e^-5x over/undershoot values
		// which will generate matching e^-5x curves
		m_dAttackTCO = exp(-5.0);  
		m_dDecayTCO = m_dAttackTCO; 
		m_dReleaseTCO = m_dDecayTCO; 
	}
	else
	{
		// digital is linear-in-dB so use
		// the -96dB noise floor for over/undershoot
		m_dAttackTCO = pow(10.0, -96.0/20.0);
		m_dDecayTCO = m_dAttackTCO;
		m_dReleaseTCO = m_dDecayTCO; 
	}
	/*
		NOTE: for Attack/Release only EGs, set these to give a smooth
		      charge/discharge curve since there is no Sustain bais
			  voltage on the cap:
		m_dDecayTCO = 0.99632;
		m_dReleaseTCO = 0.00368; 
	*/

	// recalc these
	calculateAttackTime();
	calculateDecayTime();
	calculateReleaseTime();
}

// returns time in samples
int CEnvelopeGenerator::calculateLinearAttackSamples(double dAttackStart)
{
	double dSamples = m_dSampleRate*(m_dAttackTime_mSec/1000.0);

	if(dAttackStart == 0.0)
		return dSamples/m_dSampleRate; // full range

	if(dAttackStart == 1.0)
		return 0.0; // no more attack time

	// find the e^exponent (-5 for analog)
	double dExp = -log((1.0 + m_dAttackTCO)/m_dAttackTCO);

	// find the distance throught the attack time
	double dMultiplier = log(-(dAttackStart - m_dAttackTCO - 1.0))/dExp;

	// multiply by samples
	double dSamps = dMultiplier*dSamples;

	return (int)dSamps;
}



double CEnvelopeGenerator::calculateExpoDecayTime()
{
	double dSamples = m_dSampleRate*(m_dDecayTime_mSec/1000.0);

	if(m_dSustainLevel == 0.0)
		return dSamples/m_dSampleRate;
	else if (m_dSustainLevel == 1.0)
		return 0.0;
	
	// ratio of natural logs
	double dDcySamples = dSamples - log(1.0 - m_dSustainLevel)/log(m_dDecayCoeff);
	
	if(dDcySamples > dSamples)
		int t=0;
	if(dDcySamples < 0)
		int t=0;

	// clamp (not observed)
	dDcySamples = min(dDcySamples, dSamples);
	dDcySamples = max(dDcySamples, 0.0);

	// return seconds
	return dDcySamples/m_dSampleRate;
}

double CEnvelopeGenerator::calculateExpoReleaseTime(double dReleasePoint)
{
	double dSamples = m_dSampleRate*(m_dReleaseTime_mSec/1000.0);

	if(dReleasePoint == 0.0)
		return 0.0;
	else if (dReleasePoint == 1.0)
		return dSamples/m_dSampleRate;
	
	// ratio of natural logs
	double dRelSamples = dSamples - log(dReleasePoint)/log(m_dReleaseCoeff);
	
	if(dRelSamples > dSamples)
		int t=0;
	if(dRelSamples < 0)
		int t=0;

	// clamp (not observed)
	dRelSamples = min(dRelSamples, dSamples);
	dRelSamples = max(dRelSamples, 0.0);

	// return seconds
	return dRelSamples/m_dSampleRate;
}

// reset
void CEnvelopeGenerator::reset()
{
	// state
	m_uState = off;
		
	// reset 
	setEGMode(m_uEGMode);

	// may be modified in noteOff()
	calculateReleaseTime();
	
	// if reset to zero, clear cap voltage
	// else let it stay frozen
	if(m_bResetToZero)
	{
		m_dEnvelopeOutputExp = 0.0;
		m_dEnvelopeOutputLin = 0.0;
	} 
	else if(m_dEnvelopeOutputExp != 0)
	{
		// samples for the exponential rate
		double dSamples = m_dSampleRate*(m_dAttackTime_mSec/1000.0);

		// calculate the exact time for this curve
		int nAttackSamples = calculateLinearAttackSamples(m_dEnvelopeOutputExp);

		m_dEnvelopeOutputLin = nAttackSamples*m_dLinearIncAttack;
	}
}
	
void CEnvelopeGenerator::calculateAttackTime()
{
	if(m_dAttackTime_mSec == 0)
	{
		m_dLinearIncAttack = 0.0;
		return;
	}

	// samples for the exponential rate
	double dSamples = m_dSampleRate*(m_dAttackTime_mSec/1000.0);

	// coeff and base for iterative exponential calculation
	m_dAttackCoeff = exp(-log((1.0 + m_dAttackTCO)/m_dAttackTCO)/dSamples);	
	m_dAttackOffset = (1.0 + m_dAttackTCO)*(1.0 - m_dAttackCoeff);
	
	// attack time from 0 -> 1.0
	m_dLinearIncAttack = 1000.0/m_dAttackTime_mSec/m_dSampleRate; 
}

// changes to the decay rate OR the sustain level require
// an update in analog mode
void CEnvelopeGenerator::calculateDecayTime()
{
	// not needed for time = 0
	if(m_dDecayTime_mSec == 0)
	{
		m_dLinearIncDecay = 0.0;
		return;
	}
	
	// samples for the exponential rate
	double dSamples = m_dSampleRate*(m_dDecayTime_mSec/1000.0);
	
	// coeff and base for iterative exponential calculation
	m_dDecayCoeff = exp(-log((1.0 + m_dDecayTCO)/m_dDecayTCO)/dSamples);
	m_dDecayOffset = (m_dSustainLevel - m_dDecayTCO)*(1.0 - m_dDecayCoeff);

	// calculate the exact time for this curve
	double dDecayTime_Sec = calculateExpoDecayTime();

	// calculate the inc for linear output
	m_dLinearIncDecay = (m_dSustainLevel - 1.0)/dDecayTime_Sec/m_dSampleRate;
}

void CEnvelopeGenerator::calculateReleaseTime()
{
	// not needed for time = 0
	if(m_dReleaseTime_mSec == 0)
	{
		m_dLinearIncRelease = 0.0;
		return;
	}
	
	// samples for the exponential rate
	double dSamples = m_dSampleRate*(m_dReleaseTime_mSec/1000.0);

	// coeff and base for iterative exponential calculation
	m_dReleaseCoeff = exp(-log((1.0 + m_dReleaseTCO)/m_dReleaseTCO)/dSamples);
	m_dReleaseOffset = -m_dReleaseTCO*(1.0 - m_dReleaseCoeff);	
	
	// calculate the exact time for this curve
	double dReleaseTime_Sec = calculateExpoReleaseTime(m_dSustainLevel);
	
	// calculate the inc for linear output
	m_dLinearIncRelease = -m_dSustainLevel/dReleaseTime_Sec/m_dSampleRate;
}

// go to release state; reset 
void CEnvelopeGenerator::noteOff()
{
	// need to recalc if we get here before sustain
	if(m_uState != sustain)
	{
		if(m_dReleaseTime_mSec == 0.0 || m_dSustainLevel == 0.0)
		{
			m_dLinearIncRelease = 0.0;
		}
		else // only need to calc linear output inc, expo is OK
		{
			// samples for the exponential rate
			double dSamples = m_dSampleRate*(m_dReleaseTime_mSec/1000.0);

			// calculate the exact time for this curve
			double dReleaseTime_Sec = calculateExpoReleaseTime(1.0 - m_dEnvelopeOutputExp);
			
			// calculate the inc for linear output
			m_dLinearIncRelease = -m_dEnvelopeOutputLin/dReleaseTime_Sec/m_dSampleRate;
		}
	}

	// go directly to release state
	if(m_dEnvelopeOutputExp > 0)
		m_uState = release;
	else // sustain was already at zero
		m_uState = off;
}

// goto shutdown state
void CEnvelopeGenerator::shutDown()
{
	// legato mode - ignore
	if(m_bLegatoMode)
		return;
	
	// calculate the linear inc values based on current outputs
	m_dLinearIncShutdown = -(1000.0*m_dEnvelopeOutputLin)/m_dShutdownTime_mSec/m_dSampleRate;
	m_dExpoIncShutdown = -(1000.0*m_dEnvelopeOutputExp)/m_dShutdownTime_mSec/m_dSampleRate;

	// set state and reset counter
	m_uState = shutdown;
}


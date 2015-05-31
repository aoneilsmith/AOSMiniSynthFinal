#pragma once
#include "synthfunctions.h"

class CEnvelopeGenerator
{
public:
	CEnvelopeGenerator(void);
	~CEnvelopeGenerator(void);

protected:
	double m_dSampleRate;
	int ncounter;

	// Coefficient, offset and TCO values
	// for each state
	double m_dAttackCoeff;
	double m_dAttackOffset;
	double m_dAttackTCO;

	double m_dDecayCoeff;
	double m_dDecayOffset;
	double m_dDecayTCO;

	double m_dReleaseCoeff;
	double m_dReleaseOffset;
	double m_dReleaseTCO;

	// ADSR times from user
	double m_dAttackTime_mSec;	// att: is a time duration
	double m_dDecayTime_mSec;	// dcy: is a time to decay 1->0
	double m_dReleaseTime_mSec;	// rel: is a time to decay 1->0
	
	// this is set internally; user normally not allowed to adjust
	double m_dShutdownTime_mSec; // shutdown is a time

	// sustain is a level, not a time
	double m_dSustainLevel;

	// current and last outputs, linear
	double m_dEnvelopeOutputLin;

	// and exponential
	double m_dEnvelopeOutputExp;

	// inc values for linear counter
	double m_dLinearIncAttack;
	double m_dLinearIncDecay;
	double m_dLinearIncRelease;
	double m_dLinearIncShutdown;
	double m_dExpoIncShutdown;

	// special modes
	bool m_bResetToZero; // return to zero
	bool m_bLegatoMode;  // S-trigger 

	// stage variable
	UINT m_uState;
	enum {off, attack, decay, sustain, release, shutdown};

	// analog and digital mode
	UINT m_uEGMode;
	enum {analog, digital};

public:
	// accessor - allow owner to get our state
	inline UINT getState() {return m_uState;}

	// mutators
	inline void setState(UINT u){m_uState = u;}
	inline void setSampleRate(double d){m_dSampleRate = d;}
	inline void setResetToZero(bool b){m_bResetToZero = b;}
	inline void setLegatoMode(bool b){m_bLegatoMode = b;}
	inline bool getLegatoMode(){return m_bLegatoMode;}

	// user variables from GUI
	inline void setAttackTime_mSec(double d)
	{
		m_dAttackTime_mSec = d;
		calculateAttackTime();
	}
	inline void setDecayTime_mSec(double d)
	{
		m_dDecayTime_mSec = d;
		calculateDecayTime();
	}
	inline void setReleaseTime_mSec(double d)
	{
		m_dReleaseTime_mSec = d;
		calculateReleaseTime();
	}
	inline void setShutdownTime_mSec(double d)
	{
		m_dShutdownTime_mSec = d;
	}
	// sustain is a level not a time
	inline void setSustainLevel(double d)
	{
		m_dSustainLevel = d;
		
		// sustain level affects decay
		calculateDecayTime();
		
		// and release, if not in release state
		if(m_uState != release)
			calculateReleaseTime();
	}

	// reset and go to attack state
	inline void startEG()
	{
		// ignore
		if(m_bLegatoMode && m_uState != off && m_uState != release)
			return;

		// reset and go
		reset();
		m_uState = attack;
	}

	// go to off state
	inline void stopEG()
	{
		m_uState = off;
	}

	// set analog or digital
	void setEGMode(UINT u);

	// reset
	void reset();

	// calculate the decay or release time based on the exponential rate ans sustain level
	double calculateExpoDecayTime();
	double calculateExpoReleaseTime(double dReleasePoint);

	// calculate the linear attack in samples for a given Exp output (for retriggering)
	int calculateLinearAttackSamples(double dAttackStart);
	
	// calculate both the exponential and linear inc values
	void calculateAttackTime();
	void calculateDecayTime();
	void calculateReleaseTime();

	// go to release state; reset counter
	void noteOff();

	// goto shutdown state
	void shutDown();

/* do the envelope

	double& dLinEnvOut = linear output passed by reference
	double& dExpEnvOut = exponential output passed by reference

*/
inline void doEnvelope(double& dLinEnvOut, double& dExpEnvOut)
{
	switch(m_uState)
	{
		case off:
		{
			// reset everything
			reset();
			break;
		}
		case attack:
		{
			// EXPONENTIAL output dictates cap voltage and state-change points
			m_dEnvelopeOutputExp = m_dAttackOffset + m_dEnvelopeOutputExp*m_dAttackCoeff;
		
			// LINEAR output
			m_dEnvelopeOutputLin += m_dLinearIncAttack; 
			m_dEnvelopeOutputLin = min(m_dEnvelopeOutputLin, 1.0);

			// check go to next state
			if(m_dEnvelopeOutputExp >= 1.0 || m_dAttackTime_mSec <= 0.0)
			{
				m_dEnvelopeOutputLin = 1.0;
				m_dEnvelopeOutputExp = 1.0;
				m_uState = decay;	// go to next state	
				break;
			}

			break;
		}
		case decay:
		{
			// EXPONENTIAL output dictates cap voltage and state-change points
			m_dEnvelopeOutputExp = m_dDecayOffset + m_dEnvelopeOutputExp*m_dDecayCoeff;
		
			// LINEAR output
			m_dEnvelopeOutputLin += m_dLinearIncDecay;
			m_dEnvelopeOutputLin = max(m_dEnvelopeOutputLin, m_dSustainLevel);

			// go to next state if wrapped
			if(m_dEnvelopeOutputExp <= m_dSustainLevel || m_dDecayTime_mSec <= 0.0)
			{
				m_dEnvelopeOutputLin = m_dSustainLevel;
				m_dEnvelopeOutputExp = m_dSustainLevel;
				m_uState = sustain;		// go to next state
				break;
			}

			break;
		}
		case sustain:
		{
			// nothing to do, just set level
			m_dEnvelopeOutputLin = m_dSustainLevel;
			m_dEnvelopeOutputExp = m_dSustainLevel;

			break;
		}
		case release:
		{
			// EXPONENTIAL output dictates cap voltage and state-change points
			m_dEnvelopeOutputExp = m_dReleaseOffset + m_dEnvelopeOutputExp*m_dReleaseCoeff;
		
			// LINEAR output
			m_dEnvelopeOutputLin += m_dLinearIncRelease;
			m_dEnvelopeOutputLin = max(m_dEnvelopeOutputLin, 0.0);

			// go to next state if wrapped
			if(m_dEnvelopeOutputExp <= 0.0 || m_dReleaseTime_mSec <= 0.0)
			{
				m_dEnvelopeOutputLin = 0.0; 
				m_dEnvelopeOutputExp = 0.0; 
				m_uState = off;		// go to next state
				break;
			}
			break;
		}	
		case shutdown:
		{
			if(m_bResetToZero)
			{
				// the shutdown state is just a linear taper since it is so short
				m_dEnvelopeOutputLin += m_dLinearIncShutdown;
				m_dEnvelopeOutputExp += m_dExpoIncShutdown;

				// go to next state 
				if(m_dEnvelopeOutputLin <= 0 || m_dEnvelopeOutputExp <= 0)
				{
					m_uState = off;		// go to next state
					m_dEnvelopeOutputLin = 0.0; // reset envelope
					m_dEnvelopeOutputExp = 0.0; // reset envelope
					break;
				}
			}
			else
			{
				// we are guaranteed to be retriggered
				// just go to off state
				m_uState = off;	
			}
			break;
		}
	}

	// set the outputs
	dLinEnvOut = m_dEnvelopeOutputLin;
	dExpEnvOut = m_dEnvelopeOutputExp;
}

};
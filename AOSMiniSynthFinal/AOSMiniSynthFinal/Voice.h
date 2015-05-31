#pragma once
#include "synthfunctions.h"

class CVoice
{
public:
	CVoice(void);
	virtual ~CVoice(void);
	// our state; may be different from Oscillator states
	bool m_bNoteOn;

protected:
	double m_dSampleRate;

	// this will be different for the voices; usually
	// octave/fifths etc...
	UINT m_uVoiceMode;

	UINT m_uMIDINoteNumber;
	UINT m_uMIDINoteNumberPending;
	
	UINT m_uMIDIVelocity;
	UINT m_uMIDIVelocityPending;
	
	double m_dOscPitch;
	double m_dOscPitchPending;

	double m_dOscPitchPortamentoStart;
	double m_dModuloPortamento;
	double m_dPortamentoInc;
	double m_dPortamentoSemitones;

	bool m_bNotePending;
	


	// enum for EG state variable so we can query the EG	
	enum {off, attack, decay, sustain, release, shutdown};
	
	// for LFOs
	enum {sine,usaw,dsaw,tri,square,expo,rsh,qrsh};

	// for PITCHED Oscillators
	enum {SINE,SAW1,SAW2,SAW3,HSSAW,TRI,SQUARE,NOISE,PNOISE};
	
	// for NLP and other ON/OFF switches
	enum{OFF,ON}; 

	// for EG mode
	enum {analog, digital};

public:
	virtual void setSampleRate(double dSampleRate){m_dSampleRate = dSampleRate;}

	virtual void prepareForPlay();

	virtual void update();
	
	virtual void reset();

	virtual void doVoice(double& dLeftOutput, double& dRightOutput) = 0;

};

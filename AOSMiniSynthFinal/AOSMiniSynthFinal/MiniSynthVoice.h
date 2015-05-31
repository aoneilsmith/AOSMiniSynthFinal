#pragma once

#include "voice.h"
#include "AlgorithmicOscillator.h"
#include "DCA.h"
#include "DiodeLadderFilter.h"
#include "EnvelopeGenerator.h"
#include "LFO.h"
#include "MoogLadderFilter.h"
#include "StereoDelay.h"
#include "DLLModule.h"


class CMiniSynthVoice : public CVoice
{
public:
	CMiniSynthVoice(void);
	~CMiniSynthVoice(void);

protected:
	// our synth components
	//
	// Three oscillators
	CAlgorithmicOscillator m_Osc1;
	CAlgorithmicOscillator m_Osc2;
	CAlgorithmicOscillator m_Osc3;
	
	// 1 filter
	// Korg Volca had a diode filter variation
	// CDiodeLadderFilter m_LPF;
	// but I like the Moog ladder
	CMoogLadderFilter m_LPF;

	// 2 LFOs	
	CLFO m_LFO;
	CLFO m_LFO2;

	// 1 EG
	CEnvelopeGenerator m_EG1;
	
	// 1 DCA
	CDCA m_DCA;



	//Volca Modes for pitched oscillators (plus a few more)
	enum {Unison,Octave,Third,Fifth,Sixth,Seventh,UniRing};
	//NLP type
	UINT m_uNLP;// on/off
	enum {arraya,arctan,sigmoid};
	double m_dSaturation;
	UINT m_uNLP_Type;

	//

public:

	// --- Methods from the GUI
	inline void setVoiceMode(UINT u){m_uVoiceMode = u;}
	inline void setNLP(UINT u){m_uNLP = u;} //nlp on/off
	inline void setNLPType(UINT u){m_uNLP_Type = u;} //type of nlp
	inline void setSaturation(double d){m_dSaturation = d;} //saturation
	inline void setLegatoMode(bool b){m_EG1.setLegatoMode(b);}
	inline void setResetToZero(bool b){m_EG1.setResetToZero(b);}
	inline void setLFOWaveform(UINT u){m_LFO.setWaveform(u);}
	inline void setLFO2Waveform(UINT u){m_LFO2.setWaveform(u);} //nlp, delay lfo


	inline void setPoratmentoTime_mSec(double dTime)
	{
		if(dTime == 0.0)
			m_dPortamentoInc = 0.0;
		else
			m_dPortamentoInc = 1000.0/dTime/m_dSampleRate;
	}

	// --- Call Forwarding Messages to OSCILLATORS
	inline void setOctave(int n)
	{
		m_Osc1.setOctave(n);
		m_Osc2.setOctave(n);
		m_Osc3.setOctave(n);
	}

	inline void setDetuneCents(double dDetuneCents)
	{
		// make very thick by detuning in offsets
		m_Osc1.setDetuneCents(dDetuneCents);
		m_Osc2.setDetuneCents(-dDetuneCents);
		m_Osc3.setDetuneCents(dDetuneCents/2.0);
	}

	inline void setEGIntensity_VCO(double dIntensity)
	{
		m_Osc1.setFoEgModIntensity(dIntensity);
		m_Osc2.setFoEgModIntensity(dIntensity);
		m_Osc3.setFoEgModIntensity(dIntensity);
	}

	inline void setLFOIntensity_VCO(double dIntensity)
	{
		// only one source of Mod Intensity
		m_Osc1.setFoModIntensity(dIntensity);
		m_Osc2.setFoModIntensity(dIntensity);
		m_Osc3.setFoModIntensity(dIntensity);
	}
	
	inline void setPitchBendModRange(double d)
	{
		m_Osc1.setPitchBendModRange(d);
		m_Osc2.setPitchBendModRange(d);
		m_Osc3.setPitchBendModRange(d);
	}

	inline void setPitchBendMod(double d)
	{
		m_Osc1.setPitchBendMod(d);
		m_Osc2.setPitchBendMod(d);
		m_Osc3.setPitchBendMod(d);
	}
	//nlp sat
	inline void setSaturationIntensity(double dIntensity)
	{
		m_Osc1.setFoModIntensity(dIntensity);
		m_Osc2.setFoModIntensity(dIntensity);
		m_Osc3.setFoModIntensity(dIntensity);
	}
	// --- Call Forwarding Messages to FILTER(s)
	inline void setLPFCutoff(double dFc)
	{
		m_LPF.setFcControl(dFc);
	}

	inline void setLPFQ(double dQ)
	{
		m_LPF.setQControl(dQ);
	}
	
	inline void setEGIntensity_LPF(double dIntensity)
	{
		m_LPF.setEGModIntensity(dIntensity);
	}

	inline void setLFORate(double dRate)
	{
		m_LFO.setOscFo(dRate);
	}
	//NLP
	inline void setLFO2Rate(double dRate)
	{
		m_LFO2.setOscFo(dRate);
	}
	//delay
	/*inline void setDelayLFO2(double dRate)
	{
		m_LFO.set
	}
	inline void setDelayEG1(double dIntensity)
	{
		m_EG1.(dIntensity);
	}*/

/*
	inline void setNLP(double dOn)
	{
		m_uNLP.setN
	}
	inline void setNLP_Type(d
	{

	}
	*/

	inline void setLFOIntensity_VCF(double dIntensity)
	{
		m_LPF.setFcModIntensity(dIntensity);
	}
	
	// --- Call Forwarding Messages to EG(s)
	inline void setAttackTime_mSec(double d)
	{
		m_EG1.setAttackTime_mSec(d);
	}
	
	inline void setDecayReleaseTime_mSec(double d)
	{
		m_EG1.setDecayTime_mSec(d);
		m_EG1.setReleaseTime_mSec(d);
	}

	inline void setSustainLevel(double d)
	{
		m_EG1.setSustainLevel(d);
	}
	
	// --- Call Forwarding Messages to DCA
	inline void setDCAAmplitude_dB(double d)
	{
		m_DCA.setAmplitude_dB(d);
	}

	// --- MIDI methods
	inline void noteOn(UINT uMIDINote, UINT uMIDIVelocity, double dFrequency, double dLastNoteFrequency)
	{
		m_dOscPitch = dFrequency;
		m_dOscPitchPortamentoStart = dFrequency;

		// is our voice avaialble?
		if(!m_bNoteOn && !m_bNotePending)
		{
			// save the note number (for voice steal later)
			m_uMIDINoteNumber = uMIDINote;
			
			// the DCA needs this
			m_uMIDIVelocity = uMIDIVelocity;
			
			// set the velocity info 
			m_DCA.setMIDIVelocity(m_uMIDIVelocity);

			// set the OSC
			m_Osc1.setOscFo(m_dOscPitch);
			m_Osc2.setOscFo(m_dOscPitch);
			m_Osc3.setOscFo(m_dOscPitch);

			// start; NOTE this will reset and update()
			m_Osc1.startOscillator();
			m_Osc2.startOscillator();
			m_Osc3.startOscillator();

			// start EG
			m_EG1.startEG();

			// start Ocsillator
			m_LFO.startOscillator();
			m_LFO2.startOscillator();

			// we are rendering!
			m_bNoteOn = true;

			// bail
			return;
		}

		// IF we get here, we are playing a note and need to steal it
		// Save PENDING note number and velocity
		m_uMIDINoteNumberPending = uMIDINote;
			
		m_dOscPitchPending = dFrequency;
		m_uMIDIVelocityPending = uMIDIVelocity;
	
		// set the flag that we have a note pending
		m_bNotePending = true;

		// and shutdown the EG
		m_EG1.shutDown();
	}

	inline void noteOff(UINT uMIDINoteNumber)
        {
                if(m_bNoteOn)
                {
                        // are we already in Release or Shutdown or Off?
                        if(m_EG1.getState() == release || m_EG1.getState() == shutdown || m_EG1.getState() == off)
                                return;

                        if(m_bNotePending && (uMIDINoteNumber == m_uMIDINoteNumberPending))
                        {
                                m_bNotePending = false;
                                return;
                        }

                        if(uMIDINoteNumber != m_uMIDINoteNumber)
                                return;
                        
                        m_EG1.noteOff();
                       // m_EG2.noteOff();
                }
        }

	// --- Overrides
	virtual void prepareForPlay();
	virtual void setSampleRate(double dSampleRate);
	virtual void update();
	virtual void reset();
	virtual void doVoice(double& dLeftOutput, double& dRightOutput);


};

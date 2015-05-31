/*
	RackAFX(TM)
	Applications Programming Interface
	Derived Class Object Definition
	Copyright(c) Tritone Systems Inc. 2006-2012

	Your plug-in must implement the constructor,
	destructor and virtual Plug-In API Functions below.
*/

#pragma once

// base class
#include "plugin.h"
#include "MiniSynthVoice.h"
//polyphony
#include <vector>
#define MAX_VOICES 4

// abstract base class for RackAFX filters
class CMiniSynth : public CPlugIn
{
public:
	// RackAFX Plug-In API Member Methods:
	// The followung 5 methods must be impelemented for a meaningful Plug-In
	//
	// 1. One Time Initialization
	CMiniSynth();

	// 2. One Time Destruction
	virtual ~CMiniSynth(void);

	// 3. The Prepare For Play Function is called just before audio streams
	virtual bool __stdcall prepareForPlay();

	// 4. processAudioFrame() processes an audio input to create an audio output
	virtual bool __stdcall processAudioFrame(float* pInputBuffer, float* pOutputBuffer, UINT uNumInputChannels, UINT uNumOutputChannels);

	// 5. userInterfaceChange() occurs when the user moves a control.
	virtual bool __stdcall userInterfaceChange(int nControlIndex);


	// OPTIONAL ADVANCED METHODS ------------------------------------------------------------------------------------------------
	// These are more advanced; see the website for more details
	//
	// 6. initialize() is called once just after creation; if you need to use Plug-In -> Host methods
	//				   such as sendUpdateGUI(), you must do them here and NOT in the constructor
	virtual bool __stdcall initialize();

	// 7. joystickControlChange() occurs when the user moves a control.
	virtual bool __stdcall joystickControlChange(float fControlA, float fControlB, float fControlC, float fControlD, float fACMix, float fBDMix);

	// 8. process buffers instead of Frames:
	// NOTE: set m_bWantBuffers = true to use this function
	virtual bool __stdcall processRackAFXAudioBuffer(float* pInputBuffer, float* pOutputBuffer, UINT uNumInputChannels, UINT uNumOutputChannels, UINT uBufferSize);

	// 9. rocess buffers instead of Frames:
	// NOTE: set m_bWantVSTBuffers = true to use this function
	virtual bool __stdcall processVSTAudioBuffer(float** inBuffer, float** outBuffer, UINT uNumChannels, int inFramesToProcess);

	// 10. MIDI Note On Event
	virtual bool __stdcall midiNoteOn(UINT uChannel, UINT uMIDINote, UINT uVelocity);

	// 11. MIDI Note Off Event
	virtual bool __stdcall midiNoteOff(UINT uChannel, UINT uMIDINote, UINT uVelocity, bool bAllNotesOff);


	// 12. MIDI Modulation Wheel uModValue = 0 -> 127
	virtual bool __stdcall midiModWheel(UINT uChannel, UINT uModValue);

	// 13. MIDI Pitch Bend
	//					nActualPitchBendValue = -8192 -> 8191, 0 is center, corresponding to the 14-bit MIDI value
	//					fNormalizedPitchBendValue = -1.0 -> +1.0, 0 is at center by using only -8191 -> +8191
	virtual bool __stdcall midiPitchBend(UINT uChannel, int nActualPitchBendValue, float fNormalizedPitchBendValue);

	// 14. MIDI Timing Clock (Sunk to BPM) function called once per clock
	virtual bool __stdcall midiClock();


	// 15. all MIDI messages -
	// NOTE: set m_bWantAllMIDIMessages true to get everything else (other than note on/off)
	virtual bool __stdcall midiMessage(unsigned char cChannel, unsigned char cStatus, unsigned char cData1, unsigned char cData2);

	// 16. initUI() is called only once from the constructor; you do not need to write or call it. Do NOT modify this function
	virtual bool __stdcall initUI();



	// Add your code here: ----------------------------------------------------------- //
	//CMiniSynthVoice m_Voice;

	// Polyphony Step: 2
	std::vector<CMiniSynthVoice*> m_VoicePtrStack;
	std::vector<CMiniSynthVoice*>::iterator m_VoiceIterator;



	void updateVoice();

	// for portamento
	double m_dLastNoteFrequency;

		//for stereo delay part
		// Left and Right Delay Elements
	CDLLModule m_DDL_Left;
	CDLLModule m_DDL_Right;

	void setDelayVariables(bool bCook);

	float m_fDelayInSamples;
	float m_fFeedback;
	float m_fWetLevel;
	
	float* m_pBuffer;
	int m_nReadIndex;
	int m_nWriteIndex;
	int m_nBufferSize;

	bool m_bUseExternalFeedback; //flag for enabling/disabling
	float m_fFeedbackIn; //the user supplied feedback sample value

	//current FB is fb*output
	float getCurrentFeedbackOutput(){return m_fFeedback*m_pBuffer[m_nReadIndex];}

	//set the feedback sample
	void setCurrentFeedbackInput(float f){m_fFeedbackIn=f;}

	//enable/disable external FB source
	void setUsesExternalFeedback(bool b){m_bUseExternalFeedback = false;}

	void cookVariables();
	void resetDelay();
		//function to calculate Shelving filter coefficients
	void calculateShelfCoeffs(float fLowGain, float fHighGain);
	
	CBiQuad m_LeftShelf;
	CBiQuad m_RightShelf;
	//delay
	//CLFO dLFO3;

	//float m_fMeterValue;

	// END OF USER CODE -------------------------------------------------------------- //


	// ADDED BY RACKAFX -- DO NOT EDIT THIS CODE!!! ----------------------------------- //
	//  **--0x07FD--**

	UINT m_uVoiceMode;
	enum{Unison,Octave,Third,Fifth,Sixth,Seventh,UniRing};
	double m_dDetune_cents;
	double m_dFcControl;
	double m_dRate_LFO;
	double m_dAttackTime_mSec;
	UINT m_uNLP;
	enum{OFF,ON};
	float m_fDelay_ms;
	float m_f_WetLevel_pct;
	int m_nOctave;
	double m_dQControl;
	double m_dLFOIntensity_VCO;
	double m_dDecayReleaseTime_mSec;
	double m_dRate_NLP_LFO;
	float m_fFeedback_pct;
	double m_dEGIntensity_VCO;
	double m_dEGIntensity_VCF;
	double m_dLFOIntensity_VCF;
	double m_dSustainLevel;
	double m_dSaturation;
	UINT m_uLFO_Waveform;
	enum{sine,usaw,dsaw,tri,square,expo,rsh,qrsh};
	UINT m_uNLP_Type;
	enum{arraya,arctan,sigmoid};
	double m_dPortamentoTime_mSec;
	double m_dAmplitude_dB;
	UINT m_uLegatoMode;
	enum{mono,legato};
	int m_nPitchBendRange;
	UINT m_uResetToZero;
	float m_fMeterValue;

	// **--0x1A7F--**
	// ------------------------------------------------------------------------------- //

};









































































































































































































































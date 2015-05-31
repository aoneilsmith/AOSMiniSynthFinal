#pragma once
#include "synthfunctions.h"

class CDCA
{
public:
	CDCA(void);
	~CDCA(void);

protected:
	// our internal gain variable
	double m_dGain;				

	// velocity input from MIDI keyboard
	UINT m_uMIDIVelocity; // 0 -> 127
	
	// MIDI CC Input Values
	UINT m_uCC7_Volume;	// CC7
	UINT m_uCC10_Pan;	// CC10

	// controls for user GUI (optional)
	double m_dAmplitude_dB;		// the user's control setting in dB
	double m_dAmplitudeControl;	// the user's control setting, converted from dB
	
	// pan control
	double m_dPanControl;	/* -1 to +1 == left to right */

	// modulate amplitude
	double m_dAmpMod;				/* modulation input for LFOs */
	double m_dAmpModIntensity_dB;	/* -dB (attenuation) only */

	// input to EGMod is EXPONENTIAL
	double m_dEGMod;		 /* modulation input for EG 0 to +1 */
	double m_dEGModIntensity;/*control the EG intensity -1 to +1 */

	// input to modulate pan control is bipolar
	double m_dPanMod;			/* modulation input for EG 0 to +1 */
	double m_dPanModIntensity;	/*control the EG intensity -1 to +1 */

public:
	// MIDI controller functions
	inline void setMIDIVelocity(UINT u){m_uMIDIVelocity = u;}
	inline void setMIDIVolume(UINT u){m_uCC7_Volume = u;}
	inline void setMIDIPan(UINT u){m_uCC10_Pan = u;}

	// expecting connection from bipolar source (LFO)
	// but this component will only be able to attenuate
	// so convert to unipolar 
	inline void setAmpMod(double d){m_dAmpMod = bipolarToUnipolar(d);}
	inline void setAmpModIntensity_dB(double d){m_dAmpModIntensity_dB = d;}

	// EG Mod Input Functions
	inline void setEGMod(double d){m_dEGMod = d;}
	inline void setEGModIntensity(double d){m_dEGModIntensity = d;}
	
	// Pan control and Mod Input Functions
	inline void setPanControl(double d){m_dPanControl = d;}
	inline void setPanMod(double d){m_dPanMod = d;}
	inline void setPanModIntensity(double d){m_dPanModIntensity = d;}

	inline void reset()
	{
		m_dEGMod = 0.0;
		m_dAmpMod = 0.0;
	}

	// -96dB to +12dB
	inline void setAmplitude_dB(double d)
	{
		m_dAmplitude_dB = d;
		m_dAmplitudeControl = pow((double)10.0, m_dAmplitude_dB/(double)20.0);
	}

	// recalculate gain value
	inline void updateDCA()
	{
		// check polarity
		if(m_dEGModIntensity >= 0)
			m_dGain = m_dEGMod*m_dEGModIntensity;
		else
			// unipolar inversion
			m_dGain = (1.0 - m_dEGMod)*m_dEGModIntensity;

		// amp mod is attenuation only, in dB
		// multiply raw gains (same as adding dB)
		m_dGain *= pow(10.0, (m_dAmpMod*m_dAmpModIntensity_dB)/(double)20.0);

		// use MMA MIDI->Atten (convex) transform
		m_dGain *= mmaMIDItoAtten(m_uMIDIVelocity);
		
		// use MMA MIDI->Atten (convex) transform
		m_dGain *= mmaMIDItoAtten(m_uCC7_Volume);
	}

	// do the DCA: uses pass-by-reference for outputs
	inline void doDCA(double dInput, double& dLeftOutput, double& dRightOutput)
	{
		// total pan value	
		double dPanTotal = m_dPanModIntensity*(m_dPanControl + m_dPanMod + midiToPanValue(m_uCC10_Pan));
		
		// limit in case pan control is biased
		dPanTotal = min(dPanTotal, 1.0);
		dPanTotal = max(dPanTotal, -1.0);

		double dPanLeft = 0.707;
		double dPanRight = 0.707;

		// equal power calculation in synthfunction.h
		calculatePanValues(dPanTotal, dPanLeft, dPanRight);

		// form left and right outputs
		dLeftOutput =  dPanLeft*m_dAmplitudeControl*dInput*m_dGain;
		dRightOutput =  dPanRight*m_dAmplitudeControl*dInput*m_dGain;
	}
};

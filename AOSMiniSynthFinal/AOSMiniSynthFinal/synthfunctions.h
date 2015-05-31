#pragma once
#include "fastapprox.h"
#include "pluginconstants.h"
#include "time.h"

#define CONVEX_LIMIT 0.00398107
#define CONCAVE_LIMIT 0.99601893

#define EXTRACT_BITS(the_val, bits_start, bits_len) ((the_val >> (bits_start - 1)) & ((1 << bits_len) - 1))

//----------------------------------------------------------------
// Constants
//----------------------------------------------------------------
// Status byte for Active Sensing message
const unsigned char ACTIVE_SENSING = 0xFE;

// Command value for Channel Pressure (Aftertouch)
const unsigned char CHANNEL_PRESSURE = 0xD0;

// Status byte for Continue message
const unsigned char CONTINUE = 0xFB;

// Command value for Control Change message
const unsigned char CONTROL_CHANGE = 0xB0;

// these are Data1 for CC
const unsigned char MOD_WHEEL = 0x01;

// Status byte for System Exclusive message
const unsigned char SYSTEM_EXCLUSIVE = 0xF0;

// Status byte for End of System Exclusive message
const unsigned char END_OF_EXCLUSIVE = 0xF7;

// Status byte for MIDI Time Code Quarter Fram message
const unsigned char MIDI_TIME_CODE = 0xF1;

// Command value for Note Off message
const unsigned char NOTE_OFF = 0x80;

// Command value for Note On message
const unsigned char NOTE_ON = 0x90;
const unsigned char ALL_NOTES_OFF = 0x7E;

// Command value for Pitch Bend message
const unsigned char PITCH_BEND = 0xE0;

// Command value for Polyphonic Key Pressure (Aftertouch)
const unsigned char POLY_PRESSURE = 0xA0;

// Command value for Program Change message
const unsigned char PROGRAM_CHANGE = 0xC0;

// Status byte for Song Position Pointer message
const unsigned char SONG_POSITION_POINTER = 0xF2;

// Status byte for MIDI Song Select message
const unsigned char SONG_SELECT = 0xF3;

// Status byte for Start message
const unsigned char START = 0xFA;

// Status byte for Stop message
const unsigned char STOP = 0xFC;

// Status byte for System Reset message
const unsigned char SYSTEM_RESET = 0xFF;

// Status byte for Timing Clock message
const unsigned char TIMING_CLOCK = 0xF8;

// Status byte for Tune Request message
const unsigned char TUNE_REQUEST = 0xF6;

/*
// unsigned char = BYTE
unsigned char decodeMIDIMessage(unsigned char cChannel, unsigned char cStatus, 
								unsigned char cData1, unsigned char cData2)
{
	if(cStatus == midi::NOTE_OFF || (cStatus == midi::NOTE_ON && cData2 == 0))
    {
        UINT uNote = cData1;
        UINT uVelocity = cData2;

    }
	else if(Command == midi::NOTE_ON && ShortMsg.GetData2() > 0)
    {
        UINT uNote = ShortMsg.GetData1();
        UINT uVelocity = ShortMsg.GetData2();
		if(m_pFilter)
		{
			if(m_pFilter->getPlugInBuddy())
				m_pFilter->getPlugInBuddy()->midiNoteOn(Channel, uNote, uVelocity);
			bDeliveredMessage = true;
		}

		sMIDI1 = "ntON";
		sMIDI2 = UINTToString(uNote);
		sMIDI3 = UINTToString(uVelocity);
		//sMIDI4 = "Ch" + UINTToString(Channel+1);
		sMIDI4 = "Ch" + UINTToString(Channel);
    }
	else if(Command == midi::TIMING_CLOCK) // timing clock
    {
		if(m_pFilter)
		{
			if(m_pFilter->getPlugInBuddy())
				m_pFilter->getPlugInBuddy()->midiClock();
			bDeliveredMessage = true;
		}

		//sMIDI1 = "ntON";
		//sMIDI2 = UINTToString(uNote);
		//sMIDI3 = UINTToString(uVelocity);
		////sMIDI4 = "Ch" + UINTToString(Channel+1);
		//sMIDI4 = "Ch" + UINTToString(Channel);
    }
    else if(Command == midi::CONTROL_CHANGE  && ShortMsg.GetData1() == midi::ALL_NOTES_OFF)
    {
        UINT uVelocity = ShortMsg.GetData2();
		if(m_pFilter)
		{
			if(m_pFilter->getPlugInBuddy())
			{
				for(int i=0; i<128; i++)
					m_pFilter->getPlugInBuddy()->midiNoteOff(Channel, i, uVelocity, true);
			bDeliveredMessage = true;
		}
		}
		sMIDI1 = "CC";
		sMIDI2 = "anOFF";
		sMIDI3 = UINTToString(uVelocity);
		//sMIDI4 = "Ch" + UINTToString(Channel+1);
		sMIDI4 = "Ch" + UINTToString(Channel);
    }
    else if(Command == midi::CONTROL_CHANGE  && ShortMsg.GetData1() == midi::MOD_WHEEL)
    {
        UINT uModValue = ShortMsg.GetData2();

		if(m_pFilter)
		{
			if(m_pFilter->getPlugInBuddy())
				m_pFilter->getPlugInBuddy()->midiModWheel(Channel, uModValue);
			bDeliveredMessage = true;
		}

		sMIDI1 = "CC";
		sMIDI2 = "MOD";
		sMIDI3 = UINTToString(uModValue);
		//sMIDI4 = "Ch" + UINTToString(Channel+1);
		sMIDI4 = "Ch" + UINTToString(Channel);
    }
    else if(Command == midi::PITCH_BEND)
    {
        UINT uLSB7 = ShortMsg.GetData1();
        UINT uMSB7 = ShortMsg.GetData2();
	
		unsigned short shValue = (unsigned short)uLSB7;  // 0xxx xxxx 
		unsigned short shMSPart = (unsigned short)uMSB7; // 0yyy yyyy 
		unsigned short shMSPartShift = shMSPart<<7;
		
		shValue = shValue | shMSPartShift;
		int nPitchBend = (int)shValue - 8192.0;
			
		int nNormPB = nPitchBend;
		if(nNormPB == -8192)
			nNormPB = -8191;

		float fPitchBend = (float)nNormPB/8191.0; // -1.0 -> 1.0
		
		OutputDebugString("PitchBend: " + IntToString(nPitchBend) + ", " + FloatToString(fPitchBend) + "\n");

		if(m_pFilter)
		{
			if(m_pFilter->getPlugInBuddy())
				m_pFilter->getPlugInBuddy()->midiPitchBend(Channel, nPitchBend, fPitchBend);
			bDeliveredMessage = true;
		}

		sMIDI1 = "PB";
		sMIDI2 = UINTToString(uLSB7);
		sMIDI3 = UINTToString(uMSB7);
		//sMIDI4 = "Ch" + UINTToString(Channel+1);
		sMIDI4 = "Ch" + UINTToString(Channel);
    }
	
//	if(!bDeliveredMessage)
	if(true) // this will repeat MIDI but need for consistency with AU/VST for synth book
	{
		UINT Channel = ShortMsg.GetChannel();
		UINT Command = ShortMsg.GetCommand();
		UINT Data1 = ShortMsg.GetData1();
		UINT Data2 = ShortMsg.GetData2();
			
		if(Command == midi::SYSTEM_EXCLUSIVE &&
			Data1 == 0 &&
			Data2 == 0)
			bIsSysExMIDIClock = true; //
		else
		{	
			if(Command == midi::CONTROL_CHANGE)
				sMIDI1 = "CC";
			else
				sMIDI1 = UINTToString(Command);
			
			sMIDI2 = UINTToString(Data1);
			sMIDI3 = UINTToString(Data2);
			//sMIDI4 = "Ch" + UINTToString(Channel+1);
			sMIDI4 = "Ch" + UINTToString(Channel);
		}

		if(m_pFilter && !bIsSysExMIDIClock)
		{	
			if(m_pFilter->getPlugInBuddy())
			{
				if(m_pFilter->getPlugInBuddy()->m_bWantAllMIDIMessages)
				{
					m_pFilter->getPlugInBuddy()->midiMessage(Channel, Command, Data1, Data2);
				}

				broadcastMIDIControl(Channel, Command, Data1, Data2);
			}
		}
		
}
*/

inline double semitonesBetweenFrequencies(double dStartFrequency, double dEndFrequency)
{
	return fastlog2(dEndFrequency/dStartFrequency)*12.0;
}

/* pitchShiftMultiplier()

	returns a multiplier for a given pitch shift in semitones
	to shift octaves,     call pitchShiftMultiplier(octaveValue*12.0);
	to shift semitones,   call pitchShiftMultiplier(semitonesValue);
	to shift cents,       call pitchShiftMultiplier(centsValue/100.0);
*/
inline double pitchShiftMultiplier(double dPitchShiftSemitones)
{
	// 2^(N/12)
	return fastpow2(dPitchShiftSemitones/12.0);
}

/* paramToTimeCents()

	converts a time parameter (secs) to TimeCents
	See the MMA MIDI DLS Level 1 or 2 Spec

*/
inline double paramToTimeCents(double dParam)
{
	return 1200.0*65536.0*fastlog2(dParam);
}

/* timeCentsToParam()
	
	converts a timeCents to a time (sec) value
	See the MMA MIDI DLS Level 1 or 2 Spec

*/
inline double timeCentsToParam(double dTimeCents)
{
	return fastpow2(dTimeCents/(1200.0*65536.0));
}

/* mmaMIDItoParamScaling()
	
	calculates the scaled version of a parameter based on a MIDI value
	See the MMA MIDI DLS Level 1 or 2 Spec
	
	uMIDIValue = the MIDI (0 -> 127) value to apply
	fParameter = the parameter to scale
	fScaling = the scaling amount, 0 -> 1.0

	Examples:
		Velocity to EG Attack Time: attack time decreases as velocity increases
									uMIDIValue = velocity
									fParameter = attack time (mSec)
									fScaling = Vel to EG Attack Scale

		Key to EG Decay Time: decay shortens as MIDI note number increases
							  uMIDIValue = MIDI note number
							  fParameter = decay time (mSec)
							  fScaling = Vel to EG Attack Scale


*/
inline double mmaMIDItoParamScaling(UINT uMIDIValue, float fParameter, float fScaling)
{
	return timeCentsToParam(paramToTimeCents(fScaling)*(double(uMIDIValue)/128.0) + paramToTimeCents(fParameter));
}

/* mmaMIDItoAtten_dB()
	
	calculates the dB of attenuation according to MMA DLS spec
	
	uMIDIValue = the MIDI (0 -> 127) value to convert
*/
inline double mmaMIDItoAtten_dB(UINT uMIDIValue)
{
	if(uMIDIValue == 0)
		return -96.0; // dB floor

	return 20.0*log10((127.0*127.0)/((float)uMIDIValue*(float)uMIDIValue));
}

/* mmaMIDItoAtten()
	
	calculates the raw attenuation according to MMA DLS spec
	
	uMIDIValue = the MIDI (0 -> 127) value to convert
*/
inline double mmaMIDItoAtten(UINT uMIDIValue)
{
	if(uMIDIValue == 0)
		return 0.0; // floor

	return ((double)uMIDIValue*(double)uMIDIValue)/(127.0*127.0);;
}

/* capChargeTransform()
	
	calculates the expDecayTransform of the input

	dValue = the unipolar (0 -> 1) value to convert

	returns a value from 0 up to 1.0
*/
inline double capChargeTransform(double dValue)
{
	if(dValue == 0.0)
		return 0.0;

	return 1.0 - exp(-5.0*dValue);
}

/* capDischargeTransform()
	
	calculates the capDischargeTransform of the input
	
	dValue = the unipolar (0 -> 1) value to convert
*/
inline double capDischargeTransform(double dValue)
{
	//if(dValue == 1.0)
	//	return 0.0;

	return exp(-5.0*dValue);
}


/* linearIn_dB_AttackTransform()
	
	calculates the linearIn_dB_AttackTransform of the input
	
	dValue = the unipolar (0 -> 1) value to convert
*/
inline double linearIn_dB_InverseAttackTransform(double dValue)
{
	if(dValue == 0.0)
		return 0.0;

	return pow(10.0, ((1.0 - dValue)*-96.0)/20.0);
}

/* linearIn_dB_InverseAttackTransform()
	
	calculates the linearIn_dB_InverseAttackTransform of the input
	
	dValue = the unipolar (0 -> 1) value to convert
*/
inline double linearIn_dB_AttackTransform(double dValue)
{
	if(dValue == 0.0)
		return 0.0;

	return 1.0 - pow(10.0, (dValue*-96.0)/20.0);
}

/* linearIn_dB_DecayTransform()
	
	calculates the linearIn_dB_DecayTransform of the input
	
	dValue = the unipolar (0 -> 1) value to convert
*/
inline double linearIn_dB_DecayTransform(double dValue)
{
	if(dValue == 1.0)
		return 0.0;

	return pow(10.0, (dValue*-96.0)/20.0);
}

/* convexTransform()
	
	calculates the convexTransform of the input
	
	dValue = the unipolar (0 -> 1) value to convert
*/
inline double convexTransform(double dValue)
{
	if(dValue <= CONVEX_LIMIT)
		return 0.0;

	return 1.0 + (5.0/12.0)*log10(dValue);
}

/* convexInvertedTransform()
	
	calculates the convexInvertedTransform of the input
	
	dValue = the unipolar (0 -> 1) value to convert
*/
inline double convexInvertedTransform(double dValue)
{
	if(dValue >= CONCAVE_LIMIT)
		return 0.0;

	return 1.0 + (5.0/12.0)*log10(1.0 - dValue);
}

/* concaveTransform()
	
	calculates the concaveTransform of the input
	
	dValue = the unipolar (0 -> 1) value to convert
*/
inline double concaveTransform(double dValue)
{
	if(dValue >= CONCAVE_LIMIT)
		return 1.0;

	return -(5.0/12.0)*log10(1.0 - dValue);
}

/* concaveInvertedTransform()
	
	calculates the concaveInvertedTransform of the input
	
	dValue = the unipolar (0 -> 1) value to convert
*/
inline double concaveInvertedTransform(double dValue)
{
	if(dValue <= CONVEX_LIMIT)
		return 1.0;

	return -(5.0/12.0)*log10(dValue);
}

/* unipolarToBipolar()
	
	calculates the bipolar (-1 -> +1) value from a unipolar (0 -> 1) value
	
	dValue = the value to convert
*/
inline double unipolarToBipolar(double dValue)
{
	return 2.0*dValue - 1.0;
}

/* midiToBipolar()
	
	calculates the bipolar (-1 -> +1) value from a unipolar MIDI (0 -> 127) value
	
	UINT uMIDIValue = the MIDI value to convert
*/
inline double midiToBipolar(UINT uMIDIValue)
{
	return 2.0*(double)uMIDIValue/127.0 - 1.0;
}

/* midiToPanValue()
	
	calculates the pan value (-1 -> +1) value from a unipolar MIDI (0 -> 127) value
	
	UINT uMIDIValue = the MIDI value to convert
*/
inline double midiToPanValue(UINT uMIDIValue)
{
	// see MMA DLS Level 2 Spec; controls are asymmetrical
	if(uMIDIValue == 64)
		return 0.0;
	else if (uMIDIValue <= 1) // 0 or 1
		return -1.0;

	return 2.0*(double)uMIDIValue/127.0 - 1.0;
}

/* bipolarToUnipolar()
	
	calculates the unipolar (0 -> 1) value from a bipolar (-1 -> +1) value
	
	dValue = the value to convert
*/
inline double bipolarToUnipolar(double dValue)
{
	return 0.5*dValue + 0.5;
}

/* midiToUnipolar()
	
	calculates the unipolar (0 -> 1) value from a MIDI (0 -> 127) value
	
	dValue = the value to convert
*/
inline double midiToUnipolar(UINT uMIDIValue)
{
	return (double)uMIDIValue/127.0;
}

/* calculatePanValues()
	
	calculates the left and right pan values
	
	dPanMod = bipolar (-1 -> 1) pan modulation value

	returns are via pass-by-reference mechanism
*/
inline void calculatePanValues(double dPanMod, double& dLeftPanValue, double& dRightPanValue)
{
	dLeftPanValue = fastcos((pi/4.0)*(dPanMod + 1.0));
	dRightPanValue = fastsin((pi/4.0)*(dPanMod + 1.0));

	dLeftPanValue = max(dLeftPanValue, 0.0);
	dLeftPanValue = min(dLeftPanValue, 1.0);
	dRightPanValue = max(dRightPanValue, 0.0);
	dRightPanValue = min(dRightPanValue, 1.0);
}

/* calcValueVoltOctaveExp()
	
	returns the volt/octave exponential value
	
	dLowLimit = low limit of control
	dHighLimit = high limit of control
	dControlValue = on the range of 0 -> 1
*/
inline double calcValueVoltOctaveExp(double dLowLimit, double dHighLimit, double dControlValue)
{
	double dOctaves = fastlog2(dHighLimit/dLowLimit);
	if(dLowLimit == 0)
		return dControlValue;
	
	// exp control
	return dLowLimit*fastpow2(dControlValue*dOctaves);
}

/* calcInverseValueVoltOctaveExp()
	
	returns a value between 0 -> 1 representing the volt/octave control location
	
	dLowLimit = low limit of control
	dHighLimit = high limit of control
	dControlValue = value between low and high limit
*/
inline double calcInverseValueVoltOctaveExp(double dLowLimit, double dHighLimit, double dControlValue)
{
	double dOctaves = fastlog2(dHighLimit/dLowLimit);
	if(dLowLimit == 0)
		return dControlValue;
	
	return fastlog2(dControlValue/dLowLimit)/dOctaves;
}

/* calcModulatedValueExp()
	
	returns the new exponentially modulated control value
	
	dLowLimit = low limit of control
	dHighLimit = high limit of control
	dControlValue = current value of control
	dModValue = moudulator: on the range of 0 -> 1
	bExpUserControl = type of control on GUI; TRUE if user is adjusting a volt/octave control FALSE if linear
*/
inline double calcModulatedValueVoltOctaveExp(double dLowLimit, double dHighLimit, double dControlValue, 
											  double dModValue, bool bExpUserControl)
{
	dModValue = bipolarToUnipolar(dModValue);

	double dOctaves = fastlog2(dHighLimit/dLowLimit);
	
	// location of control relative to 50% point
	double dControlOffset = bExpUserControl ? calcInverseValueVoltOctaveExp(dLowLimit, dHighLimit,dControlValue) - 0.5 : ((dControlValue - dLowLimit)/(dHighLimit - dLowLimit)) - 0.5;

	// exp control
	double dExpControlOffset = dLowLimit*fastpow2(dControlOffset*dOctaves);
	
	// mod exp control
	double dModulatedValueExp = dExpControlOffset*fastpow2(dModValue*dOctaves);

	// bound
	dModulatedValueExp = min(dModulatedValueExp, dHighLimit);
	dModulatedValueExp = max(dModulatedValueExp, dLowLimit);

	// return it
	return dModulatedValueExp;
}

/* calcModulatedValueLin()
	
	returns the new linearly modulated control value
	
	dLowLimit = low limit of control
	dHighLimit = high limit of control
	dControlValue = current value of control
	dModValue = moudulator: on the range of 0 -> 1
*/
inline double calcModulatedValueLin(double dLowLimit, double dHighLimit, double dControlValue, double dModValue)
{	
	// convert mod value to bipolar
	double dModulator = unipolarToBipolar(dModValue)*(dHighLimit - dLowLimit)/2.0;// + dLowLimit; 
	
	double dModulatedValue = dControlValue + dModulator;

	// bound
	dModulatedValue = min(dModulatedValue, dHighLimit);
	dModulatedValue = max(dModulatedValue, dLowLimit);

	// return it
	return dModulatedValue;
}

/* polyBLEP()
	
	returns the PolyBLEP residual for a given sample
	
	dModulo = modulo counter value (0->1)
	dInc = phase increment
	dHeight = height of discontinuity (0->1)
	bRisingEdge = true for rising edge, false for falling
	bDone = pass by reference flag telling caller PolyBLEP did the second sample and is done for this discont.
	dEdgeLocation = location of the discontinuity between 0 and 1; 
					for Sawtooth the edge is at 1.0, falling
					for Square the rising edge is at 1.0 and the falling edge is at 0.5
*/
inline double polyBLEP(double dModulo, double dInc, double dHeight, bool bRisingEdge, bool& bDone, double dEdgeLocation = 0.0)
{
	// bDone is a flag letting the caller know we processed 
	// the point on the right side of the discontinuity
	bDone = false;

	// return value
	double dPolyBLEP = 0.0;

	// t = the distance from the discontinuity
	double t = 0.0;
	
	// LEFT side of edge
	// -1 < t < 0
	if((dEdgeLocation == 0 && dModulo > 1.0 - dInc) || 
		((dModulo > dEdgeLocation - dInc) && dModulo < dEdgeLocation))
	{ 
		// for the wrap-around edge only
		if(dEdgeLocation == 0.0)
			dEdgeLocation = 1.0;
		
		// calculate distance
		t = (dModulo - dEdgeLocation)/dInc; 

		// calculate residual
		dPolyBLEP = dHeight*(t*t + 2.0*t + 1.0); 
//		dPolyBLEP = dHeight*(t*t/2.0 + t + 0.5); 
	
		// subtract for falling, add for rising edge
		if(!bRisingEdge)
			dPolyBLEP *= -1.0;

		return dPolyBLEP;
	} 

	// RIGHT side of discontinuity
	// 0 <= t < 1 
	if(dModulo < dEdgeLocation + dInc && dModulo > dEdgeLocation)
	{ 
		// set done flag
		bDone = true;

		// calculate distance
		t = (dModulo - dEdgeLocation)/dInc; 

		// calculate residual
		dPolyBLEP = dHeight*(2.0*t - t*t - 1.0); 
//		dPolyBLEP = dHeight*(t - t*t/2.0 - 0.5); 
	
		// subtract for falling, add for rising edge
		if(!bRisingEdge)
			dPolyBLEP *= -1.0;

		return dPolyBLEP;
	} 

	return 0.0; 
} 

inline double doWhiteNoise()
{
	// fNoise is 0 -> 32767.0
	float fNoise = (float)rand();

	// normalize and make bipolar
	fNoise = 2.0*(fNoise/32767.0) - 1.0;

	return fNoise;
}

inline double doPNSequence(UINT& uPNRegister)
{
	// get the bits
	UINT b0 = EXTRACT_BITS(uPNRegister, 1, 1); // 1 = b0 is FIRST bit from right
	UINT b1 = EXTRACT_BITS(uPNRegister, 2, 1); // 1 = b1 is SECOND bit from right
	UINT b27 = EXTRACT_BITS(uPNRegister, 28, 1); // 28 = b27 is 28th bit from right
	UINT b28 = EXTRACT_BITS(uPNRegister, 29, 1); // 29 = b28 is 29th bit from right

	// form the XOR
	UINT b31 = b0^b1^b27^b28;

	// form the mask to OR with the register to load b31
	if(b31 == 1)
		b31 = 0x10000000;

	// shift one bit to right
	uPNRegister >>= 1; 

	// set the b31 bit
	uPNRegister |= b31;

	// convert the output into a floating point number, scaled by experimentation
	// to a range of o to +2.0
	float fOut = (float)(uPNRegister)/((pow((float)2.0,(float)32.0))/16.0);

	// shift down to form a result from -1.0 to +1.0
	fOut -= 1.0;

	return fOut;
}

const double B = 4/pi;
const double C = -4/(pi*pi);
const double P = 0.225;
// http://devmaster.net/posts/9648/fast-and-accurate-sine-cosine
// input is -pi to +pi
inline double parabolicSine(double x, bool bHighPrecision = true)
{
    double y = B * x + C * x * abs(x);

    if(bHighPrecision)
        y = P * (y * abs(y) - y) + y;   

	return y;
}

const double D = 5.0*pi*pi;
inline double BhaskaraISine(double x)
{
	double sgn = x/abs(x);
	return 16.0*x*(pi - sgn*x)/(D - sgn*4.0*x*(pi - sgn*x));
}

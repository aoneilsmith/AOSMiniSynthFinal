#include "AlgorithmicOscillator.h"

CAlgorithmicOscillator::CAlgorithmicOscillator(void)
{
}

CAlgorithmicOscillator::~CAlgorithmicOscillator(void)
{
}

void CAlgorithmicOscillator::reset()
{	
	COscillator::reset();
	
	// saw/tri starts at 0.5
	if(m_uWaveform == SAW1 || m_uWaveform == SAW2 || 
	   m_uWaveform == SAW3 || m_uWaveform == HSSAW ||
	   m_uWaveform == TRI)
	{
		m_dModulo = 0.5;		
		m_dModulo2 = 0.5;		
	}

	// 303sqr
	m_bSquareEdgeRising = true;
	
	// update
	updateOscillator();
}

void CAlgorithmicOscillator::startOscillator()
{
	reset();
	m_bNoteOn = true;
}

void CAlgorithmicOscillator::stopOscillator()
{
	m_bNoteOn = false;
}



double CAlgorithmicOscillator::doSawtooth(double dModulo, double dInc)
{
	if(m_uWaveform == SAW1)			// SAW1 = normal sawtooth (ramp)
	{
		// tb303 saw
		double dOut = 0.0;
		double dTrivialSaw = 0.0;
		bool bDone = false;

		//if(dModulo <= 0.1)
		if(false)
			dOut = -1.0;
		else
		{
			//(1.0/0.9)*
	//		dTrivialSaw = unipolarToBipolar((1.0/0.9)*(dModulo - 0.1));
			double saw = 0.0;
			if(dModulo <= 0.2)
				saw = (0.04/0.2)*dModulo + 0;
			else
				saw = ((1.0 - 0.04)/0.8)*(dModulo - 0.2) + 0.04;

			//double saw = 0.0;
			//if(dModulo <= 0.1)
			//	saw = (0.03/0.1)*dModulo + 0;
			//else
			//	saw = ((1.0 - 0.03)/0.9)*(dModulo - 0.1) + 0.03;

		//	dTrivialSaw = unipolarToBipolar(dModulo);
			dTrivialSaw = unipolarToBipolar(saw);

			//// waveshape. arraya
			//if(dTrivialSaw >= 0.9)
			//{
			//	dTrivialSaw = 0.5*dTrivialSaw + 0.5*((3.0*dTrivialSaw/2.0)*(1.0 - dTrivialSaw*dTrivialSaw/3.0));
			////	dTrivialSaw = atan(3.0*dTrivialSaw);
			//	//dTrivialSaw = (3.0*dTrivialSaw/2.0)*(1.0 - dTrivialSaw*dTrivialSaw/3.0);
			//}

			if(m_uBLEP)
				dOut = -(dTrivialSaw + polyBLEP(dModulo, 
											  dInc,/* abs(dInc) is for FM synthesis with negative frequencies */
											  1.0,		/* sawtooth edge = 1.0 */
											  false,	/* falling edge */
											  bDone)); 
			else
				dOut = -dTrivialSaw;
		}

		return dOut;

	}
	else
	{
	double dTrivialSaw = 0.0;
	double dOut = 0.0;
	bool bDone = false;

	if(m_uWaveform == SAW1)			// SAW1 = normal sawtooth (ramp)
		dTrivialSaw = unipolarToBipolar(dModulo);
	else if(m_uWaveform == SAW2)	// SAW2 = one sided wave shaper
		dTrivialSaw = 2.0*(tanh(2.5*dModulo)/tanh(2.5)) - 1.0; 
	else if(m_uWaveform == SAW3)	// SAW3 = double sided wave shaper
	{
		dTrivialSaw = unipolarToBipolar(dModulo);
	//	dTrivialSaw = tanh(2.5*dTrivialSaw)/tanh(2.5);
		dTrivialSaw = 3.0*dTrivialSaw/2.0*(1.0 - (dTrivialSaw*dTrivialSaw*dTrivialSaw*dTrivialSaw/2.3));
	//	dTrivialSaw = 3.0*dTrivialSaw/2.0*(1.0 - (dTrivialSaw*dTrivialSaw*dTrivialSaw*dTrivialSaw*dTrivialSaw*dTrivialSaw/1.5));

	}

	// call the parabolicSine approximator
	if(m_uWaveform == SAW3)
		dOut = dTrivialSaw + polyBLEP(dModulo, 
									  abs(dInc),/* abs(dInc) is for FM synthesis with negative frequencies */
									  m_dEGModIntensity,		/* sawtooth edge = 1.0 */
									  false,	/* falling edge */
									  bDone); 
	else
		dOut = dTrivialSaw + polyBLEP(dModulo, 
									  abs(dInc),/* abs(dInc) is for FM synthesis with negative frequencies */
									  1.0,		/* sawtooth edge = 1.0 */
									  false,	/* falling edge */
									  bDone); 

	return dOut;
	}
}
	
double CAlgorithmicOscillator::doSquare(double dModulo, double dInc, bool* pRisingEdge)
{
	if(false)
	{
	// pulse width conversion from %
	double dPW = 0.3; //m_dPulseWidth/100.0;

	// trivial square
	double dTrivialSquare = dModulo <= dPW ? -1.0 : unipolarToBipolar(dModulo);

	// setup for PolyBLEP
	bool bDone = false;

	// loc = pw for rising edge, or 0 for falling edge
	 double dEdgeLocation = *pRisingEdge ? 0.3 : 0.0;

	 double dHeight = *pRisingEdge ? 0.479 : 0.828;
	
	 // call the function
	 double dOut = dTrivialSquare;
	 
	 float a,b,c,d,f,g;

	a = -4.4120279720284691E-02;
	b = 4.9007925407923931E+00;
	c = -3.5536687458431082E+01;
	d = 1.7135184456404264E+02;
	f = -3.2818455993250672E+02;
	g = 2.0690598862907100E+02;

	 float x = dModulo - 0.3;

//	 if(m_uBLEP && dModulo > 0.767)
	 if(m_uBLEP && dModulo > 0.3)
//	 if(m_uBLEP)
	 {
		//if(dModulo < 0.767)
		// //if(false)
		//	dTrivialSquare = ((1.0-0.479)/(0.767-0.3))*(dModulo-0.3) + 2.0*(0.479) - 1.0;
		// else
		// {
		//	dTrivialSquare = ((0.707 - 1.0)/(1.0 - 0.85))*(dModulo-0.85) + 1.0;
			dTrivialSquare = a + b*x + c*x*x + d*x*x*x + f*x*x*x*x + g*x*x*x*x*x;
		 //}
	 }
//		dTrivialSquare = convexTransform(dTrivialSquare);



	 if(true)
		dOut = dTrivialSquare + polyBLEP(dModulo, 
											dInc, 
											dHeight,	/* always 1.0 for square discont */
											*pRisingEdge, 
											bDone, 
											dEdgeLocation);

	// toggle the edge state
	if(bDone)
		*pRisingEdge = !*pRisingEdge;


	return dOut;
	}
	else if(false)
	{
	// pulse width conversion from %
	double dPW = 0.3; //m_dPulseWidth/100.0;

	// trivial square
	double dTrivialSquare = dModulo <= dPW ? -1.0 : unipolarToBipolar(dModulo);

	// setup for PolyBLEP
	bool bDone = false;

	// loc = pw for rising edge, or 0 for falling edge
	 double dEdgeLocation = *pRisingEdge ? 0.3 : 0.0;

	 double dHeight = *pRisingEdge ? 0.479 : 0.828;
	
	 // call the function
	 double dOut = dTrivialSquare;
	 
	 float a,b,c,d,f,g;

	a = -4.4120279720284691E-02;
	b = 4.9007925407923931E+00;
	c = -3.5536687458431082E+01;
	d = 1.7135184456404264E+02;
	f = -3.2818455993250672E+02;
	g = 2.0690598862907100E+02;

	 float x = dModulo - 0.3;

	a = 9.9929999999999963E-01;
	b = -3.4565789473684152E+00;
	c = 8.5699445983379139E+00;
	x = dModulo - 0.767;

//	 if(m_uBLEP && dModulo > 0.767)
	 if(m_uBLEP && dModulo > 0.3)
//	 if(m_uBLEP)
	 {
		if(dModulo < 0.767)
	//		dTrivialSquare = ((0.92+0.042)/(0.767-0.3))*(dModulo-0.3) - 0.042;
			dTrivialSquare = ((1.0+0.042)/(0.767-0.3))*(dModulo-0.3) - 0.042;
		 else
		 {
			// dTrivialSquare = ((0.707 - 1.0)/(1.0 - 0.85))*(dModulo-0.85) + 1.0;
			dTrivialSquare = a + b*x + c*x*x;// + d*x*x*x + f*x*x*x*x + g*x*x*x*x*x;
		 }
	 }
//		dTrivialSquare = convexTransform(dTrivialSquare);



	 if(true)
		dOut = dTrivialSquare + polyBLEP(dModulo, 
											dInc, 
											dHeight,	/* always 1.0 for square discont */
											*pRisingEdge, 
											bDone, 
											dEdgeLocation);

	// toggle the edge state
	if(bDone)
		*pRisingEdge = !*pRisingEdge;


	return dOut;
	}
	else if(true)
	{
	// pulse width conversion from %
	double dPW = 0.3; //m_dPulseWidth/100.0;

	// trivial square
	double dTrivialSquare = dModulo <= dPW ? -1.0 : unipolarToBipolar(dModulo);

	// setup for PolyBLEP
	bool bDone = false;

	// loc = pw for rising edge, or 0 for falling edge
	 double dEdgeLocation = *pRisingEdge ? 0.3 : 0.0;

	 double dHeight = *pRisingEdge ? 0.479 : 0.828;
	
	 // call the function
	 double dOut = dTrivialSquare;
	 
	 float a,b,c,d,f,g;

	a = -4.4120279720284691E-02;
	b = 4.9007925407923931E+00;
	c = -3.5536687458431082E+01;
	d = 1.7135184456404264E+02;
	f = -3.2818455993250672E+02;
	g = 2.0690598862907100E+02;

	 float x = dModulo - 0.3;

//	 if(m_uBLEP && dModulo > 0.767)
	 if(m_uBLEP && dModulo > 0.3)
//	 if(m_uBLEP)
	 {
		if(dModulo < 0.767)
			dTrivialSquare = ((1.0+0.042)/(0.767-0.3))*(dModulo-0.3) - 0.042;
		 else
		 {
			dTrivialSquare = ((0.656-1.0)/(1.0-0.767))*(dModulo-0.767) + 1.0;
		 }
	 }
//		dTrivialSquare = convexTransform(dTrivialSquare);



	 if(true)
		dOut = dTrivialSquare + polyBLEP(dModulo, 
											dInc, 
											dHeight,	/* always 1.0 for square discont */
											*pRisingEdge, 
											bDone, 
											dEdgeLocation);

	// toggle the edge state
	if(bDone)
		*pRisingEdge = !*pRisingEdge;


	return dOut;
	}


	if(false)
	{

	// pulse width conversion from %
	double dPW = m_dPulseWidth/100.0;

	// trivial square
	double dTrivialSquare = dModulo >= dPW ? -1.0 : +1.0;

	// setup for PolyBLEP
	bool bDone = false;

	// loc = 0 for rising edge, or pw for falling edge
	 double dEdgeLocation = *pRisingEdge ? 0.0 : dPW;

	// call the function
	double dOut = dTrivialSquare + polyBLEP(dModulo, 
											dInc, 
											1.0,	/* always 1.0 for square discont */
											*pRisingEdge, 
											bDone, 
											dEdgeLocation);

	// toggle the edge state
	if(bDone)
		*pRisingEdge = !*pRisingEdge;

	// need to correct for pulse-width
	//
	// 25% -> 0.667 amplitude(0.5/(1-0.25)), +0.333 DC Offset (1-amp)
	// 50% -> 1.0 amplitude, 0 DC offset
	// 75% -> 0.667 amplitude (0.5/0.75),    -0.333 DC Offset (1-amp)
	double dCorrection = 1.0;
	if(dPW < 0.5)
	{
		dCorrection = 0.5/(1.0 - dPW);
		dOut = dOut*dCorrection + (1.0 - dCorrection);
	}
	else if(dPW > 0.5)
	{
		dCorrection = 0.5/dPW;
		dOut = dOut*dCorrection - (1.0 - dCorrection);
	}

	return dOut;

	}
}

double CAlgorithmicOscillator::doTriangle(double dModulo, double dInc, double dFo, double dSquareModulator, double* pZ_register)
{
	double dOut = 0.0;
	bool bDone = false;

	// bipolar conversion and squaring
	double dBipolar = unipolarToBipolar(dModulo);
	double dSq = dBipolar*dBipolar;

	// inversion
	double dInv = 1.0 - dSq;

	// modulation with square modulo
	double dSqMod = dInv*dSquareModulator;
		
	// original
	double dDifferentiatedSqMod = dSqMod - *pZ_register;
	*pZ_register = dSqMod;

	// c = fs/[4fo(1-2fo/fs)]
	double c = m_dSampleRate/(4.0*2.0*dFo*(1-dInc));

	return dDifferentiatedSqMod*c;
}

double CAlgorithmicOscillator::doHardSyncSaw(bool bWrap)
{
	double dOut = 0.0;
	bool bDone = false;

	// reset detection
	bool bResetPending = false;
	bool bReset = false;

	// detect if reset is occurring now
	if(bWrap)
		bReset = true;
	// detect if reset will occur on the NEXT sample period
	else if (m_dModulo + m_dInc >= 1.0)
		bResetPending = true;

	// do the modulo wrap on slave osc
	checkWrapModulo2();

	// calculate the ratio
	double dRatio = m_dFo2/m_dFo;

	// move the slave modulo to the master location scaled by height
	if(bReset)
		m_dModulo2 = dRatio*m_dModulo;

	// unipolar to bipolar
	double dTrivialHS = unipolarToBipolar(m_dModulo2);

	if(bReset || bResetPending)
	{
		// height of transition
		double dHeight = 1.0;
		if(dRatio - (int)dRatio > 0)
			dHeight = dRatio - (int)dRatio;

		// use the master modulo because it is ultimately responsible for the 
		// transition location scale by height of discontinuity
		dOut = dTrivialHS + dHeight*polyBLEP(m_dModulo, m_dInc, dHeight, false, bDone);
	}
	else // test for the normal PolyBLEP transition
		dOut = dTrivialHS + polyBLEP(m_dModulo2, m_dInc2, 1.0, false, bDone); 

	return dOut;
}

double CAlgorithmicOscillator::doOscillate()
{
	if(!m_bNoteOn)
		return 0.0;

	double dOut = 0.0;
		
	// always first
	bool bWrap = checkWrapModulo();
	
	// need for triangle
	bool bWrap2 = false;

	if(m_bUnisonMode)
		bWrap2 = checkWrapModulo2();
	
	switch(m_uWaveform)
	{
		case SINE:
		{
			// calculate angle
			double dAngle = m_dModulo*2.0*pi - pi;

			// call the parabolicSine approximator
			dOut = parabolicSine(-dAngle);	

			// unison mode?
			if(m_bUnisonMode)
			{
				// calculate angle
				double dAngle2 = m_dModulo2*2.0*pi - pi;
				dOut = 0.5*dOut + 0.5*parabolicSine(-dAngle2);
			}
			
			break;
		}


		case SAW1:
		case SAW2:
		case SAW3:
		{
			// do first waveform
			dOut = doSawtooth(m_dModulo, m_dInc);

			// unison mode?
			if(m_bUnisonMode)
				dOut = 0.5*dOut + 0.5*doSawtooth(m_dModulo2, m_dInc2);
			
			break;
		}

		case HSSAW:
		{
			dOut = doHardSyncSaw(bWrap);
			break;
		}

		case SQUARE:
		{
			dOut = doSquare(m_dModulo, m_dInc, &m_bSquareEdgeRising);
			
			// unison mode?
			if(m_bUnisonMode)
				dOut = 0.5*dOut + 0.5*doSquare(m_dModulo2, m_dInc2, &m_bSquareEdgeRising2);

			break;
		}

		case TRI:
		{
			// do first waveform
			if(bWrap)
				m_dDPWSquareModulator *= -1.0;

			dOut = doTriangle(m_dModulo, m_dInc, m_dFo, m_dDPWSquareModulator, &m_dDPWz1_1);

			// unison mode?
			if(m_bUnisonMode)
			{
				if(bWrap2)
					m_dDPWSquareModulator2 *= -1.0;

				dOut = 0.5*dOut + 0.5*doTriangle(m_dModulo2, m_dInc2, m_dFo2, m_dDPWSquareModulator2, &m_dDPWz1_2);
			}
			
			break;
		}

		case NOISE:
		{
			// use helper function
			dOut = doWhiteNoise();
			break;
		}

		case PNOISE:
		{
			// use helper function
			dOut = doPNSequence(m_uPNRegister);
			break;
		}
		default:
			break;
	}
	
	// ok to inc modulo now
	incModulo();
	if(m_uWaveform == TRI)
		incModulo();

	// if unison mode or hard sync, inc the second one
	if(m_bUnisonMode || m_uWaveform == HSSAW)
	{
		incModulo2();
		if(m_uWaveform == TRI)
			incModulo2();
	}

	return dOut*m_dAmplitude;
}

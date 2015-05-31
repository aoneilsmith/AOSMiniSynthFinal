#include "LFO.h"

CLFO::CLFO(void)
{
	m_uLFOMode = sync;
}

CLFO::~CLFO(void)
{
}

void CLFO::reset()
{
	// call base class
	COscillator::reset();

	// call our update
	updateOscillator();
}

void CLFO::startOscillator()
{
	// if one shot or sync'd LFO, reset 
	if(m_uLFOMode == sync || m_uLFOMode == shot)
		reset();

	// set flag
	m_bNoteOn = true;
}

void CLFO::stopOscillator()
{
	// clear flag
	m_bNoteOn = false;
}

double CLFO::doOscillate()
{
	// silent if not on
	if(!m_bNoteOn)
		return 0.0;

	double dOut = 0.0;
		
	// always first
	bool bWrap =  checkWrapModulo();

	// one shot LFO? 
	if(m_uLFOMode == shot && bWrap)
	{
		m_bNoteOn = false;
		return 0.0;
	}

	// decode and calculate
	switch(m_uWaveform)
	{
		case sine:
		{
			// calculate angle
			double dAngle = m_dModulo*2.0*pi - pi;

			// call the parabolicSine approximator
			dOut = parabolicSine(-dAngle);	
			break;
		}

		case usaw:
		case dsaw:
		{		
			// unipolar to bipolar
			dOut = unipolarToBipolar(m_dModulo);

			// invert for downsaw
			if(m_uWaveform == dsaw)
				dOut *= -1.0;

			break;
		}

		case square:
		{
			// check pulse width and output either +1 or -1
			dOut = m_dModulo > m_dPulseWidth/100.0 ? -1.0 : +1.0;
			break;
		}

		case tri:
		{
			if(m_dModulo < 0.5) // out  = 0 -> +1
				dOut = 2.0*m_dModulo;
			else				// out = +1 -> 0
				dOut = 1.0 - 2.0*(m_dModulo - 0.5);

			// unipolar to bipolar
			dOut = unipolarToBipolar(dOut);
			break;
		}
		
		case expo:
		{
			// calculate the output directly
			dOut = concaveInvertedTransform(m_dModulo);
			break;
		}
		
		case rsh:
		case qrsh:
		{
			// this is the very first run
			if(m_nRSHCounter < 0)
			{
				if(m_uWaveform == rsh)
					m_dRSHValue = doWhiteNoise();
				else
					m_dRSHValue = doPNSequence(m_uPNRegister);

				m_nRSHCounter = 1.0;
			}
			// hold time exceeded?
			else if(m_nRSHCounter > m_dSampleRate/m_dFo)
			{
				m_nRSHCounter -= m_dSampleRate/m_dFo;
				
				if(m_uWaveform == rsh)
					m_dRSHValue = doWhiteNoise();
				else
					m_dRSHValue = doPNSequence(m_uPNRegister);
			}

			// inc the counter
			m_nRSHCounter += 1.0;

			// output held value
			dOut = m_dRSHValue;
			break;
		}


		default:
			break;
	}

	// ok to inc modulo now
	incModulo();

	return dOut*m_dAmplitude;
}
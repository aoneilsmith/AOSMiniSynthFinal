#include "MiniSynthVoice.h"
#include <math.h>

CMiniSynthVoice::CMiniSynthVoice(void)
{
}

CMiniSynthVoice::~CMiniSynthVoice(void)
{
}

void CMiniSynthVoice::setSampleRate(double dSampleRate)
{
	// save (needed?)
	m_dSampleRate = dSampleRate;

	m_Osc1.setSampleRate(dSampleRate);
	m_Osc2.setSampleRate(dSampleRate);
	m_Osc3.setSampleRate(dSampleRate);

	m_EG1.setSampleRate(dSampleRate);

	m_LFO.setSampleRate(dSampleRate);

	m_LPF.setSampleRate(dSampleRate);

}

void CMiniSynthVoice::prepareForPlay()
{
	reset();

	m_Osc1.setUnisonMode(true);
	m_Osc2.setUnisonMode(true);
	m_Osc3.setUnisonMode(true);

	m_LPF.setNLP(ON);

	m_EG1.setEGMode(analog);

}
void CMiniSynthVoice::reset()
{
	m_dPortamentoInc = 0.0;
	m_dSaturation = 0.0;

	m_Osc1.setWaveform(SAW2);
	m_Osc2.setWaveform(SAW2);
	m_Osc3.setWaveform(SAW2);

	m_LFO.setWaveform(tri);
	m_LFO2.setWaveform(tri);

	m_EG1.reset();
}

void CMiniSynthVoice::update()
{
	if(m_uVoiceMode == UniRing)
	{
		m_Osc3.setWaveform(SQUARE);
		m_Osc2.setSemitones(-8);
		m_Osc3.setSemitones(9);
	}
	else
	{
		m_Osc3.setWaveform(SAW2);
		m_Osc2.setSemitones(0);
		m_Osc3.setSemitones(0);
	}

	if(m_uVoiceMode == Octave)
		m_Osc3.setSemitones(12);
	else if(m_uVoiceMode == Third)
		m_Osc3.setSemitones(4);
	else if(m_uVoiceMode == Fifth)
		m_Osc3.setSemitones(7);
	else if(m_uVoiceMode == Sixth)
		m_Osc3.setSemitones(9);
	else if(m_uVoiceMode == Seventh)
		m_Osc3.setSemitones(10);
	else
		m_Osc3.setSemitones(0);

	m_Osc1.updateOscillator();
	m_Osc2.updateOscillator();
	m_Osc3.updateOscillator();

	m_LFO.updateOscillator();
	m_LFO2.updateOscillator();

	m_LPF.updateFilter();

	m_DCA.updateDCA();
}




void CMiniSynthVoice::doVoice(double& dLeftOutput, double& dRightOutput)
{
	// clear destinations
	dLeftOutput = 0.0;
	dRightOutput = 0.0;

	// bail if no note 
	if(!m_bNoteOn)
		return;

	// did EG finish? - its the flag for us as a voice
	if(m_EG1.getState() == off || m_bNotePending)
	{
		// did EG finish with NO note pending?
		if(m_EG1.getState() == off && !m_bNotePending)
		{
			// shut off and reset everything
			m_Osc1.stopOscillator();
			m_Osc2.stopOscillator();
			m_Osc3.stopOscillator();

			// need this in case of steal mode
			m_Osc1.reset();
			m_Osc2.reset();
			m_Osc3.reset();

			// stop the LFO
			m_LFO.stopOscillator();
			m_LFO2.stopOscillator();

			//m reset the EG
			//m_EG1.reset();

			// need to reset filter too??
			m_bNoteOn = false;

			// done
			return;
		}
		else if(m_bNotePending && (m_EG1.getState() == off || m_EG1.getLegatoMode()))// note pending so turn it on
		{
			// transfer informationn from PENDING values
			m_uMIDINoteNumber = m_uMIDINoteNumberPending;
			m_dOscPitch = m_dOscPitchPending;
			m_uMIDIVelocity = m_uMIDIVelocityPending;
			
			if(!m_EG1.getLegatoMode())
				// new velocity value
				m_DCA.setMIDIVelocity(m_uMIDIVelocity);

			double dPitch = m_dOscPitch;
			// hint portamento on dPitch
			dPitch = semitonesBetweenFrequencies(m_dOscPitch,m_dOscPitchPending);

			m_Osc1.setOscFo(dPitch);
			m_Osc2.setOscFo(dPitch);
			m_Osc3.setOscFo(dPitch);

			// update so new pitch is used
			m_Osc1.updateOscillator();
			m_Osc2.updateOscillator();
			m_Osc3.updateOscillator();

			// crank the EG back up
			m_EG1.startEG();

			// start Oscillator
			m_LFO.startOscillator();
			m_LFO2.startOscillator();

			// clear flag
			m_bNotePending = false;
		}
	}

	// layer 1 modulators
	double dLin, dExp;
	m_EG1.doEnvelope(dLin, dExp);

	double dLFO = m_LFO.doOscillate();
	double dLFO2 = m_LFO2.doOscillate();
	
	// apply modulatora
	m_DCA.setEGMod(dExp);
	m_DCA.updateDCA();

	m_LPF.setFcEGMod(dLin);
	m_LPF.setFcMod(dLFO);
	m_LPF.updateFilter();

	m_Osc1.setFoEgMod(dLin);
	m_Osc2.setFoEgMod(dLin);
	m_Osc3.setFoEgMod(dLin);

	m_Osc1.setFoModExp(dLFO);
	m_Osc2.setFoModExp(dLFO);
	m_Osc3.setFoModExp(dLFO);

	setSaturationIntensity(dLFO2);
	/*
	m_Osc1.setFoModIntensity(dLFO2);
	m_Osc2.setFoModIntensity(dLFO2);
	m_Osc3.setFoModIntensity(dLFO2);
	*/
	// ocillator is still running, so just update
	m_Osc1.updateOscillator();
	m_Osc2.updateOscillator();
	m_Osc3.updateOscillator();

	

//
//
	// do the voice
	double dOscMix = 0.0;
	double dOscMixNLP = 0.0;
	if(m_uVoiceMode == UniRing)
		dOscMix = m_Osc1.doOscillate()*m_Osc2.doOscillate()*m_Osc3.doOscillate();
/*	else if(m_uNLP == OFF || m_uNLP_Type == tanh )		
		{
			dOscMix = 0.33*m_Osc1.doOscillate() + 0.33*m_Osc2.doOscillate() + 0.33*m_Osc3.doOscillate();
//			dOscMix = tanh(dOscMixNLP);
		}	
	else if(m_uNLP == OFF || m_uNLP_Type == tanhkx )
		{
			dOscMixNLP = 0.33*m_Osc1.doOscillate() + 0.33*m_Osc2.doOscillate() + 0.33*m_Osc3.doOscillate();
			dOscMix = tanh*m_dSaturation*(dOscMixNLP);
		}*/
	else if(m_uNLP == OFF || m_uNLP_Type == arraya )
		{
			dOscMixNLP = 0.33*m_Osc1.doOscillate() + 0.33*m_Osc2.doOscillate() + 0.33*m_Osc3.doOscillate();
			dOscMix = ((3*dOscMixNLP)/2)*(1-((dOscMixNLP*dOscMixNLP)/3));
		}
	else if(m_uNLP == OFF || m_uNLP_Type == arctan)
		{
			dOscMixNLP = 0.33*m_Osc1.doOscillate() + 0.33*m_Osc2.doOscillate() + 0.33*m_Osc3.doOscillate();
			dOscMix = atan(m_dSaturation*dOscMixNLP)/atan(m_dSaturation);	
		}
	else if(m_uNLP == OFF || m_uNLP_Type == sigmoid )
		{
			dOscMixNLP = 0.33*m_Osc1.doOscillate() + 0.33*m_Osc2.doOscillate() + 0.33*m_Osc3.doOscillate();
			dOscMix = 2*(1/(1+exp(-(m_dSaturation)*dOscMixNLP)))-1;
		}
	else
		dOscMix = 0.33*m_Osc1.doOscillate() + 0.33*m_Osc2.doOscillate() + 0.33*m_Osc3.doOscillate();


	// apply the filter
	double dLPFOut = m_LPF.doFilter(dOscMix);

	// apply the DCA
	m_DCA.doDCA(dLPFOut, dLeftOutput, dRightOutput);



}

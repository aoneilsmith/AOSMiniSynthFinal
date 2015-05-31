#include "DCA.h"

CDCA::CDCA(void)
{
	m_dAmplitudeControl = 1.0;
	m_dGain = 1.0;
	m_dAmplitude_dB = 0.0;
	m_dEGMod = 1.0;
	m_dEGModIntensity = 1.0;
	m_dAmpMod = 1.0;
	m_dAmpModIntensity_dB = 0.0;

	m_dPanControl = 0.0;
	m_dPanModIntensity = 1.0;
	m_dPanMod = 0.0;

	m_uCC10_Pan = 64;
	m_uCC7_Volume = 127;
	m_uMIDIVelocity = 127;
}

CDCA::~CDCA(void)
{
}

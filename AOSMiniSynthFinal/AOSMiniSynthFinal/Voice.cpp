#include "Voice.h"

CVoice::CVoice(void)
{
	m_bNoteOn = false;
	m_bNotePending = false;
	m_dSampleRate = 44100;

	m_dOscPitch = 440.0;
	m_dOscPitchPending = 440.0;
	m_dOscPitchPortamentoStart = 440.0;
	
	m_dModuloPortamento = 0.0;
	m_dPortamentoInc = 0.0;
	m_dPortamentoSemitones = 0.0;
}

CVoice::~CVoice(void)
{
}

void CVoice::prepareForPlay()
{

}

void CVoice::update()
{

}
	
void CVoice::reset()
{

}

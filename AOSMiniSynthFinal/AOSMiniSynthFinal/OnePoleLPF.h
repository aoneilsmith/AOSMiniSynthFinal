/*
	COnePoleLPF: implements a one pole feedback LPF
				 the user controls the cutofff with
				 the coefficient g



	Copyright (c) 2010 Will Pirkle
	Free for academic use.
*/
#pragma once

// underflow protection
#ifndef FLT_MIN_PLUS 
	#define FLT_MIN_PLUS          1.175494351e-38         /* min positive value */
#endif
#ifndef FLT_MIN_MINUS 
	#define FLT_MIN_MINUS        -1.175494351e-38         /* min negative value */
#endif

class COnePoleLPF
{
public:
	// constructor/Destructor
	COnePoleLPF(void);
	~COnePoleLPF(void);

	// members
protected:
	float m_fLPF_g; // one gain coefficient
	float m_fLPF_z1; // one delay

public:

	// set our one and only gain coefficient
	void setLPF_g(float fLPFg){m_fLPF_g = fLPFg;}

	// function to init
	void init();

	// function to process audio
	bool processAudio(float* pInput, float* pOutput);

};

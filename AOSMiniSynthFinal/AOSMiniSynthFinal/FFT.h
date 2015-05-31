#pragma once
#define  PI 3.1415926535897932384626433832795
#include <math.h>
#include "windows.h"

class CFFT
{
public:
	CFFT(void);
	~CFFT(void);


	void fft_double(unsigned int p_nSamples, bool p_bInverseTransform,
					double *p_lpRealIn, double *p_lpImagIn,
					double *p_lpRealOut, double *p_lpImagOut);

	bool IsPowerOfTwo(unsigned int p_nX);
	unsigned int NumberOfBitsNeeded(unsigned int p_nSamples);
	unsigned int ReverseBits(unsigned int p_nIndex, unsigned int p_nBits);
	double Index_to_frequency(unsigned int p_nBaseFreq, unsigned int p_nSamples, unsigned int p_nIndex);
	double Index_to_frequency( unsigned NumSamples, unsigned Index );

};

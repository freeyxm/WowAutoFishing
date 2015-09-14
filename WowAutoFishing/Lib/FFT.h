#pragma once
#pragma execution_character_set("utf-8")
#include "stdafx.h"
#include <vector>
#include <bitset>
#include <complex>

class FFT
{
public:
	FFT();
	~FFT();

	static void DoFFT(unsigned long &ulN, std::vector<std::complex<double> >& vecList);
};


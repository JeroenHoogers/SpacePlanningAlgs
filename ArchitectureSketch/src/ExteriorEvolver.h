#pragma once
#include "Evolver.h"

class ExteriorEvolver : public Evolver
{
public:
	ExteriorEvolver();
	~ExteriorEvolver();

	void generate();
};


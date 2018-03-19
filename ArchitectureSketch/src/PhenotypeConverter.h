#pragma once

#include "GeneticAlgorithm.h"
#include "ArchitectureProgram.h"

class PhenotypeConverter
{
protected:
	ArchitectureProgram* pProgram;

public:
	//virtual void Convert(Genotype genotype) = 0;

	void setProgram(ArchitectureProgram& program)
	{
		pProgram = &program;
	};

	virtual bool isValid(Genotype genotype) = 0;
};
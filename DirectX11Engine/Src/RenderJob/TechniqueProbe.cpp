#include "TechniqueProbe.h"
#include "Technique.h"
#include "RenderStep.h"

void TechniqueProbe::SetTechnique(Technique* pTech)
{
	this->pTech = pTech;
	OnSetTechnique();
}

void TechniqueProbe::SetStep(RenderStep* pStep)
{
	this->pStep = pStep;
	OnSetStep();
}

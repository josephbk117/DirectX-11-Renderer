#pragma once
class TechniqueProbe
{
public:
	void SetTechnique(class Technique* pTech);
	void SetStep(class RenderStep* pStep);
	virtual bool VisitBuffer() = 0;
protected:
	virtual void OnSetTechnique() {}
	virtual void OnSetStep() {}
	class Technique* pTech = nullptr;
	class RenderStep* pStep = nullptr;
};


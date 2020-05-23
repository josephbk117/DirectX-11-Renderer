#pragma once
#include "..\Graphics.h"
#include <DirectXMath.h>

class Bindable;

class Drawable
{
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	virtual void Draw(Graphics& gfx) const noexcept;
	virtual ~Drawable() = default;
protected:
	void AddBind(std::shared_ptr<Bindable> bind) noexcept;
private:
	const class IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::shared_ptr<Bindable>> binds;
};
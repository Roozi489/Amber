#pragma once
#include "BaseSystem.h"
#include "Graphics/Renderer.h"

#include <memory>

namespace Amber
{

struct UniformValue;
struct CommandQueueSettings;
class ShaderProgram;

class RenderSystem : public BaseSystem
{
public:
	RenderSystem();
	~RenderSystem() = default;

	Renderer* getRenderer();

	virtual void init() override;
	virtual void destroy() override;
	virtual void update(Time delta) override;

	void handleWindowSizeChange(int newWidth, int newHeight);
	void display();

private:
	std::unique_ptr<Renderer> m_renderer;
};

}

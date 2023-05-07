#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Events/Event.h"
#include "Engine/Core/Timestep.h"

namespace Hanabi
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer") :m_DebugName(name) {}
		virtual ~Layer() = default;
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnEvent(Event& evnet) {}
		virtual void OnImGuiRender() {}
		const std::string& GetName()const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};
}
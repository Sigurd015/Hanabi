#pragma once
#include "Engine/Core.h"
#include "Engine/Events/Event.h"

namespace Hanabi
{
	class HNB_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() = default;
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnEvent(Event& evnet) {}
		virtual void OnImGuiRender() {}
		inline const std::string& GetName()const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};
}
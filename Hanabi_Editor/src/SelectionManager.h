#pragma once
#include "Hanabi.h"

namespace Hanabi
{
	class SelectionManager
	{
	public:
		inline static void SetSelectedEntity(Entity entity) { m_selectedEntity = entity; }
		inline static Entity GetSelectedEntity() { return m_selectedEntity; }

	private:
		inline static Entity m_selectedEntity;
	};
}
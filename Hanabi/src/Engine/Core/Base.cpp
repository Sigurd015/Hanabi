#include "hnbpch.h"
#include "Base.h"

namespace Hanabi
{
	void InitializeCore()
	{
		Log::Init();
		HNB_CORE_TRACE("Initializing...");
	}

	void ShutdownCore()
	{
		HNB_CORE_TRACE("Shutting down...");
		Log::Showdown();
	}
}
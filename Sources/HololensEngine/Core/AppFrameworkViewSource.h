#pragma once

#include "pch.h"

namespace HololensEngineCore
{
	ref class AppFrameworkViewSource : public Windows::ApplicationModel::Core::IFrameworkViewSource
	{
	public:
		virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView();
	};
}
#include "AppFrameworkViewSource.h"
#include "AppFrameworkView.h"

using namespace Windows::ApplicationModel::Core;
using namespace HololensEngineCore;

IFrameworkView^ AppFrameworkViewSource::CreateView()
{
	return ref new AppFrameworkView();
}
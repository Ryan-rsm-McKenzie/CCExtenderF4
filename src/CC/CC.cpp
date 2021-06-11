#include "CC.h"

#include "CC/AddAchievement.h"
#include "CC/Clear.h"
#include "CC/ClearAchievement.h"
#include "CC/CrashToDesktop.h"
#include "CC/Help.h"

namespace CC
{
	void Install()
	{
		AddAchievement::Install();
		Clear::Install();
		ClearAchievement::Install();
		CrashToDesktop::Install();
		Help::Install();

		logger::debug("installed all console commands"sv);
	}
}

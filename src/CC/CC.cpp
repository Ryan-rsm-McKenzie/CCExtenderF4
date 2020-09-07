#include "CC.h"

#include "CC/Clear.h"
#include "CC/Help.h"

namespace CC
{
	void Install()
	{
		Clear::Install();
		Help::Install();

		logger::info("installed all console commands"sv);
	}
}

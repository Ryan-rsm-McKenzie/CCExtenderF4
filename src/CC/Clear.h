#pragma once

namespace CC::Clear
{
	namespace detail
	{
		inline constexpr auto LONG_NAME = "Clear"sv;
		inline constexpr auto SHORT_NAME = ""sv;

		inline bool Execute(const RE::SCRIPT_PARAMETER*, const char*, RE::TESObjectREFR*, RE::TESObjectREFR*, RE::Script*, RE::ScriptLocals*, float&, std::uint32_t&)
		{
			const auto task = F4SE::GetTaskInterface();
			task->AddUITask([]() {
				const auto ui = RE::UI::GetSingleton();
				const auto console = ui ? ui->GetMenu("Console"sv) : nullptr;
				if (console) {
					console->menuObj.Invoke("ClearHistory");
				}
			});

			return true;
		}

		[[nodiscard]] inline const std::string& HelpString()
		{
			static auto help = []() {
				std::string buf;
				buf += "\"Clear\"";
				return buf;
			}();
			return help;
		}
	}

	inline void Install()
	{
		const auto functions = RE::SCRIPT_FUNCTION::GetConsoleFunctions();
		const auto it = std::find_if(
			functions.begin(),
			functions.end(),
			[&](auto&& a_elem) {
				return _stricmp(a_elem.functionName, "DumpNiUpdates") == 0;
			});
		if (it != functions.end()) {
			*it = RE::SCRIPT_FUNCTION{ detail::LONG_NAME.data(), detail::SHORT_NAME.data(), it->output };
			it->helpString = detail::HelpString().data();
			it->executeFunction = detail::Execute;

			logger::debug("installed {}", detail::LONG_NAME);
		} else {
			stl::report_and_fail("failed to find function"sv);
		}
	}
}

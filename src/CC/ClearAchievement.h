#pragma once

namespace CC::ClearAchievement
{
	namespace detail
	{
		inline constexpr auto LONG_NAME = "ClearAchievement"sv;
		inline constexpr auto SHORT_NAME = ""sv;

		[[nodiscard]] inline const std::string& HelpString()
		{
			static auto help = []() {
				std::string buf;
				buf += "\"ClearAchievement\" <id>";
				buf += "\n\t<id> ::= <integer>";
				return buf;
			}();
			return help;
		}

		inline void Print(stl::zstring a_string)
		{
			const auto log = RE::ConsoleLog::GetSingleton();
			if (log) {
				log->AddString(a_string.data());
			}
		}

		inline bool Execute(
			const RE::SCRIPT_PARAMETER* a_parameters,
			const char* a_compiledParams,
			RE::TESObjectREFR* a_refObject,
			RE::TESObjectREFR* a_container,
			RE::Script* a_script,
			RE::ScriptLocals* a_scriptLocals,
			float&,
			std::uint32_t& a_offset)
		{
			std::int32_t id = -1;
			RE::Script::ParseParameters(
				a_parameters,
				a_compiledParams,
				a_offset,
				a_refObject,
				a_container,
				a_script,
				a_scriptLocals,
				&id);

			const auto dhandler = RE::TESDataHandler::GetSingleton();
			if (dhandler && !dhandler->CheckModsLoaded(true)) {
				auto& awards = RE::BSAwardsSystemUtility::GetSingleton();
				awards.DoClearAward(id);
			} else {
				Print("Achievements are disabled on this save!\n"sv);
			}

			return true;
		}
	}

	inline void Install()
	{
		const auto functions = RE::SCRIPT_FUNCTION::GetConsoleFunctions();
		const auto it = std::find_if(
			functions.begin(),
			functions.end(),
			[&](auto&& a_elem) {
				return _stricmp(a_elem.functionName, "ClearAchievement") == 0;
			});
		if (it != functions.end()) {
			static std::array params{
				RE::SCRIPT_PARAMETER{ "Integer", RE::SCRIPT_PARAM_TYPE::kInt, false },
			};

			*it = RE::SCRIPT_FUNCTION{ detail::LONG_NAME.data(), detail::SHORT_NAME.data(), it->output };
			it->helpString = detail::HelpString().data();
			it->paramCount = static_cast<std::uint16_t>(params.size());
			it->parameters = params.data();
			it->executeFunction = detail::Execute;

			logger::debug("installed {}", detail::LONG_NAME);
		} else {
			stl::report_and_fail("failed to find function"sv);
		}
	}
}

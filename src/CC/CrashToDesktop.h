#pragma once

namespace CC::CrashToDesktop
{
	namespace detail
	{
		inline bool Execute(const RE::SCRIPT_PARAMETER*, const char*, RE::TESObjectREFR*, RE::TESObjectREFR*, RE::Script*, RE::ScriptLocals*, float&, std::uint32_t&)
		{
			const auto task = F4SE::GetTaskInterface();
			task->AddTask([]() {
				REL::Relocation<RE::NiPointer<RE::PlayerCharacter>*> player{ REL::ID(303410) };
				*player = nullptr;
			});

			return true;
		}

		[[nodiscard]] inline const std::string& HelpString()
		{
			static auto help = []() {
				std::string buf;
				buf += "\"CrashToDesktop\" | \"CTD\"";
				return buf;
			}();
			return help;
		}

		static constexpr auto LONG_NAME = "CrashToDesktop"sv;
		static constexpr auto SHORT_NAME = "CTD"sv;
	}

	inline void Install()
	{
		const auto functions = RE::SCRIPT_FUNCTION::GetConsoleFunctions();
		const auto it = std::find_if(
			functions.begin(),
			functions.end(),
			[&](auto&& a_elem) {
				return _stricmp(a_elem.functionName, "CollisionMesh") == 0;
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

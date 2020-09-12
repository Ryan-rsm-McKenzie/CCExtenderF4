#pragma once

namespace CC
{
	class CrashToDesktop
	{
	public:
		static void Install()
		{
			const auto functions = RE::SCRIPT_FUNCTION::GetConsoleFunctions();
			const auto it = std::find_if(
				functions.begin(),
				functions.end(),
				[&](auto&& a_elem) {
					return _stricmp(a_elem.functionName, "CollisionMesh") == 0;
				});
			if (it != functions.end()) {
				*it = RE::SCRIPT_FUNCTION{ LONG_NAME.data(), SHORT_NAME.data(), it->output };
				it->helpString = HelpString().data();
				it->executeFunction = Execute;

				logger::info("installed {}", LONG_NAME);
			} else {
				stl::report_and_fail("failed to find function"sv);
			}
		}

	private:
		static bool Execute(const RE::SCRIPT_PARAMETER*, const char*, RE::TESObjectREFR*, RE::TESObjectREFR*, RE::Script*, RE::ScriptLocals*, float&, std::uint32_t&)
		{
			const auto task = F4SE::GetTaskInterface();
			task->AddTask([]() {
				REL::Relocation<RE::NiPointer<RE::PlayerCharacter>*> player{ REL::ID(303410) };
				*player = nullptr;
			});

			return true;
		}

		[[nodiscard]] static const std::string& HelpString()
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
	};
}

#pragma once

#include "EditorIDCache.h"

namespace CC
{
	class Help
	{
	public:
		static void Install()
		{
			const auto functions = RE::SCRIPT_FUNCTION::GetConsoleFunctions();
			const auto it = std::find_if(
				functions.begin(),
				functions.end(),
				[&](auto&& a_elem) { return _stricmp(a_elem.functionName, LONG_NAME.data()) == 0; });
			if (it != functions.end()) {
				it->helpString = HelpString().data();
				it->executeFunction = Execute;

				logger::info("installed {}", LONG_NAME);
			} else {
				stl::report_and_fail("failed to find function"sv);
			}
		}

	private:
		enum class Filter
		{
			kAll,
			kFunctions,
			kGlobals,
			kForms
		};

		template <class T, std::size_t N, class UnaryFunctor>
		[[nodiscard]] static auto Enumerate(
			std::string_view a_matchstring,
			stl::span<T, N> a_src,
			UnaryFunctor a_callback)
		{
			boost::container::vector<bool> results(a_src.size(), false);
			const boost::algorithm::knuth_morris_pratt kmp{
				stl::cistring_iterator{ a_matchstring.begin() },
				stl::cistring_iterator{ a_matchstring.end() }
			};

			std::for_each_n(
				std::execution::parallel_unsequenced_policy{},
				a_src.begin(),
				a_src.size(),
				[&](auto&& a_elem) noexcept {
					for (const auto& haystack : a_callback(a_elem)) {
						const auto [first, last] = kmp(
							stl::cistring_iterator{ haystack.begin() },
							stl::cistring_iterator{ haystack.end() });
						if (first != last) {
							const auto pos = std::addressof(a_elem) - a_src.data();
							results[pos] = true;
						}
					}
				});

			std::vector<
				std::conditional_t<
					std::is_pointer_v<T>,
					std::remove_const_t<T>,
					T*>>
				matched;
			matched.reserve(a_src.size());
			for (std::size_t i = 0; i < results.size(); ++i) {
				if (results[i]) {
					if constexpr (std::is_pointer_v<T>) {
						matched.push_back(a_src[i]);
					} else {
						matched.push_back(a_src.data() + i);
					}
				}
			}
			return matched;
		}

		static void EnumerateFunctions(std::string_view a_matchstring)
		{
			const auto print = [](const std::vector<RE::SCRIPT_FUNCTION*>& a_todo) {
				std::string line;
				for (auto& elem : a_todo) {
					line = stl::safe_string{ elem->functionName };

					if (const stl::safe_string shortName = elem->shortName; !shortName.empty()) {
						line += " ("sv;
						line += shortName;
						line += ')';
					}

					if (const stl::safe_string helpString = elem->helpString; !helpString.empty()) {
						line += " -> "sv;
						line += helpString;
					}

					line += '\n';
					Print(line);
				}
			};

			const auto functor = [](const RE::SCRIPT_FUNCTION& a_elem) noexcept {
				return std::array<std::string_view, 2>{
					stl::safe_string{ a_elem.functionName },
					stl::safe_string{ a_elem.shortName }
				};
			};

			Print("----CONSOLE COMMANDS--------------------\n"sv);
			print(Enumerate(
				a_matchstring,
				RE::SCRIPT_FUNCTION::GetConsoleFunctions(),
				functor));

			Print("----SCRIPT FUNCTIONS--------------------\n"sv);
			print(Enumerate(
				a_matchstring,
				RE::SCRIPT_FUNCTION::GetScriptFunctions(),
				functor));
		}

		static void EnumerateGlobals([[maybe_unused]] std::string_view a_matchstring)
		{
			Print("----GLOBAL VARIABLES--------------------\n"sv);
			const auto dataHandler = RE::TESDataHandler::GetSingleton();
			const auto& globals = dataHandler->GetFormArray<RE::TESGlobal>();
			const auto cache = EditorIDCache::get().access();
			const auto matches = Enumerate(
				a_matchstring,
				stl::span{ globals.begin(), globals.size() },
				[&](const RE::TESGlobal* a_global) noexcept {
					boost::container::static_vector<std::string_view, 1> arr;
					const auto editorID = a_global ? cache->find(a_global->GetFormID()) : nullptr;
					if (editorID) {
						arr.emplace_back(*editorID);
					}
					return arr;
				});

			std::string buf;
			for (const auto match : matches) {
				buf = fmt::format(
					FMT_STRING("{} = {:0.2f}\n"sv),
					*cache->find(match->GetFormID()),
					match->value);
				Print(buf);
			}
		}

		static bool Execute(
			const RE::SCRIPT_PARAMETER* a_parameters,
			const char* a_compiledParams,
			RE::TESObjectREFR* a_refObject,
			RE::TESObjectREFR* a_container,
			RE::Script* a_script,
			RE::ScriptLocals* a_scriptLocals,
			float&,
			std::uint32_t& a_offset)
		{
			auto [matchstring, filter, formtype] = Parse(a_parameters, a_compiledParams, a_offset, a_refObject, a_container, a_script, a_scriptLocals);

			if (!matchstring) {
				Print(HelpString() + '\n');
				return true;
			} else {
				for (auto& ch : *matchstring) {
					ch = stl::tolower(ch);
				}
			}

			if (!filter) {
				filter = Filter::kAll;
			}

			switch (*filter) {
			case Filter::kAll:
			case Filter::kFunctions:
				EnumerateFunctions(*matchstring);
				break;
			default:
				break;
			}

			switch (*filter) {
			case Filter::kAll:
			case Filter::kGlobals:
				EnumerateGlobals(*matchstring);
				break;
			default:
				break;
			}

			return true;
		}

		[[nodiscard]] static const std::string& HelpString()
		{
			static auto help = []() {
				std::string buf;
				buf += "\"Help\" <expr>";
				buf += "\n\t<expr> ::= <empty> | \" \" <matchstring> | \" \" <matchstring> \" \" <filter> | \" \" <matchstring> \" \" <filter> \" \" <form-type>";
				buf += "\n\t<matchstring> ::= <string> ; The string to filter results with";
				buf += "\n\t<filter> ::= <integer>";
				buf += "\n\t\t; 0 - All";
				buf += "\n\t\t; 1 - Functions";
				buf += "\n\t\t; 2 - Settings";
				buf += "\n\t\t; 3 - Globals";
				buf += "\n\t\t; 4 - Forms";
				buf += "\n\t<form-type> ::= <string> ; The form type to filter form results with";
				return buf;
			}();
			return help;
		}

		[[nodiscard]] static auto Parse(
			const RE::SCRIPT_PARAMETER* a_parameters,
			const char* a_compiledParams,
			std::uint32_t& a_offset,
			RE::TESObjectREFR* a_refObject,
			RE::TESObjectREFR* a_container,
			RE::Script* a_script,
			RE::ScriptLocals* a_scriptLocals)
			-> std::tuple<
				std::optional<std::string>,
				std::optional<Filter>,
				std::optional<std::string>>
		{
			std::array<char, 0x200> matchstring{ '\0' };
			std::int32_t filter = -1;
			std::array<char, 0x200> formtype{ '\0' };

			RE::Script::ParseParameters(
				a_parameters,
				a_compiledParams,
				a_offset,
				a_refObject,
				a_container,
				a_script,
				a_scriptLocals,
				matchstring.data(),
				std::addressof(filter),
				formtype.data());

			std::tuple<
				std::optional<std::string>,
				std::optional<Filter>,
				std::optional<std::string>>
				results;

			if (matchstring[0] != '\0') {
				std::get<0>(results) = matchstring.data();
			}

			if (filter != -1) {
				std::get<1>(results) = static_cast<Filter>(filter);
			}

			if (formtype[0] != '\0') {
				std::get<2>(results) = formtype.data();
			}

			return results;
		}

		static void Print(stl::zstring a_string)
		{
			const auto log = RE::ConsoleLog::GetSingleton();
			if (log) {
				log->AddString(a_string.data());
			}
		}

		static constexpr auto LONG_NAME = "Help"sv;
	};
}

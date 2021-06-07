#pragma once

#include "EditorIDCache.h"
#include "FormTypeMap.h"

namespace CC::Help
{
	namespace detail
	{
		enum class Filter
		{
			kAll,
			kFunctions,
			kSettings,
			kGlobals,
			kForms,

			kTotal
		};

		[[nodiscard]] inline const std::string& HelpString()
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

		[[nodiscard]] inline auto Parse(
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

		inline void Print(stl::zstring a_string)
		{
			const auto log = RE::ConsoleLog::GetSingleton();
			if (log) {
				log->AddString(a_string.data());
			}
		}

		template <class T, std::size_t N, class UnaryFunctor>
		[[nodiscard]] inline auto Enumerate(
			std::string_view a_matchstring,
			std::span<T, N> a_src,
			UnaryFunctor a_callback)
		{
			boost::container::vector<bool> results(a_src.size(), false);
			const boost::algorithm::knuth_morris_pratt kmp{
				a_matchstring.begin(),
				a_matchstring.end()
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

		inline void EnumerateForms(std::string_view a_matchstring, std::optional<RE::ENUM_FORM_ID> a_formtype)
		{
			Print("----OTHER FORMS--------------------\n"sv);
			const auto [allForms, allFormsMapLock] = RE::TESForm::GetAllForms();
			RE::BSAutoReadLock l{ allFormsMapLock };
			if (allForms) {
				std::vector<RE::TESForm*> candidates;
				std::for_each(
					allForms->begin(),
					allForms->end(),
					[&](auto&& a_elem) {
						if (const auto form = a_elem.second;
							form && (!a_formtype || form->GetFormType() == *a_formtype)) {
							candidates.push_back(form);
						}
					});

				const auto getDisplayName = [](RE::TESForm& a_form) {
					auto displayName = RE::TESFullName::GetFullName(a_form, true);
					if (displayName.empty()) {
						const auto lvli = a_form.As<RE::TESLeveledList>();
						displayName = lvli ? stl::safe_string(lvli->GetOverrideName()) : ""sv;
					}
					return displayName;
				};

				const auto idCache = EditorIDCache::get().access();
				auto matches = Enumerate(
					a_matchstring,
					std::span{ candidates.data(), candidates.size() },
					[&](auto&& a_form) {
						boost::container::static_vector<std::string_view, 2> arr;
						if (const auto editorID = idCache->find(a_form->GetFormID()); editorID) {
							arr.emplace_back(*editorID);
						}
						if (const auto displayName = getDisplayName(*a_form); !displayName.empty()) {
							arr.push_back(displayName);
						}
						return arr;
					});

				std::sort(
					matches.begin(),
					matches.end(),
					[](auto&& a_lhs, auto&& a_rhs) noexcept {
						return a_lhs->GetFormType() != a_rhs->GetFormType() ?
					               a_lhs->GetFormType() < a_rhs->GetFormType() :
                                   a_lhs->GetFormID() < a_rhs->GetFormID();
					});
				const auto& formTypeMap = FormTypeMap::get();
				std::string buf;
				for (const auto match : matches) {
					buf.clear();
					if (const auto filename = [&]() {
							const auto file = match->GetDescriptionOwnerFile();
							return file ? file->GetFilename() : ""sv;
						}();
						!filename.empty()) {
						buf += filename;
						buf += ' ';
					}

					if (const auto typeString = formTypeMap.find(match->GetFormType());
						typeString) {
						buf += *typeString;
					}

					buf += ':';

					if (const auto editorID = idCache->find(match->GetFormID());
						editorID) {
						buf += ' ';
						buf += *editorID;
					}

					buf += fmt::format(FMT_STRING(" ({:08X})"), match->GetFormID());

					if (const auto displayName = getDisplayName(*match);
						!displayName.empty()) {
						buf += ' ';
						buf += displayName;
					}

					buf += '\n';
					Print(buf);
				}
			}
		}

		inline void EnumerateFunctions(std::string_view a_matchstring)
		{
			const auto print = [](const std::vector<RE::SCRIPT_FUNCTION*>& a_todo) {
				std::string line;
				for (auto& elem : a_todo) {
					line = stl::safe_string(elem->functionName);

					if (const auto shortName = stl::safe_string(elem->shortName);
						!shortName.empty()) {
						line += " ("sv;
						line += shortName;
						line += ')';
					}

					if (const auto helpString = stl::safe_string(elem->helpString);
						!helpString.empty()) {
						line += " -> "sv;
						line += helpString;
					}

					line += '\n';
					Print(line);
				}
			};

			const auto functor = [](const RE::SCRIPT_FUNCTION& a_elem) noexcept {
				return std::array{
					stl::safe_string(a_elem.functionName),
					stl::safe_string(a_elem.shortName)
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

		inline void EnumerateGlobals(std::string_view a_matchstring)
		{
			Print("----GLOBAL VARIABLES--------------------\n"sv);
			const auto dataHandler = RE::TESDataHandler::GetSingleton();
			const auto& globals = dataHandler->GetFormArray<RE::TESGlobal>();
			const auto cache = EditorIDCache::get().access();
			auto matches = Enumerate(
				a_matchstring,
				std::span{ globals.begin(), globals.size() },
				[&](const RE::TESGlobal* a_global) noexcept {
					boost::container::static_vector<std::string_view, 1> arr;
					const auto editorID = a_global ? cache->find(a_global->GetFormID()) : nullptr;
					if (editorID) {
						arr.emplace_back(*editorID);
					}
					return arr;
				});

			std::sort(
				matches.begin(),
				matches.end(),
				[](auto&& a_lhs, auto&& a_rhs) {
					return a_lhs->GetFormID() < a_rhs->GetFormID();
				});
			std::string buf;
			for (const auto match : matches) {
				buf = fmt::format(
					FMT_STRING("{} = {:0.2f}\n"),
					*cache->find(match->GetFormID()),
					match->value);
				Print(buf);
			}
		}

		inline void EnumerateSettings(std::string_view a_matchstring)
		{
			Print("----SETTINGS----------------------------\n"sv);

			auto candidates = []() {
				robin_hood::unordered_flat_map<std::string_view, RE::Setting*> map;
				const auto inis = stl::make_array(
					RE::INISettingCollection::GetSingleton(),
					RE::INIPrefSettingCollection::GetSingleton());
				for (const auto ini : inis) {
					if (ini) {
						for (const auto setting : ini->settings) {
							if (setting) {
								map.insert_or_assign(setting->GetKey(), setting);
							}
						}
					}
				}
				return std::vector<decltype(map)::value_type>{ map.begin(), map.end() };
			}();

			if (const auto gmst = RE::GameSettingCollection::GetSingleton(); gmst) {
				for (const auto& [name, setting] : gmst->settings) {
					if (setting) {
						candidates.emplace_back(name, setting);
					}
				}
			}

			auto matches = Enumerate(
				a_matchstring,
				std::span{ candidates.data(), candidates.size() },
				[](auto&& a_elem) {
					return std::array{ a_elem.first };
				});

			std::sort(
				matches.begin(),
				matches.end(),
				[](auto&& a_lhs, auto&& a_rhs) {
					return _stricmp(a_lhs->first.data(), a_rhs->first.data()) < 0;
				});
			std::string buf;
			for (const auto match : matches) {
				const auto& [name, setting] = *match;
				using Type = RE::Setting::SETTING_TYPE;
				switch (setting->GetType()) {
				case Type::kBinary:
					buf = fmt::format(FMT_STRING("{} = {}\n"), name, setting->GetBinary());
					break;
				case Type::kChar:
					buf = fmt::format(FMT_STRING("{} = {}\n"), name, setting->GetChar());
					break;
				case Type::kUChar:
					buf = fmt::format(FMT_STRING("{} = {:#04x}\n"), name, setting->GetUChar());
					break;
				case Type::kInt:
					buf = fmt::format(FMT_STRING("{} = {}\n"), name, setting->GetInt());
					break;
				case Type::kUInt:
					buf = fmt::format(FMT_STRING("{} = {}\n"), name, setting->GetUInt());
					break;
				case Type::kFloat:
					buf = fmt::format(FMT_STRING("{} = {:0.2f}\n"), name, setting->GetFloat());
					break;
				case Type::kString:
					buf = fmt::format(FMT_STRING("{} = {}\n"), name, setting->GetString());
					break;
				case Type::kRGB:
					{
						const auto rgb = setting->GetRGB();
						buf = fmt::format(FMT_STRING("{} = R:{} G:{} B:{}\n"), name, rgb[0], rgb[1], rgb[2]);
					}
				case Type::kRGBA:
					{
						const auto rgba = setting->GetRGBA();
						buf = fmt::format(FMT_STRING("{} = R:{} G:{} B:{} A:{}\n"), name, rgba[0], rgba[1], rgba[2], rgba[3]);
					}
					break;
				default:
					buf = fmt::format(FMT_STRING("{} = <UNKNOWN>\n"), name);
					break;
				}
				Print(buf);
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
			auto [matchstring, filter, formtype] = Parse(a_parameters, a_compiledParams, a_offset, a_refObject, a_container, a_script, a_scriptLocals);

			if (!matchstring) {
				Print(HelpString() + '\n');
				return true;
			} else if (filter && (*filter < static_cast<Filter>(0) || *filter >= Filter::kTotal)) {
				Print("<filter> must be a valid filter\n"sv);
				return true;
			} else if (formtype && formtype->length() != 4) {
				Print("<form-type> must be 4 characters in length\n"sv);
				return true;
			} else if ([](std::optional<std::string>& a_formtype) {
						   if (a_formtype) {
							   for (auto& ch : *a_formtype) {
								   ch = stl::toupper(ch);
							   }
							   return !FormTypeMap::get().find(*a_formtype);
						   } else {
							   return false;
						   }
					   }(formtype)) {
				Print("<form-type> must be a valid form type\n"sv);
				return true;
			}

			for (auto& ch : *matchstring) {
				ch = stl::tolower(ch);
			}

			if (!filter) {
				filter = Filter::kAll;
			}

			if (*filter == Filter::kAll || *filter == Filter::kFunctions) {
				EnumerateFunctions(*matchstring);
			}

			if (*filter == Filter::kAll || *filter == Filter::kSettings) {
				EnumerateSettings(*matchstring);
			}

			if (*filter == Filter::kAll || *filter == Filter::kGlobals) {
				EnumerateGlobals(*matchstring);
			}

			if (*filter == Filter::kAll || *filter == Filter::kForms) {
				EnumerateForms(
					*matchstring,
					(formtype ? FormTypeMap::get().find(*formtype) : std::nullopt));
			}

			return true;
		}

		inline constexpr auto LONG_NAME = "Help"sv;
		inline constexpr auto SHORT_NAME = ""sv;
	}

	inline void Install()
	{
		const auto functions = RE::SCRIPT_FUNCTION::GetConsoleFunctions();
		const auto it = std::find_if(
			functions.begin(),
			functions.end(),
			[&](auto&& a_elem) {
				return _stricmp(a_elem.functionName, detail::LONG_NAME.data()) == 0;
			});
		if (it != functions.end()) {
			static std::array params{
				RE::SCRIPT_PARAMETER{ "String (Optional)", RE::SCRIPT_PARAM_TYPE::kChar, true },
				RE::SCRIPT_PARAMETER{ "Integer (Optional)", RE::SCRIPT_PARAM_TYPE::kInt, true },
				RE::SCRIPT_PARAMETER{ "String (Optional)", RE::SCRIPT_PARAM_TYPE::kChar, true },
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

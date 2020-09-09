#pragma once

#pragma push_macro("ELEMENT")
#define ELEMENT(a_elem) std::make_pair(#a_elem##sv, RE::ENUM_FORM_ID::k##a_elem)

class FormTypeMap
{
public:
	using string_type = std::string_view;
	using enum_type = RE::ENUM_FORM_ID;

	[[nodiscard]] static FormTypeMap& get()
	{
		static FormTypeMap singleton;
		return singleton;
	}

	[[nodiscard]] std::optional<enum_type> find(string_type a_key) const
	{
		assert(a_key.length() == 4);
		const auto it = _str2Enum.find(a_key);
		return it != _str2Enum.end() ? std::make_optional(it->second) : std::nullopt;
	}

	[[nodiscard]] std::optional<string_type> find(enum_type a_key) const
	{
		const auto it = _enum2Str.find(a_key);
		return it != _enum2Str.end() ? std::make_optional(it->second) : std::nullopt;
	}

private:
	FormTypeMap()
	{
		constexpr std::array seeds{
			ELEMENT(NONE),
			ELEMENT(TES4),
			ELEMENT(GRUP),
			ELEMENT(GMST),
			ELEMENT(KYWD),
			ELEMENT(LCRT),
			ELEMENT(AACT),
			ELEMENT(TRNS),
			ELEMENT(CMPO),
			ELEMENT(TXST),
			ELEMENT(MICN),
			ELEMENT(GLOB),
			ELEMENT(DMGT),
			ELEMENT(CLAS),
			ELEMENT(FACT),
			ELEMENT(HDPT),
			ELEMENT(EYES),
			ELEMENT(RACE),
			ELEMENT(SOUN),
			ELEMENT(ASPC),
			ELEMENT(SKIL),
			ELEMENT(MGEF),
			ELEMENT(SCPT),
			ELEMENT(LTEX),
			ELEMENT(ENCH),
			ELEMENT(SPEL),
			ELEMENT(SCRL),
			ELEMENT(ACTI),
			ELEMENT(TACT),
			ELEMENT(ARMO),
			ELEMENT(BOOK),
			ELEMENT(CONT),
			ELEMENT(DOOR),
			ELEMENT(INGR),
			ELEMENT(LIGH),
			ELEMENT(MISC),
			ELEMENT(STAT),
			ELEMENT(SCOL),
			ELEMENT(MSTT),
			ELEMENT(GRAS),
			ELEMENT(TREE),
			ELEMENT(FLOR),
			ELEMENT(FURN),
			ELEMENT(WEAP),
			ELEMENT(AMMO),
			ELEMENT(NPC_),
			ELEMENT(LVLN),
			ELEMENT(KEYM),
			ELEMENT(ALCH),
			ELEMENT(IDLM),
			ELEMENT(NOTE),
			ELEMENT(PROJ),
			ELEMENT(HAZD),
			ELEMENT(BNDS),
			ELEMENT(SLGM),
			ELEMENT(TERM),
			ELEMENT(LVLI),
			ELEMENT(WTHR),
			ELEMENT(CLMT),
			ELEMENT(SPGD),
			ELEMENT(RFCT),
			ELEMENT(REGN),
			ELEMENT(NAVI),
			ELEMENT(CELL),
			ELEMENT(REFR),
			ELEMENT(ACHR),
			ELEMENT(PMIS),
			ELEMENT(PARW),
			ELEMENT(PGRE),
			ELEMENT(PBEA),
			ELEMENT(PFLA),
			ELEMENT(PCON),
			ELEMENT(PBAR),
			ELEMENT(PHZD),
			ELEMENT(WRLD),
			ELEMENT(LAND),
			ELEMENT(NAVM),
			ELEMENT(TLOD),
			ELEMENT(DIAL),
			ELEMENT(INFO),
			ELEMENT(QUST),
			ELEMENT(IDLE),
			ELEMENT(PACK),
			ELEMENT(CSTY),
			ELEMENT(LSCR),
			ELEMENT(LVSP),
			ELEMENT(ANIO),
			ELEMENT(WATR),
			ELEMENT(EFSH),
			ELEMENT(TOFT),
			ELEMENT(EXPL),
			ELEMENT(DEBR),
			ELEMENT(IMGS),
			ELEMENT(IMAD),
			ELEMENT(FLST),
			ELEMENT(PERK),
			ELEMENT(BPTD),
			ELEMENT(ADDN),
			ELEMENT(AVIF),
			ELEMENT(CAMS),
			ELEMENT(CPTH),
			ELEMENT(VTYP),
			ELEMENT(MATT),
			ELEMENT(IPCT),
			ELEMENT(IPDS),
			ELEMENT(ARMA),
			ELEMENT(ECZN),
			ELEMENT(LCTN),
			ELEMENT(MESG),
			ELEMENT(RGDL),
			ELEMENT(DOBJ),
			ELEMENT(DFOB),
			ELEMENT(LGTM),
			ELEMENT(MUSC),
			ELEMENT(FSTP),
			ELEMENT(FSTS),
			ELEMENT(SMBN),
			ELEMENT(SMQN),
			ELEMENT(SMEN),
			ELEMENT(DLBR),
			ELEMENT(MUST),
			ELEMENT(DLVW),
			ELEMENT(WOOP),
			ELEMENT(SHOU),
			ELEMENT(EQUP),
			ELEMENT(RELA),
			ELEMENT(SCEN),
			ELEMENT(ASTP),
			ELEMENT(OTFT),
			ELEMENT(ARTO),
			ELEMENT(MATO),
			ELEMENT(MOVT),
			ELEMENT(SNDR),
			ELEMENT(DUAL),
			ELEMENT(SNCT),
			ELEMENT(SOPM),
			ELEMENT(COLL),
			ELEMENT(CLFM),
			ELEMENT(REVB),
			ELEMENT(PKIN),
			ELEMENT(RFGP),
			ELEMENT(AMDL),
			ELEMENT(LAYR),
			ELEMENT(COBJ),
			ELEMENT(OMOD),
			ELEMENT(MSWP),
			ELEMENT(ZOOM),
			ELEMENT(INNR),
			ELEMENT(KSSM),
			ELEMENT(AECH),
			ELEMENT(SCCO),
			ELEMENT(AORU),
			ELEMENT(SCSN),
			ELEMENT(STAG),
			ELEMENT(NOCM),
			ELEMENT(LENS),
			ELEMENT(LSPR),
			ELEMENT(GDRY),
			ELEMENT(OVIS),
		};

		static_assert(seeds.size() == stl::to_underlying(RE::ENUM_FORM_ID::kTotal));

		for (const auto& seed : seeds) {
			_str2Enum.emplace(seed.first, seed.second);
			_enum2Str.emplace(seed.second, seed.first);
		}

		assert(_str2Enum.size() == stl::to_underlying(RE::ENUM_FORM_ID::kTotal));
		assert(_enum2Str.size() == stl::to_underlying(RE::ENUM_FORM_ID::kTotal));
	}

	FormTypeMap(const FormTypeMap&) = delete;
	FormTypeMap(FormTypeMap&&) = delete;

	~FormTypeMap() = default;

	FormTypeMap& operator=(const FormTypeMap&) = delete;
	FormTypeMap& operator=(FormTypeMap&&) = delete;

	robin_hood::unordered_flat_map<string_type, enum_type> _str2Enum;
	robin_hood::unordered_flat_map<enum_type, string_type> _enum2Str;
};

#pragma pop_macro("ELEMENT")

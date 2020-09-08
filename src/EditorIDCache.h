#pragma once

class EditorIDCache
{
public:
	using lock_type = std::mutex;

	class Cache
	{
	public:
		using key_type = std::uint32_t;
		using mapped_type = std::string;

		[[nodiscard]] mapped_type* find(key_type a_key)
		{
			const auto it = _formID2EditorID.find(a_key);
			return it != _formID2EditorID.end() ? std::addressof(it->second) : nullptr;
		}

		[[nodiscard]] const mapped_type* find(key_type a_key) const
		{
			const auto it = _formID2EditorID.find(a_key);
			return it != _formID2EditorID.cend() ? std::addressof(it->second) : nullptr;
		}

		bool insert(key_type a_key, mapped_type a_mapped)
		{
			return _formID2EditorID.insert_or_assign(a_key, std::move(a_mapped)).second;
		}

		bool insert(key_type a_key, std::string_view a_mapped)
		{
			return _formID2EditorID.insert_or_assign(a_key, a_mapped).second;
		}

	private:
		robin_hood::unordered_flat_map<key_type, mapped_type> _formID2EditorID;
	};

	class Accessor
	{
	public:
		using value_type = Cache;

		Accessor() = delete;
		Accessor(const Accessor&) = delete;
		Accessor(Accessor&&) = delete;

		~Accessor() = default;

		Accessor& operator=(const Accessor&) = delete;
		Accessor& operator=(Accessor&&) = delete;

		[[nodiscard]] value_type& operator*() noexcept { return get(); }
		[[nodiscard]] const value_type& operator*() const noexcept { return get(); }

		[[nodiscard]] value_type* operator->() noexcept { return std::addressof(get()); }
		[[nodiscard]] const value_type* operator->() const noexcept { return std::addressof(get()); }

		[[nodiscard]] value_type& get() noexcept { return _proxy; }
		[[nodiscard]] const value_type& get() const noexcept { return _proxy; }

	protected:
		friend class EditorIDCache;

		Accessor(lock_type& a_lock, value_type& a_cache) :
			_locker(a_lock),
			_proxy(a_cache)
		{}

	private:
		std::scoped_lock<lock_type> _locker;
		value_type& _proxy;
	};

	[[nodiscard]] static EditorIDCache& get()
	{
		static EditorIDCache singleton;
		return singleton;
	}

	[[nodiscard]] Accessor access() { return { _lock, _cache }; }

	void install()
	{
		Hook<RE::TESForm>::Install();
		Hook<RE::TESObject>::Install();
		Hook<RE::TESBoundObject>::Install();
		Hook<RE::MagicItem>::Install();
		Hook<RE::TESBoundAnimObject>::Install();
		Hook<RE::TESActorBase>::Install();
		Hook<RE::BGSStoryManagerTreeForm>::Install();
		Hook<RE::BGSStoryManagerNodeBase>::Install();
		Hook<RE::BGSKeyword>::Install();
		Hook<RE::BGSLocationRefType>::Install();
		Hook<RE::BGSAction>::Install();
		Hook<RE::BGSTransform>::Install();
		Hook<RE::BGSComponent>::Install();
		Hook<RE::BGSTextureSet>::Install();
		Hook<RE::BGSMenuIcon>::Install();
		Hook<RE::TESGlobal>::Install();
		Hook<RE::BGSDamageType>::Install();
		Hook<RE::TESClass>::Install();
		Hook<RE::TESFaction>::Install();
		Hook<RE::BGSHeadPart>::Install();
		Hook<RE::TESEyes>::Install();
		Hook<RE::TESRace>::Install();
		Hook<RE::TESSound>::Install();
		Hook<RE::BGSAcousticSpace>::Install();
		Hook<RE::EffectSetting>::Install();
		Hook<RE::Script>::Install();
		Hook<RE::TESLandTexture>::Install();
		Hook<RE::EnchantmentItem>::Install();
		Hook<RE::SpellItem>::Install();
		Hook<RE::ScrollItem>::Install();
		Hook<RE::TESObjectACTI>::Install();
		Hook<RE::BGSTalkingActivator>::Install();
		Hook<RE::TESObjectARMO>::Install();
		Hook<RE::TESObjectBOOK>::Install();
		Hook<RE::TESObjectCONT>::Install();
		Hook<RE::TESObjectDOOR>::Install();
		Hook<RE::IngredientItem>::Install();
		Hook<RE::TESObjectLIGH>::Install();
		Hook<RE::TESObjectMISC>::Install();
		Hook<RE::TESObjectSTAT>::Install();
		Hook<RE::BGSStaticCollection>::Install();
		Hook<RE::BGSMovableStatic>::Install();
		Hook<RE::TESGrass>::Install();
		Hook<RE::TESObjectTREE>::Install();
		Hook<RE::TESFlora>::Install();
		Hook<RE::TESFurniture>::Install();
		Hook<RE::TESObjectWEAP>::Install();
		Hook<RE::TESAmmo>::Install();
		Hook<RE::TESNPC>::Install();
		Hook<RE::TESLevCharacter>::Install();
		Hook<RE::TESKey>::Install();
		Hook<RE::AlchemyItem>::Install();
		Hook<RE::BGSIdleMarker>::Install();
		Hook<RE::BGSNote>::Install();
		Hook<RE::BGSProjectile>::Install();
		Hook<RE::BGSHazard>::Install();
		Hook<RE::BGSBendableSpline>::Install();
		Hook<RE::TESSoulGem>::Install();
		Hook<RE::BGSTerminal>::Install();
		Hook<RE::TESLevItem>::Install();
		Hook<RE::TESWeather>::Install();
		Hook<RE::TESClimate>::Install();
		Hook<RE::BGSShaderParticleGeometryData>::Install();
		Hook<RE::BGSReferenceEffect>::Install();
		Hook<RE::TESRegion>::Install();
		Hook<RE::NavMeshInfoMap>::Install();
		Hook<RE::TESObjectCELL>::Install();
		Hook<RE::TESObjectREFR>::Install();
		Hook<RE::Explosion>::Install();
		Hook<RE::Projectile>::Install();
		Hook<RE::Actor>::Install();
		Hook<RE::PlayerCharacter>::Install();
		Hook<RE::MissileProjectile>::Install();
		Hook<RE::ArrowProjectile>::Install();
		Hook<RE::GrenadeProjectile>::Install();
		Hook<RE::BeamProjectile>::Install();
		Hook<RE::FlameProjectile>::Install();
		Hook<RE::ConeProjectile>::Install();
		Hook<RE::BarrierProjectile>::Install();
		Hook<RE::Hazard>::Install();
		Hook<RE::TESWorldSpace>::Install();
		Hook<RE::TESObjectLAND>::Install();
		Hook<RE::NavMesh>::Install();
		Hook<RE::TESTopic>::Install();
		Hook<RE::TESTopicInfo>::Install();
		Hook<RE::TESQuest>::Install();
		Hook<RE::TESIdleForm>::Install();
		Hook<RE::TESPackage>::Install();
		Hook<RE::AlarmPackage>::Install();
		Hook<RE::DialoguePackage>::Install();
		Hook<RE::FleePackage>::Install();
		Hook<RE::SpectatorPackage>::Install();
		Hook<RE::TrespassPackage>::Install();
		Hook<RE::TESCombatStyle>::Install();
		Hook<RE::TESLoadScreen>::Install();
		Hook<RE::TESLevSpell>::Install();
		Hook<RE::TESObjectANIO>::Install();
		Hook<RE::TESWaterForm>::Install();
		Hook<RE::TESEffectShader>::Install();
		Hook<RE::BGSExplosion>::Install();
		Hook<RE::BGSDebris>::Install();
		Hook<RE::TESImageSpace>::Install();
		Hook<RE::TESImageSpaceModifier>::Install();
		Hook<RE::BGSListForm>::Install();
		Hook<RE::BGSPerk>::Install();
		Hook<RE::BGSBodyPartData>::Install();
		Hook<RE::BGSAddonNode>::Install();
		Hook<RE::ActorValueInfo>::Install();
		Hook<RE::BGSCameraShot>::Install();
		Hook<RE::BGSCameraPath>::Install();
		Hook<RE::BGSVoiceType>::Install();
		Hook<RE::BGSMaterialType>::Install();
		Hook<RE::BGSImpactData>::Install();
		Hook<RE::BGSImpactDataSet>::Install();
		Hook<RE::TESObjectARMA>::Install();
		Hook<RE::BGSEncounterZone>::Install();
		Hook<RE::BGSLocation>::Install();
		Hook<RE::BGSMessage>::Install();
		Hook<RE::BGSDefaultObjectManager>::Install();
		Hook<RE::BGSDefaultObject>::Install();
		Hook<RE::BGSLightingTemplate>::Install();
		Hook<RE::BGSMusicType>::Install();
		Hook<RE::BGSFootstep>::Install();
		Hook<RE::BGSFootstepSet>::Install();
		Hook<RE::BGSStoryManagerBranchNode>::Install();
		Hook<RE::BGSStoryManagerQuestNode>::Install();
		Hook<RE::BGSStoryManagerEventNode>::Install();
		Hook<RE::BGSDialogueBranch>::Install();
		Hook<RE::BGSMusicTrackFormWrapper>::Install();
		Hook<RE::TESWordOfPower>::Install();
		Hook<RE::TESShout>::Install();
		Hook<RE::BGSEquipSlot>::Install();
		Hook<RE::BGSRelationship>::Install();
		Hook<RE::BGSScene>::Install();
		Hook<RE::BGSAssociationType>::Install();
		Hook<RE::BGSOutfit>::Install();
		Hook<RE::BGSArtObject>::Install();
		Hook<RE::BGSMaterialObject>::Install();
		Hook<RE::BGSMovementType>::Install();
		Hook<RE::BGSSoundDescriptorForm>::Install();
		Hook<RE::BGSDualCastData>::Install();
		Hook<RE::BGSSoundCategory>::Install();
		Hook<RE::BGSSoundOutput>::Install();
		Hook<RE::BGSCollisionLayer>::Install();
		Hook<RE::BGSColorForm>::Install();
		Hook<RE::BGSReverbParameters>::Install();
		Hook<RE::BGSPackIn>::Install();
		Hook<RE::BGSAimModel>::Install();
		Hook<RE::BGSConstructibleObject>::Install();
		Hook<RE::BGSMod::Attachment::Mod>::Install();
		Hook<RE::BGSMaterialSwap>::Install();
		Hook<RE::BGSZoomData>::Install();
		Hook<RE::BGSInstanceNamingRules>::Install();
		Hook<RE::BGSSoundKeywordMapping>::Install();
		Hook<RE::BGSAudioEffectChain>::Install();
		Hook<RE::BGSAttractionRule>::Install();
		Hook<RE::BGSSoundCategorySnapshot>::Install();
		Hook<RE::BGSSoundTagSet>::Install();
		Hook<RE::BGSLensFlare>::Install();
		Hook<RE::BGSGodRays>::Install();

		logger::info("installed hooks for {}"sv, typeid(EditorIDCache).name());
	}

private:
	template <class T>
	class Hook
	{
	public:
		static void Install()
		{
			REL::Relocation<std::uintptr_t> vtable{ T::VTABLE[0] };
			_original = vtable.write_vfunc(0x3B, SetFormEditorID);
		}

	private:
		static bool SetFormEditorID(RE::TESForm* a_this, const char* a_editorID)
		{
			if (a_this && !a_this->IsCreated()) {
				EditorIDCache::get().access()->insert(
					a_this->GetFormID(),
					stl::safe_string{ a_editorID });
			}

			return _original(a_this, a_editorID);
		}

		static inline REL::Relocation<decltype(&RE::TESForm::SetFormEditorID)> _original;
	};

	lock_type _lock;
	Cache _cache;
};

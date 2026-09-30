#pragma once
#ifdef __ENABLE_ITEM_TOGGLE__
#include "item.h"

namespace NSToggle {
#ifdef __ENABLE_AUTO_PICKUP__
	bool PerformPickup(CHARACTER* ch, CItem* item = nullptr, CHARACTER* enemy = nullptr);
#endif
};

namespace item {
	class ItemToggle : public CItem {
		public:
			ItemToggle(uint32_t vnum);
			virtual ~ItemToggle();

			virtual bool CanActivate(bool byLoad = false) const;
			bool Activate(bool byLoad = false);
			void Deactivate();

			void SetActive(bool active);
			bool IsActive() const;

			int32_t GetGroup() const;

			void StartExpire();
			void StopExpire();

			virtual void ModifyPoints(bool add);

			virtual void OnComputePoints();

			virtual void OnCreate();
			virtual void OnLoad();
			virtual void OnRemove();
			virtual bool OnUse();

			virtual void OnActivate();
			virtual void OnDeactivate();

		public:
			static bool Is(const CItem& item);
	};
};
#endif

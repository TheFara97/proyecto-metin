#pragma once

#include "abstract/IRequirement.hpp"

#include "../common/RequirementTypes.hpp"

namespace requirement {
    class Item : public abstract::IRequirement {
        public:
            Item(const item& requirement);
            Item(item&& requirement);
            virtual ~Item() = default;

            virtual bool Can(const CHARACTER& character) const;
            virtual void Grant(CHARACTER& character) const;
            virtual void Remove(CHARACTER& character) const;

        protected:
            item requirement_;
    };
}  // namespace requirement


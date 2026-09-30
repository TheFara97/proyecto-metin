#pragma once

#include "abstract/IRequirement.hpp"

#include "../common/RequirementTypes.hpp"

namespace requirement {
    class Gold : public abstract::IRequirement {
        public:
            Gold(const gold& requirement);
            Gold(gold&& requirement);
            virtual ~Gold() = default;

            virtual bool Can(const CHARACTER& character) const;
            virtual void Grant(CHARACTER& character) const;
            virtual void Remove(CHARACTER& character) const;

        protected:
            gold requirement_;
    };
}  // namespace requirement

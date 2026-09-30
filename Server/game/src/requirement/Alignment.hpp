#pragma once

#include "abstract/IRequirement.hpp"

#include "../common/RequirementTypes.hpp"

namespace requirement {
    class Alignment : public abstract::IRequirement {
        public:
            Alignment(const alignment& requirement);
            Alignment(alignment&& requirement);
            virtual ~Alignment() = default;

            virtual bool Can(const CHARACTER& character) const;
            virtual void Grant(CHARACTER& character) const;
            virtual void Remove(CHARACTER& character) const;

        protected:
            alignment requirement_;
    };
}  // namespace requirement


#pragma once

#include <memory>

class CHARACTER;

namespace requirement {
    namespace abstract {
        class IRequirement {
            public:
                virtual ~IRequirement() = default;

                virtual bool Can(const CHARACTER& character) const = 0;
                virtual void Grant(CHARACTER& character) const = 0;
                virtual void Remove(CHARACTER& character) const = 0;
            };

    }  // namespace abstract

    using requirement_ptr = std::unique_ptr<abstract::IRequirement>;
}  // namespace requirement


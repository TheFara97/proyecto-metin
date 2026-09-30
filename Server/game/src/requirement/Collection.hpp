#pragma once

#include "abstract/IRequirement.hpp"

namespace requirement {
    namespace xml {
        class Parser;
    }

    class Collection : public abstract::IRequirement {
        public:
            using collection_t = std::vector<requirement_ptr>;

        public:
            Collection() = default;
            Collection(collection_t&& requirement);
            Collection(xml::Parser& parser);
            Collection(Collection&& o);
            virtual ~Collection() = default;

            virtual bool Can(const CHARACTER& character) const;
            virtual void Grant(CHARACTER& character) const;
            virtual void Remove(CHARACTER& character) const;

            void Add(requirement_ptr ptr);
            void Clear();

        protected:
            collection_t collection_;
    };
}  // namespace requirement


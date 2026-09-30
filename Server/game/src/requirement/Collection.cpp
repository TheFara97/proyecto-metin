#include "../stdafx.h"
#include "Collection.hpp"
#include "Alignment.hpp"
#include "Gold.hpp"
#include "Item.hpp"
#include "xml/Parser.hpp"

namespace requirement {
    Collection::Collection(collection_t&& collection)
        : collection_(std::move(collection)) {}

    Collection::Collection(xml::Parser& parser) {
        for (const auto& alignment : parser.Alignments())
            Add(std::make_unique<Alignment>(alignment));

        for (const auto& gold : parser.Golds())
            Add(std::make_unique<Gold>(gold));

        for (const auto& item : parser.Items())
            Add(std::make_unique<Item>(item));
    }

    Collection::Collection(Collection&& o)
        : collection_(std::move(o.collection_)) {}

    bool Collection::Can(const CHARACTER& character) const {
        for (const auto& requirement : collection_) {
            if (!requirement->Can(character))
                return false;
        }

        return true;
    }

    void Collection::Grant(CHARACTER& character) const {
        for (const auto& requirement : collection_)
            requirement->Grant(character);
    }

    void Collection::Remove(CHARACTER& character) const {
        for (const auto& requirement : collection_)
            requirement->Remove(character);
    }

    void Collection::Add(requirement_ptr ptr) {
        collection_.emplace_back(std::move(ptr));
    }

    void Collection::Clear() {
        collection_.clear();
    }
}  // namespace requirement


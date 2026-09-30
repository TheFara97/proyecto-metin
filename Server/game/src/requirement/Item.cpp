#include "../stdafx.h"
#include "Item.hpp"
#include "../char.h"

namespace requirement {
    Item::Item(const item& requirement) : requirement_(requirement) {}

    Item::Item(item&& requirement) : requirement_(std::move(requirement)) {}

    bool Item::Can(const CHARACTER& character) const {
        return character.CountSpecifyItem(requirement_.vnum) >= requirement_.count;
    }

    void Item::Grant(CHARACTER& character) const {
        character.AutoGiveItem(requirement_.vnum, requirement_.count);
    }

    void Item::Remove(CHARACTER& character) const {
        character.RemoveSpecifyItem(requirement_.vnum, requirement_.count);
    }
}  // namespace requirement


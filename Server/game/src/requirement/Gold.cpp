#include "../stdafx.h"
#include "Gold.hpp"
#include "../char.h"

namespace requirement {
    Gold::Gold(const gold& requirement) : requirement_(requirement) {}

    Gold::Gold(gold&& requirement) : requirement_(std::move(requirement)) {}

    bool Gold::Can(const CHARACTER& character) const {
        return character.GetGold() >= requirement_.amount;
    }

    void Gold::Grant(CHARACTER& character) const {
        character.PointChange(POINT_GOLD, requirement_.amount);
    }

    void Gold::Remove(CHARACTER& character) const {
        character.PointChange(POINT_GOLD, -requirement_.amount);
    }
}  // namespace requirement

#include "../stdafx.h"
#include "Alignment.hpp"
#include "../char.h"

namespace requirement {
    Alignment::Alignment(const alignment& requirement)
        : requirement_(requirement) {}

    Alignment::Alignment(alignment&& requirement)
        : requirement_(std::move(requirement)) {}

    bool Alignment::Can(const CHARACTER& character) const {
        return character.GetRealAlignment() >= requirement_.amount;
    }

    void Alignment::Grant(CHARACTER& character) const {
        character.UpdateAlignment(requirement_.amount);
    }

    void Alignment::Remove(CHARACTER& character) const {
        character.UpdateAlignment(-requirement_.amount);
    }
}  // namespace requirement

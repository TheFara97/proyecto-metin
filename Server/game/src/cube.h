#pragma once

#include "buffer_manager.h"
#include "../../common/length.h"
#include <memory>
class CHARACTER;

namespace cube {

struct Recipe {
    struct Item {
        uint32_t vnum;
        uint16_t count;

        bool upgradeable;
        bool sockets;
        bool magicPct;
        bool removeOnFailure;
        
        // helpers
        bool Check(CHARACTER& ch) const;
        void Charge(CHARACTER& ch, CItem* reward) const;
    };

    struct Reward {
        uint32_t vnum;
        uint16_t count;

        // helpers
        CItem* Create() const;
    };

    struct FailReward {
        uint32_t vnum;
        uint16_t count;

        // helpers
        CItem* Create() const;
    };

    std::vector<Item> items;
    Reward reward;
    FailReward failReward;

    uint8_t category;
    int64_t price;

    int64_t priceCheque;
    int32_t priceAchievement;

    uint8_t chance;

    // helpers
    bool Check(CHARACTER& character, bool ignorePrice = false) const;
    void Charge(CHARACTER& character, CItem* reward) const;
};

namespace packets {

#pragma pack(1)
struct Header {
    uint8_t header;
    uint16_t size;
    uint8_t subheader;
};
#pragma pack()

namespace game {

enum {
    SUBHEADER_START,
    SUBHEADER_INFO,
    SUBHEADER_CLOSE
};

#pragma pack(1)
struct Item {
    Item(const cube::Recipe::Item& item) : vnum(item.vnum), count(item.count) {}

    Item(const cube::Recipe::Reward& item)
        : vnum(item.vnum), count(item.count) {}

    uint32_t vnum;
    uint16_t count;
};

struct Recipe {
    Recipe(const cube::Recipe& recipe)
        : itemCount(recipe.items.size()),
          reward(recipe.reward),
          price(recipe.price), priceCheque(recipe.priceCheque), priceAchievement(recipe.priceAchievement),
            chance(recipe.chance), category(recipe.category) {}

    uint16_t itemCount;
    Item reward;
    int64_t price;
    int64_t priceCheque;
    uint32_t priceAchievement;
    uint8_t chance;
    uint8_t category;
};

struct Start {
    uint32_t recipeCount;
};

struct Info {
    int64_t price;
    int64_t priceCheque;
    int32_t priceAchievement;
    uint8_t chance;
};
#pragma pack()

};  // namespace game

namespace client {

enum {
    SUBHEADER_MAKE,
    SUBHEADER_CLOSE
};

#pragma pack(1)
struct Make {
    uint8_t id;
    uint16_t count;
    TItemPos position;
};
#pragma pack()

};  // namespace client

};  // namespace packets

using RecipeCollection = std::vector<Recipe>;
using RecipeCollectionMap = std::map<uint32_t, RecipeCollection>;

class Cube {
protected:
    static TEMP_BUFFER netBuffer;

public:
    Cube(CHARACTER& character, const RecipeCollection& recipes);
    virtual ~Cube();

    void Refresh(uint8_t id) const;

    bool Make(uint8_t id, TItemPos position, bool refresh = true);
    void MakeAll(uint8_t id, uint16_t count, TItemPos position);
	uint16_t CalculateMaxPossibleCrafts(const Recipe* recipe) const;

    void SendRecipesAndOpen() const;
    void SendRecipeInfo(const Recipe* recipe) const;
    void SendClose() const;

    CHARACTER& GetCharacter() const { return character_; }
    const RecipeCollection& GetRecipes() const { return recipes_; }

protected:
    const Recipe* FindMakeableRecipe(uint8_t id, bool ignorePrice = false) const;

protected:
    CHARACTER& character_;
    const RecipeCollection& recipes_;
};

using CubePtr = std::unique_ptr<Cube>;

class CubeManager : public singleton<CubeManager> {
public:
    static std::string GetCubeFilename();

public:
    CubeManager() = default;
    virtual ~CubeManager() = default;

    bool Initialize();
    bool Initialize(const std::string& filename);

    bool TryOpenByNpc(CHARACTER& character, uint32_t npc) const;

    int32_t ReceivePacket(CHARACTER& character, const char* data) const;

protected:
    RecipeCollectionMap recipesByNpc_;
};

};  // namespace cube


#include "stdafx.h"
#include "cube.h"

#include "char.h"
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
#include "locale_item_manager.h"
#endif
#include "char_manager.h"
#include "desc.h"
#include "DynamicPacket.hpp"
#include "item.h"
#include "item_manager.h"
#include "locale_service.h"
#include "questmanager.h"
#ifdef _ENABLE_BATTLEPASS_
#include "BattlePassManager.h"
#endif

#include "../../libxml/xml.hpp"
#include <fstream>


namespace cube {

bool Recipe::Item::Check(CHARACTER& ch) const {
	auto& item = *this;
	auto count = item.count;

	if (ch.FindSpecifyItem(item.vnum)) {
		if (ch.CountSpecifyItem(item.vnum) >= count)
			return true;
	}

	return false;
}

void Recipe::Item::Charge(CHARACTER& ch, CItem* reward) const {
	auto leftCount = count;

	for (uint32_t i = 0; leftCount > 0; ++i) {
		CItem* playerItem = ch.FindSpecifyItem(vnum);
		if (!playerItem || playerItem->GetVnum() != vnum) continue;

		if (reward && upgradeable)
			playerItem->CopyAttributeTo(reward);
		else if (reward && magicPct)
			reward->AlterToMagicItem();

		if (reward && sockets) playerItem->CopySocketTo(reward);

		if (!reward && !removeOnFailure) continue;

		auto newCount = 0;
		if (leftCount >= playerItem->GetCount()) {
			leftCount -= playerItem->GetCount();
		} else {
			newCount = playerItem->GetCount() - leftCount;
			leftCount = 0;
		}

		if (!playerItem->SetCount(newCount)) playerItem = nullptr;
	}
}

CItem* Recipe::Reward::Create() const {
	return ITEM_MANAGER::Instance().CreateItem(vnum, count);
}

CItem* Recipe::FailReward::Create() const {
	if (vnum == 0 || count == 0)
		return nullptr;

	return ITEM_MANAGER::Instance().CreateItem(vnum, count);
}

bool Recipe::Check(CHARACTER& character,
				bool ignorePrice) const {
	// Check if player has enough materials
	for (auto& item : items) {
		const TItemTable* itemTable = ITEM_MANAGER::instance().GetTable(item.vnum);
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		const std::string& localName = CLocaleItemManager::instance().Find(item.vnum, character.GetLanguage());
		const char* itemName = !localName.empty() ? localName.c_str() : (itemTable && itemTable->szLocaleName) ? itemTable->szLocaleName : "Unknown Item";
#else
		const char* itemName = (itemTable && itemTable->szLocaleName) ? itemTable->szLocaleName : "Unknown Item";
#endif

		if (!item.Check(character)) {
			// Send specific message about missing materials
			character.ChatPacket(CHAT_TYPE_INFO, "[LS;10084;%s;%d]", itemName, item.count);
			return false;
		}
	}

	if (!ignorePrice && (character.GetGold() < price)) {
		character.ChatPacket(CHAT_TYPE_INFO, "[LS;2410]");
		return false;
	}
	if (!ignorePrice && (character.GetCheque() < priceCheque))
	{
		character.ChatPacket(CHAT_TYPE_INFO, "[LS;2411]");
		return false;        
	}
	if (!ignorePrice && (character.GetPktOsiag() < priceAchievement))
	{
		character.ChatPacket(CHAT_TYPE_INFO, "[LS;2412]");
		return false;        
	}
	return true;
}

void Recipe::Charge(CHARACTER& character, CItem* reward) const {
	character.PointChange(POINT_GOLD, -price);
	character.PointChange(POINT_CHEQUE, -priceCheque);
	character.PointChange(POINT_PKT_OSIAG, -priceAchievement);

	for (auto& item : items)
		item.Charge(character, reward);
}

std::string CubeManager::GetCubeFilename() {
	return LocaleService_GetBasePath() + "/cube.xml";
}

bool CubeManager::Initialize() {
	return Initialize(GetCubeFilename());
}

bool CubeManager::Initialize(const std::string& filename) {
	// close all open cubes
	CHARACTER_MANAGER::Instance().for_each_pc([](CHARACTER* character) { character->ResetCube(); });

	// clear old data
	recipesByNpc_.clear();

	// create document
	xml::Document document(filename);

	// find root node
	auto rootNode = document.first_node("Recipes");
	if (!rootNode) {
		sys_err("Root node in cube.xml not found: Recipes");
		return false;
	}

	for (auto recipeNode = rootNode->first_node("Recipe"); recipeNode;
		recipeNode = recipeNode->next_sibling()) {
		// npc is required
		uint32_t npc;

		try {
			xml::GetAttribute(recipeNode, "npc", npc);
		} catch (const xml::Exception& e) {
			sys_err(e.what());
			continue;
		}

		Recipe recipe;

		// <Items />
		{
			const xml::Node* itemsNode;
			try {
				itemsNode = xml::GetNode(recipeNode, "Items");
			} catch (const xml::Exception& e) {
				sys_err(e.what());
				continue;
			}

			// <Item />
			{
				for (auto itemNode = itemsNode->first_node("Item"); itemNode;
					itemNode = itemNode->next_sibling()) {
					Recipe::Item item;

					// vnum is required
					try {
						xml::GetAttribute(itemNode, "vnum", item.vnum);
					} catch (const xml::Exception& e) {
						sys_err(e.what());
						continue;
					}

					xml::GetAttribute(itemNode, "count", uint16_t(1),
									item.count);
					xml::GetAttribute(itemNode, "upgradeable", false,
									item.upgradeable);
					xml::GetAttribute(itemNode, "sockets", false, item.sockets);
					xml::GetAttribute(itemNode, "magic-pct", false,
									item.magicPct);
					xml::GetAttribute(itemNode, "remove-on-failure", true,
									item.removeOnFailure);

					if (item.upgradeable && item.magicPct)
						sys_log(0, 
							"Cube item {} of npc {} is upgradeable and has the "
							"magic pct attribute, this is conflicting.",
							item.vnum, npc);

					recipe.items.push_back(item);
				}
			}
		}

		// <Reward />
		{
			const xml::Node* rewardNode;
			try {
				rewardNode = xml::GetNode(recipeNode, "Reward");
			} catch (const xml::Exception& e) {
				sys_err(e.what());
				continue;
			}

			try {
				xml::GetAttribute(rewardNode, "vnum", recipe.reward.vnum);
			} catch (const xml::Exception& e) {
				sys_err(e.what());
				continue;
			}

			xml::GetAttribute(rewardNode, "count", uint16_t(1),
							recipe.reward.count);
		}

		// <FailReward />
		{
			const xml::Node* failRewardNode =
				xml::TryGetNode(recipeNode, "FailReward");
			if (failRewardNode) {
				try {
					xml::GetAttribute(failRewardNode, "vnum",
									recipe.failReward.vnum);
				} catch (const xml::Exception& e) {
					sys_err(e.what());
					continue;
				}

				xml::GetAttribute(failRewardNode, "count", uint16_t(1),
								recipe.failReward.count);
			}
		}

		// <Data />
		{
			auto dataNode = xml::TryGetNode(recipeNode, "Data");
			if (dataNode) {
				xml::GetAttribute(dataNode, "category", uint8_t(0), recipe.category);
				xml::GetAttribute(dataNode, "price", int64_t(0), recipe.price);
				
				xml::GetAttribute(dataNode, "priceCheque", int64_t(0), recipe.priceCheque);
				xml::GetAttribute(dataNode, "priceAchievement", int32_t(0), recipe.priceAchievement);

				xml::GetAttribute(dataNode, "chance", uint8_t(100), recipe.chance);
			} else {
				recipe.price = 0;
				recipe.priceCheque = 0;
				recipe.priceAchievement = 0;
				recipe.chance = 100;
			}
		}

		// add npc if not found
		auto it = recipesByNpc_.find(npc);
		if (it == recipesByNpc_.end()) {
			auto ret = recipesByNpc_.emplace(npc, RecipeCollection());
			it = ret.first;
		}

		it->second.push_back(recipe);
	}

	return true;
}

bool CubeManager::TryOpenByNpc(CHARACTER& character, uint32_t npc) const {
	auto it = recipesByNpc_.find(npc);
	if (it == recipesByNpc_.end()) {
		return false;
	}

	auto cube = std::make_unique<cube::Cube>(character, it->second);
	cube->SendRecipesAndOpen();

	character.SetCube(std::move(cube));

	return true;
}

int32_t CubeManager::ReceivePacket(CHARACTER& character,
								const char* data) const {
	DynamicPacket packet(data);

	const auto& header = packet.Get<packets::Header>();
	switch (header.subheader) {
		case packets::client::SUBHEADER_MAKE: {
			auto& makePacket = packet.Get<packets::client::Make>();
			if (character.GetCube()) {
				if (makePacket.count > 1)
					character.GetCube()->MakeAll(makePacket.id, makePacket.count, makePacket.position);
				else
					character.GetCube()->Make(makePacket.id, makePacket.position);
			}

			break;
		}

		case packets::client::SUBHEADER_CLOSE: {
			character.ResetCube();
			break;
		}
	}

	return packet.GetExtraSize() - sizeof(packets::Header);
}

TEMP_BUFFER Cube::netBuffer(1024);

Cube::Cube(CHARACTER& character, const RecipeCollection& recipes)
	: character_(character), recipes_(recipes) {}

Cube::~Cube() {
	SendClose();
}

void Cube::Refresh(uint8_t id) const {
	SendRecipeInfo(FindMakeableRecipe(id, true));
}

bool Cube::Make(uint8_t id, TItemPos position, bool refresh) {
	auto recipe = FindMakeableRecipe(id);
	if (!recipe) return false;

	// Checking if we fill requirements
	if (!recipe->Check(GetCharacter())) return false;

	uint8_t chance = recipe->chance;

	if (position.IsValidItemPosition()) {
		auto cItem = GetCharacter().GetItem(position);
		if (cItem) {
			if (cItem->GetVnum() == 71084) {
				GetCharacter().RemoveSpecifyItem(71084);
				chance += 20;
			}
		}
	}

	// evaluate chance and create reward item if successful
	bool isFail = false;
	CItem* reward = nullptr;
	if (number(1, 100) <= chance) {
		reward = recipe->reward.Create();
	} else {
		isFail = true;
		reward = recipe->failReward.Create();
	}

	// Check if we can add the reward to inventory BEFORE consuming materials
	if (reward) {
		int iEmptyPos = GetCharacter().GetEmptyInventory(reward->GetSize());
		
		if (iEmptyPos != -1) {
			// We have space, proceed with charging materials and gold
#ifdef _ENABLE_BATTLEPASS_
			if (!isFail)
				BattlePassManager::Instance().Notify(MISSION_TYPE_CRAFT, &GetCharacter(), reward->GetVnum(), 1);
#endif
			recipe->Charge(GetCharacter(), reward);
			
			// Add reward to inventory
			GetCharacter().GetInventory().Add(reward);
			
			if (isFail) {
				GetCharacter().ChatPacket(CHAT_TYPE_INFO, "[LS;10082]");
			} else {
				GetCharacter().ChatPacket(CHAT_TYPE_INFO, "[LS;10088]"); // Crafting success message
			}
		} else {
			// No space in inventory
			M2_DESTROY_ITEM(reward);
			GetCharacter().ChatPacket(CHAT_TYPE_INFO, "[LS;1229]"); // Inventory full message
			return false;
		}
	} else {
		// No reward created, still charge materials (for failed crafts with no fail reward)
		recipe->Charge(GetCharacter(), nullptr);
		GetCharacter().ChatPacket(CHAT_TYPE_INFO, "[LS;10082]"); // Crafting failed message
	}

	//quest::CQuestManager::Instance().Craft(GetCharacter().GetPlayerID(), reward);

	if (refresh)
		Refresh(id);

	return true;
}

uint16_t Cube::CalculateMaxPossibleCrafts(const Recipe* recipe) const {
	if (!recipe) return 0;
	
	uint16_t maxCrafts = UINT16_MAX;
	
	// Check each required item
	for (const auto& item : recipe->items) {
		uint32_t availableCount = GetCharacter().CountSpecifyItem(item.vnum);
		uint16_t possibleCrafts = availableCount / item.count;
		maxCrafts = std::min(maxCrafts, possibleCrafts);
	}
	
	// Check gold limitation
	if (recipe->price > 0) {
		uint16_t goldCrafts = GetCharacter().GetGold() / recipe->price;
		maxCrafts = std::min(maxCrafts, goldCrafts);
	}
	
	// Check cheque limitation
	if (recipe->priceCheque > 0) {
		uint16_t chequeCrafts = GetCharacter().GetCheque() / recipe->priceCheque;
		maxCrafts = std::min(maxCrafts, chequeCrafts);
	}
	
	// Check achievement points limitation
	if (recipe->priceAchievement > 0) {
		uint16_t achievementCrafts = GetCharacter().GetPktOsiag() / recipe->priceAchievement;
		maxCrafts = std::min(maxCrafts, achievementCrafts);
	}
	
	return maxCrafts == UINT16_MAX ? 0 : maxCrafts;
}

void Cube::MakeAll(uint8_t id, uint16_t count, TItemPos position) {
	auto recipe = FindMakeableRecipe(id);
	
	if (!recipe) {
		GetCharacter().ChatPacket(CHAT_TYPE_INFO, "[LS;10085]"); // Invalid recipe
		return;
	}
		// Pre-check if we have enough materials for at least one craft
	if (!recipe->Check(GetCharacter())) {
		return; // Check method will send appropriate error message
	}
	
	// Calculate maximum possible crafts based on materials
	uint16_t maxPossibleCrafts = CalculateMaxPossibleCrafts(recipe);
	uint16_t actualCount = std::min(count, maxPossibleCrafts);
	
	if (actualCount == 0) {
		GetCharacter().ChatPacket(CHAT_TYPE_INFO, "[LS;2416]"); // No items could be crafted
		return;
	}

#ifdef __INVENTORY_BUFFERING__
	// Enable inventory buffering for instant bulk crafting
	GetCharacter().SetInventoryBuffer(true);
#endif
	
	uint16_t counter = 0;
	uint16_t maxIterations = 10000; // Safety limit to prevent infinite loops
	
	while (counter < actualCount && maxIterations > 0) {
		// Try to make the item
		if (!Make(id, position, false)) {
			// If Make fails (likely inventory full), stop crafting
			break;
		}
		
		++counter;
		--maxIterations;
	}

#ifdef __INVENTORY_BUFFERING__
	// Send all buffered inventory updates at once
	GetCharacter().SendBufferedInventoryPacket();
#endif

	// Refresh cube UI once at the end
	Refresh(id);
	
	if (counter > 0) {
		GetCharacter().ChatPacket(CHAT_TYPE_INFO, "[LS;10081;%d]", counter);
	} else {
		GetCharacter().ChatPacket(CHAT_TYPE_INFO, "[LS;10086]"); // No items could be crafted
	}
	
	if (maxIterations <= 0) {
		sys_log(0, "CubeManager::MakeAll: Hit iteration limit for player %s", GetCharacter().GetName());
	}
}

void Cube::SendRecipesAndOpen() const {
	auto desc = GetCharacter().GetDesc();
	if (!desc)
		return;

	netBuffer.reset();

	for (const auto& recipe : GetRecipes()) {
		packets::game::Recipe recipePacket(recipe);
		netBuffer.write(&recipePacket);
	
		for (const auto& item : recipe.items) {
			packets::game::Item itemPacket(item);
			netBuffer.write(&itemPacket);
		}
	}

	packets::Header headerPacket;
	headerPacket.header = HEADER_GC_CUBE;
	headerPacket.size = sizeof(packets::Header) + sizeof(packets::game::Start) +netBuffer.size();
	headerPacket.subheader = packets::game::SUBHEADER_START;

	packets::game::Start startPacket;
	startPacket.recipeCount = GetRecipes().size();

	desc->BufferedPacket(&headerPacket);
	desc->BufferedPacket(&startPacket);
	desc->Packet(netBuffer.read_peek(), netBuffer.size());
}

void Cube::SendRecipeInfo(const Recipe* recipe) const {
	auto desc = GetCharacter().GetDesc();
	if (!desc)
		return;

	packets::Header headerPacket;
	headerPacket.header = HEADER_GC_CUBE;
	headerPacket.size = sizeof(packets::Header) + sizeof(packets::game::Info);
	headerPacket.subheader = packets::game::SUBHEADER_INFO;

	packets::game::Info infoPacket;
	if (recipe) {
		infoPacket.price = recipe->price;
		infoPacket.priceCheque = recipe->priceCheque;
		infoPacket.priceAchievement = recipe->priceAchievement;
		infoPacket.chance = recipe->chance;
	} else {
		infoPacket.price = 0;
		infoPacket.priceCheque = 0;
		infoPacket.priceAchievement = 0;
		infoPacket.chance = 0;
	}

	desc->BufferedPacket(&headerPacket);
	desc->Packet(&infoPacket);
}

void Cube::SendClose() const {
	auto desc = GetCharacter().GetDesc();
	if (!desc)
		return;

	packets::Header headerPacket;
	headerPacket.header = HEADER_GC_CUBE;
	headerPacket.size = sizeof(packets::Header);
	headerPacket.subheader = packets::game::SUBHEADER_CLOSE;

	desc->Packet(&headerPacket);
}

const Recipe* Cube::FindMakeableRecipe(const uint8_t id, bool ignorePrice) const {
	auto& recipes = GetRecipes();
	if (id >= recipes.size())
		return nullptr;

	return &recipes.at(id);
}

};  // namespace cube

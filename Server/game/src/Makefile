CC = clang
CXX = clang++

BINDIR = ..
OBJDIR = .obj
MAIN = main
GAME_VERSION := $(shell cat ../../__REVISION__)
TARGET = $(BINDIR)/game_r

$(shell mkdir -p $(OBJDIR))

CPPFILES = \
	affect.cpp \
	ani.cpp \
	arena.cpp \
	AuxiliaryStringWrapper.cpp \
	banword.cpp \
	battle.cpp \
	blend_item.cpp \
	BlueDragon_Binder.cpp \
	BlueDragon.cpp \
	buff_npc_system.cpp \
	buff_on_attributes.cpp \
	buffer_manager.cpp \
	building.cpp \
	castle.cpp \
	char_affect.cpp \
	char_battle.cpp \
	char_cards.cpp \
	char_change_empire.cpp \
	char_dragonsoul.cpp \
	char_horse.cpp \
	char_item.cpp \
	char_manager.cpp \
	char_quickslot.cpp \
	char_resist.cpp \
	char_skill.cpp \
	char_state.cpp \
	char_titles.cpp \
	char_weekly.cpp \
	char.cpp \
	cipher.cpp \
	ClientPackageCryptInfo.cpp \
	cmd_emotion.cpp \
	cmd_general.cpp \
	cmd_gm.cpp \
	cmd_oxevent.cpp \
	cmd.cpp \
	CollectionsSystem.cpp \
	config.cpp \
	constants.cpp \
	crc32.cpp \
	cube.cpp \
	db.cpp \
	desc_client.cpp \
	desc_manager.cpp \
	desc_p2p.cpp \
	desc.cpp \
	dev_log.cpp \
	dragon_soul_table.cpp \
	DragonLair.cpp \
	DragonSoul.cpp \
	dungeon_info.cpp \
	dungeon.cpp \
	DungeonInfoManager.cpp \
	empire_text_convert.cpp \
	entity_view.cpp \
	entity.cpp \
	event_queue.cpp \
	event.cpp \
	exchange.cpp \
	file_loader.cpp \
	FileMonitor_FreeBSD.cpp \
	fish_wiki.cpp \
	fishing.cpp \
	FSM.cpp \
	glove_bonus_manager.cpp \
	gm.cpp \
	group_text_parse_tree.cpp \
	guild_manager.cpp \
	guild_war.cpp \
	guild.cpp \
	horse_rider.cpp \
	horsename_manager.cpp \
	input_auth.cpp \
	input_db.cpp \
	input_login.cpp \
	input_main.cpp \
	input_p2p.cpp \
	input.cpp \
	Inventory.cpp \
	ip_ban.cpp \
	item_addon.cpp \
	item_apply_random_table.cpp \
	item_attribute.cpp \
	item_manager_idrange.cpp \
	item_manager_read_tables.cpp \
	item_manager.cpp \
	item.cpp \
	ItemToggle.cpp \
	locale_service.cpp \
	locale.cpp \
	log.cpp \
	login_data.cpp \
	lzo_manager.cpp \
	main.cpp \
	map_location.cpp \
	MarkConvert.cpp \
	MarkImage.cpp \
	MarkManager.cpp \
	marriage.cpp \
	matrix_card.cpp \
	MemoryBuffer.cpp \
	messenger_manager.cpp \
	mining.cpp \
	mob_manager.cpp \
	monarch.cpp \
	MonsterPickuper.cpp \
	motion.cpp \
	MountSystem.cpp \
	new_offlineshop_manager.cpp \
	new_offlineshop_safebox.cpp \
	new_offlineshop.cpp \
	offline_shop.cpp \
	offlineshop_config.cpp \
	offlineshop_manager.cpp \
	over9refine.cpp \
	OXEvent.cpp \
	p2p.cpp \
	packet_info.cpp \
	panama.cpp \
	party.cpp \
	PetSystem.cpp \
	polymorph.cpp \
	PolySystem.cpp \
	priv_manager.cpp \
	ProtoProcessor.cpp \
	pvp.cpp \
	questevent.cpp \
	questlua_affect.cpp \
	questlua_arena.cpp \
	questlua_building.cpp \
	questlua_danceevent.cpp \
	questlua_dnd.cpp \
	questlua_dragonlair.cpp \
	questlua_dragonsoul.cpp \
	questlua_dungeon.cpp \
	questlua_forked.cpp \
	questlua_game.cpp \
	questlua_global.cpp \
	questlua_guild.cpp \
	questlua_horse.cpp \
	questlua_item.cpp \
	questlua_marriage.cpp \
	questlua_mgmt.cpp \
	questlua_monarch.cpp \
	questlua_npc.cpp \
	questlua_oxevent.cpp \
	questlua_party.cpp \
	questlua_pc.cpp \
	questlua_pet.cpp \
	questlua_quest.cpp \
	questlua_speedserver.cpp \
	questlua_target.cpp \
	questlua.cpp \
	questmanager.cpp \
	questnpc.cpp \
	questpc.cpp \
	ranking_manager.cpp \
	RealTimeRegen.cpp \
	refine.cpp \
	regen.cpp \
	resp_manager.cpp \
	safebox.cpp \
	sectree_manager.cpp \
	sectree.cpp \
	sequence.cpp \
	server_ranking_manager.cpp \
	shop_manager.cpp \
	shop.cpp \
	shopEx.cpp \
	shutdown_manager.cpp \
	skill_power.cpp \
	skill.cpp \
	SpeedServer.cpp \
	start_position.cpp \
	switchbot.cpp \
	target.cpp \
	TeamList.cpp \
	text_file_loader.cpp \
	threeway_war.cpp \
	TrafficProfiler.cpp \
	trigger.cpp \
	utils.cpp \
	vector.cpp \
	VersionManager.cpp \
	vid.cpp \
	war_map.cpp \
	wedding.cpp \
	worldboss.cpp \
	xmas_event.cpp \
	abuse.cpp \
	Controller.cpp \
	PacketAnalyzer.cpp \
	summer_hwid_manager.cpp \
	EventFunctionHandler.cpp \
	InGameLogManager.cpp \
	pvp_ranking.cpp \
	AchievementTitle.cpp \
	MonsterHighscore.cpp \
	MonsterSpawner.cpp \
	BattlePassManager.cpp \
	locale_item_manager.cpp


CFLAGS = -m32 -g -Wall -O2 -pipe -fexceptions -fno-strict-aliasing -pthread -D_THREAD_SAFE -DNDEBUG
CFLAGS += -Wno-deprecated-declarations -Wno-array-bounds -Wno-address
CFLAGS += -Wno-int-in-bool-context -Wno-sign-compare
CFLAGS += -Wno-deprecated-enum-enum-conversion -Wno-invalid-source-encoding
CFLAGS += -fstack-protector-all -Wmissing-braces
CFLAGS += -D__USER__=\"$(USER)\" -D__HOSTNAME__=\"$(HOSTNAME)\" -D__PWD__=\"$(PWD)\" -D__GAME_VERSION__=\"$(GAME_VERSION)\"

CXXFLAGS = -std=c++2b

INCLUDES = \
	-I../../../Extern/include \
	-I../../../Extern/include/boost \
	-I../../../Extern/include/IL \
	-I/usr/local/include \
	-I/usr/local/include/eigen3 \
	-I/usr/include \
	-I../../liblua/5.0/include

LIBDIRS = \
	-L../../../Extern/lib \
	-L/usr/local/lib \
	-L../../libthecore/lib \
	-L../../libpoly \
	-L../../libsql \
	-L../../libxml \
	-L../../libgame/lib \
	-L../../liblua/5.0/lib \
	-L/usr/local/lib/mysql

LIBS = \
	-lIL \
	-ljasper \
	-lpng \
	-ltiff \
	-ljbig \
	-lmng \
	-llcms \
	-llzma \
	-ljpeg \
	-lmysqlclient \
	-lxml \
	../../liblua/5.0/lib/liblua.a \
	../../liblua/5.0/lib/liblualib.a \
	/elvion-src/Extern/lib/libcryptopp.a \
	-lthecore -lpoly -lsql -lgame -lm -lmd -lssl -lcrypto -lz -lstdc++

CFILES = minilzo.c
COBJS = $(addprefix $(OBJDIR)/, $(CFILES:.c=.o))
OBJS = $(addprefix $(OBJDIR)/, $(CPPFILES:.cpp=.o)) $(COBJS)

$(OBJDIR)/%.o: %.c
	@echo [CC] $<
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Build modes
ifeq ($(BUILDTYPE),debug)
  CFLAGS += -O0 -DDEBUG
  BINDIR := $(BINDIR)/debug
else ifeq ($(BUILDTYPE),release)
  CFLAGS += -O2 -DNDEBUG
  BINDIR := $(BINDIR)/release
else ifeq ($(BUILDTYPE),asan)
  CFLAGS += -O1 -fsanitize=address -fno-omit-frame-pointer -g
  BINDIR := $(BINDIR)/asan
else
  # Default to release if BUILDTYPE is not set
  BINDIR := $(BINDIR)/release
endif

# Ensure the output directory exists for asan build
$(shell mkdir -p $(BINDIR))

# Rule to compile the .cpp files to object files
$(OBJDIR)/%.o: %.cpp
	@echo [CXX] $<
	@$(CXX) $(CFLAGS) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Main build target
all: $(TARGET)

$(TARGET): $(OBJS)
	@echo [LD] $@
	@$(CXX) $(CFLAGS) $(CXXFLAGS) $(LIBDIRS) $^ $(LIBS) -o $(TARGET)

# Clean target, works regardless of BUILDTYPE
clean:
	@echo Cleaning up...
	@rm -rf $(OBJDIR)/*.o $(BINDIR)/game_r

strip:
	@cp $(TARGET) $(BINDIR)/game
	@strip $(BINDIR)/game

dep:

symlink:

CC = clang
CXX = clang++

DB_VERSION := $(shell cat ../../__REVISION__ 2>/dev/null || echo 0)
BINDIR = ..
OBJDIR = .obj
BUILDTYPE ?= release

$(shell mkdir -p $(OBJDIR))

# Build-type-specific flags
ifeq ($(BUILDTYPE),debug)
  CFLAGS   = -g -Wall -O0 -DDEBUG
  BINDIR   := $(BINDIR)/debug
else ifeq ($(BUILDTYPE),release)
  CFLAGS   = -O2 -Wall -DNDEBUG
  BINDIR   := $(BINDIR)/release
else ifeq ($(BUILDTYPE),asan)
  CFLAGS   = -g -O1 -fsanitize=address -fno-omit-frame-pointer -Wall
  BINDIR   := $(BINDIR)/asan
else
  $(error Unknown BUILDTYPE '$(BUILDTYPE)', must be 'debug', 'release', or 'asan')
endif

# General flags
CFLAGS  += -pipe -pthread -D_THREAD_SAFE
CFLAGS  += -Wno-long-long -Wno-deprecated-declarations -Wno-array-bounds -Wno-address
CFLAGS  += -Wno-int-in-bool-context -Wno-format-truncation -Wno-deprecated-enum-enum-conversion
CFLAGS  += -Wno-invalid-source-encoding -Wno-unknown-warning-option
CXXFLAGS = -std=c++20

# Version defines
CFLAGS += -D__USER__=\"$(USER)\" -D__HOSTNAME__=\"$(HOSTNAME)\" -D__PWD__=\"$(PWD)\" -D__DB_VERSION__=\"$(DB_VERSION)\"

# Include & lib directories
INCDIR  = -I/usr/local/include -I/usr/local/include/mysql -I../../../Extern/include
LIBDIR  = -L../../../Extern/lib -L/usr/local/lib -L/usr/local/lib/mysql \
          -L../../libthecore/lib -L../../libsql -L../../libpoly -L../../libgame/lib

# LIBS
LIBS    = -lthecore -lsql -lpoly -lgame -lmysqlclient -lz -lssl -lcrypto -lm \
          ../../../Extern/lib/libcryptopp.a

# Sources
CPPFILE = \
	AuxiliaryStringWrapper.cpp \
	Cache.cpp \
	ClientManager.cpp \
	ClientManagerBoot.cpp \
	ClientManagerEventFlag.cpp \
	ClientManagerGuild.cpp \
	ClientManagerHorseName.cpp \
	ClientManagerLogin.cpp \
	ClientManagerOfflineshop.cpp \
	ClientManagerParty.cpp \
	ClientManagerPlayer.cpp \
	Config.cpp \
	CsvReader.cpp \
	DBManager.cpp \
	DungeonInfoRankingManager.cpp \
	grid.cpp \
	GuildManager.cpp \
	ItemAwardManager.cpp \
	ItemIDRangeManager.cpp \
	Lock.cpp \
	LoginData.cpp \
	Main.cpp \
	Marriage.cpp \
	Monarch.cpp \
	MoneyLog.cpp \
	NetBase.cpp \
	OfflineshopCache.cpp \
	Peer.cpp \
	PeerBase.cpp \
	PrivManager.cpp \
	ProtoReader.cpp \
	InGameLog.cpp

CPPOBJS = $(CPPFILE:%.cpp=$(OBJDIR)/%.o)
CPPDEPS = $(CPPOBJS:%.o=%.d)
MAIN_TARGET = $(BINDIR)/db

# Targets
all: $(MAIN_TARGET)

debug:
	@$(MAKE) BUILDTYPE=debug

release:
	@$(MAKE) BUILDTYPE=release

asan:
	@$(MAKE) BUILDTYPE=asan

$(MAIN_TARGET): $(CPPOBJS)
	@echo [LD] $@
	@mkdir -p $(BINDIR)
	@$(CXX) $(CFLAGS) $(CXXFLAGS) $(LIBDIR) $(CPPOBJS) $(LIBS) -o $@

$(OBJDIR)/%.o: %.cpp
	@echo [CXX] $<
	@$(CXX) $(CFLAGS) $(CXXFLAGS) $(INCDIR) -c $< -o $@
	@$(CXX) -MM -MG -MP $(CFLAGS) $(CXXFLAGS) $(INCDIR) $< > $(OBJDIR)/$*.d
	@sed -i '' -e 's|^.*:|$(OBJDIR)/$*.o:|' $(OBJDIR)/$*.d 2>/dev/null || true

clean:
	@echo Cleaning DB...
	@rm -f $(CPPOBJS) $(CPPDEPS) $(MAIN_TARGET)

dep:

symlink:

-include $(CPPDEPS)

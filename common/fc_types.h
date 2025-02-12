/**********************************************************************
 Freeciv - Copyright (C) 2004 - The Freeciv Project
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
***********************************************************************/

#ifndef FC__FC_TYPES_H
#define FC__FC_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "bitvector.h"
#include "shared.h"

/* This file serves to reduce the cross-inclusion of header files which
 * occurs when a type which is defined in one file is needed for a function
 * definition in another file.
 *
 * Nothing in this file should require anything else from the common/
 * directory! */

#define MAX_NUM_PLAYER_SLOTS 128 /* Used in the network protocol. */
#define MAX_NUM_BARBARIANS   2
#define MAX_NUM_PLAYERS      MAX_NUM_PLAYER_SLOTS - MAX_NUM_BARBARIANS
/* Used in the network protocol. */
#define MAX_NUM_CONNECTIONS (2 * (MAX_NUM_PLAYER_SLOTS))
/* e.g. unit_types. Used in the network protocol. */
#define MAX_NUM_ITEMS   200
#define MAX_NUM_TECH_LIST 10 /* Used in the network protocol. */
#define MAX_NUM_UNIT_LIST 10 /* Used in the network protocol. */
#define MAX_NUM_BUILDING_LIST 10 /* Used in the network protocol. */
#define MAX_LEN_VET_SHORT_NAME 8
/* Used in the network protocol. See diplomat_success_vs_defender() */
#define MAX_VET_LEVELS 20
#define MAX_BASE_TYPES 32 /* Used in the network protocol. */
#define MAX_ROAD_TYPES 8 /* Used in the network protocol. */
#define MAX_DISASTER_TYPES 10
#define MAX_NUM_LEADERS MAX_NUM_ITEMS /* Used in the network protocol. */
#define MAX_NUM_NATION_SETS 32 /* Used in the network protocol.
                                * RULESET_NATION_SETS packet may become too big
                                * if increased */
#define MAX_NUM_NATION_GROUPS 128 /* Used in the network protocol. */
/* Used in the network protocol -- nation count is a UINT16 */
#define MAX_NUM_NATIONS MAX_UINT16
#define MAX_NUM_STARTPOS_NATIONS 1024 /* Used in the network protocol. */

/* Changing these will probably break network compatability. */
#define MAX_LEN_NAME     48
#define MAX_LEN_DEMOGRAPHY 16
#define MAX_LEN_ALLOW_TAKE 16
#define MAX_LEN_GAME_IDENTIFIER 33
#define MAX_GRANARY_INIS 24
#define MAX_LEN_STARTUNIT (20+1)
#define MAX_LEN_ENUM     64

/* Line breaks after this number of characters; be carefull and use only 70 */
#define LINE_BREAK 70

/* symbol to flag missing numbers for better debugging */
#define IDENTITY_NUMBER_ZERO (0)

/* A bitvector for all player slots. Used in the network protocol. */
BV_DEFINE(bv_player, MAX_NUM_PLAYER_SLOTS);

/* Changing this breaks network compatibility. */
enum output_type_id {
  O_FOOD, O_SHIELD, O_TRADE, O_GOLD, O_LUXURY, O_SCIENCE, O_LAST
};

/* Changing this enum will break savegame and network compatability. */
#define SPECENUM_NAME unit_activity
#define SPECENUM_VALUE0 ACTIVITY_IDLE
#define SPECENUM_VALUE0NAME "Idle"
#define SPECENUM_VALUE1 ACTIVITY_POLLUTION
#define SPECENUM_VALUE1NAME "Pollution"
#define SPECENUM_VALUE2 ACTIVITY_OLD_ROAD
#define SPECENUM_VALUE2NAME "Unused Road"
#define SPECENUM_VALUE3 ACTIVITY_MINE
#define SPECENUM_VALUE3NAME "Mine"
#define SPECENUM_VALUE4 ACTIVITY_IRRIGATE
#define SPECENUM_VALUE4NAME "Irrigate"
#define SPECENUM_VALUE5 ACTIVITY_FORTIFIED
#define SPECENUM_VALUE5NAME "Fortified"
#define SPECENUM_VALUE6 ACTIVITY_FORTRESS
#define SPECENUM_VALUE6NAME "Fortress"
#define SPECENUM_VALUE7 ACTIVITY_SENTRY
#define SPECENUM_VALUE7NAME "Sentry"
#define SPECENUM_VALUE8 ACTIVITY_OLD_RAILROAD
#define SPECENUM_VALUE8NAME "Unused Railroad"
#define SPECENUM_VALUE9 ACTIVITY_PILLAGE
#define SPECENUM_VALUE9NAME "Pillage"
#define SPECENUM_VALUE10 ACTIVITY_GOTO
#define SPECENUM_VALUE10NAME "Goto"
#define SPECENUM_VALUE11 ACTIVITY_EXPLORE
#define SPECENUM_VALUE11NAME "Explore"
#define SPECENUM_VALUE12 ACTIVITY_TRANSFORM
#define SPECENUM_VALUE12NAME "Transform"
#define SPECENUM_VALUE13 ACTIVITY_UNKNOWN
#define SPECENUM_VALUE13NAME "Unused"
#define SPECENUM_VALUE14 ACTIVITY_AIRBASE
#define SPECENUM_VALUE14NAME "Unused Airbase"
#define SPECENUM_VALUE15 ACTIVITY_FORTIFYING
#define SPECENUM_VALUE15NAME "Fortifying"
#define SPECENUM_VALUE16 ACTIVITY_FALLOUT
#define SPECENUM_VALUE16NAME "Fallout"
#define SPECENUM_VALUE17 ACTIVITY_PATROL_UNUSED
#define SPECENUM_VALUE17NAME "Unused Patrol"
#define SPECENUM_VALUE18 ACTIVITY_BASE
#define SPECENUM_VALUE18NAME "Base"
#define SPECENUM_VALUE19 ACTIVITY_GEN_ROAD
#define SPECENUM_VALUE19NAME "Road"
#define SPECENUM_VALUE20 ACTIVITY_CONVERT
#define SPECENUM_VALUE20NAME "Convert"
#define SPECENUM_COUNT ACTIVITY_LAST
#include "specenum_gen.h"

enum adv_unit_task { AUT_NONE, AUT_AUTO_SETTLER, AUT_BUILD_CITY };

typedef signed short Continent_id;
typedef int Terrain_type_id;
typedef int Resource_type_id;
typedef int Specialist_type_id;
typedef int Impr_type_id;
typedef int Tech_type_id;
typedef enum output_type_id Output_type_id;
typedef enum unit_activity Activity_type_id;
typedef int Nation_type_id;
typedef int Unit_type_id;
typedef int Base_type_id;
typedef int Road_type_id;
typedef int Disaster_type_id;
typedef unsigned char citizens;

struct advance;
struct city;
struct connection;
struct government;
struct impr_type;
struct nation_type;
struct output_type;
struct player;
struct specialist;
struct terrain;
struct tile;
struct unit;


/* Changing these will break network compatibility. */
#define SP_MAX 20
#define MAX_NUM_REQS 10

#define MAX_NUM_RULESETS 16 /* Used in the network protocol. */
#define MAX_RULESET_NAME_LENGTH 64 /* Used in the network protocol. */
#define RULESET_SUFFIX ".serv"

/* Unit Class List, also 32-bit vector? */
#define UCL_LAST 32 /* Used in the network protocol. */
typedef int Unit_Class_id;

/* This has to be put here for now, otherwise movement.h and unittype.h
 * would have a recursive dependency. */
#define SPECENUM_NAME unit_move_type
#define SPECENUM_VALUE0 UMT_LAND
#define SPECENUM_VALUE0NAME "Land"
#define SPECENUM_VALUE1 UMT_SEA
#define SPECENUM_VALUE1NAME "Sea"
#define SPECENUM_VALUE2 UMT_BOTH
#define SPECENUM_VALUE2NAME "Both"
#include "specenum_gen.h"


/* The direction8 gives the 8 possible directions.  These may be used in
 * a number of ways, for instance as an index into the DIR_DX/DIR_DY
 * arrays.  Not all directions may be valid; see is_valid_dir and
 * is_cardinal_dir. */

/* The DIR8/direction8 naming system is used to avoid conflict with
 * DIR4/direction4 in client/tilespec.h
 *
 * Changing the order of the directions will break network compatability.
 *
 * Some code assumes that the first 4 directions are the reverses of the
 * last 4 (in no particular order).  See client/goto.c and
 * map.c:opposite_direction(). */

/* Used in the network protocol. */
#define SPECENUM_NAME direction8
#define SPECENUM_VALUE0 DIR8_NORTHWEST
#define SPECENUM_VALUE0NAME "Northwest"
#define SPECENUM_VALUE1 DIR8_NORTH
#define SPECENUM_VALUE1NAME "North"
#define SPECENUM_VALUE2 DIR8_NORTHEAST
#define SPECENUM_VALUE2NAME "Northeast"
#define SPECENUM_VALUE3 DIR8_WEST
#define SPECENUM_VALUE3NAME "West"
#define SPECENUM_VALUE4 DIR8_EAST
#define SPECENUM_VALUE4NAME "East"
#define SPECENUM_VALUE5 DIR8_SOUTHWEST
#define SPECENUM_VALUE5NAME "Southwest"
#define SPECENUM_VALUE6 DIR8_SOUTH
#define SPECENUM_VALUE6NAME "South"
#define SPECENUM_VALUE7 DIR8_SOUTHEAST
#define SPECENUM_VALUE7NAME "Southeast"
#define SPECENUM_INVALID ((enum direction8)(DIR8_SOUTHEAST + 1))
#include "specenum_gen.h"

/* Used in the network protocol. */
#define SPECENUM_NAME free_tech_method
#define SPECENUM_VALUE0 FTM_GOAL
#define SPECENUM_VALUE0NAME "Goal"
#define SPECENUM_VALUE1 FTM_RANDOM
#define SPECENUM_VALUE1NAME "Random"
#define SPECENUM_VALUE2 FTM_CHEAPEST
#define SPECENUM_VALUE2NAME "Cheapest"
#include "specenum_gen.h"

/* Some code requires compile time value for number of directions, and
 * cannot use specenum function call direction8_max(). */
#define DIR8_MAGIC_MAX 8

/* AI levels. This must correspond to ai_level_names[] in player.c */
enum ai_level {
  AI_LEVEL_AWAY         = 1,
  AI_LEVEL_NOVICE       = 2,
  AI_LEVEL_EASY         = 3,
  AI_LEVEL_NORMAL       = 5,
  AI_LEVEL_HARD         = 7,
  AI_LEVEL_CHEATING     = 8,
  AI_LEVEL_EXPERIMENTAL = 10,
  AI_LEVEL_LAST
};

#define AI_LEVEL_DEFAULT AI_LEVEL_NOVICE

/*
 * pplayer->ai.barbarian_type and nations use this enum. Note that the values
 * have to stay since they are used in savegames.
 */
enum barbarian_type {
  NOT_A_BARBARIAN = 0,
  LAND_BARBARIAN = 1,
  SEA_BARBARIAN = 2
};

/*
 * Citytile requirement types.
 */
enum citytile_type {
  CITYT_CENTER,
  CITYT_LAST
};

/* Sometimes we don't know (or don't care) if some requirements for effect
 * are currently fulfilled or not. This enum tells lower level functions
 * how to handle uncertain requirements.
 */
enum req_problem_type {
  RPT_POSSIBLE, /* We want to know if it is possible that effect is active */
  RPT_CERTAIN   /* We want to know if it is certain that effect is active  */
};

#define REVERSED_RPT(x) \
  (x == RPT_CERTAIN ? RPT_POSSIBLE : RPT_CERTAIN)

/* Originally in requirements.h, bumped up and revised to unify with
 * city_production and worklists.  Functions remain in requirements.c
 * Used in the network protocol. */
typedef union {
  struct advance *advance;
  struct government *govern;
  struct impr_type *building;
  struct nation_type *nation;
  struct nation_type *nationality;
  struct specialist *specialist;
  struct terrain *terrain;
  struct unit_class *uclass;
  struct unit_type *utype;
  struct base_type *base;
  struct road_type *road;
  struct resource *resource;

  enum ai_level ai_level;
  enum citytile_type citytile;
  int minsize;
  int minyear;
  Output_type_id outputtype;
  int terrainclass;			/* enum terrain_class */
  int terrainalter;                     /* enum terrain_alteration */
  int special;				/* enum tile_special_type */
  int unitclassflag;			/* enum unit_class_flag_id */
  int unitflag;				/* enum unit_flag_id */
  int terrainflag;                      /* enum terrain_flag_id */
  int techflag;                         /* enum tech_flag_id */
} universals_u;

/* The kind of universals_u (value_union_type was req_source_type).
 * Used in the network protocol. */
#define SPECENUM_NAME universals_n
#define SPECENUM_VALUE0 VUT_NONE
#define SPECENUM_VALUE0NAME "None"
#define SPECENUM_VALUE1 VUT_ADVANCE
#define SPECENUM_VALUE1NAME "Tech"
#define SPECENUM_VALUE2 VUT_GOVERNMENT
#define SPECENUM_VALUE2NAME "Gov"
#define SPECENUM_VALUE3 VUT_IMPROVEMENT
#define SPECENUM_VALUE3NAME "Building"
#define SPECENUM_VALUE4 VUT_SPECIAL
#define SPECENUM_VALUE4NAME "Special"
#define SPECENUM_VALUE5 VUT_TERRAIN
#define SPECENUM_VALUE5NAME "Terrain"
#define SPECENUM_VALUE6 VUT_NATION
#define SPECENUM_VALUE6NAME "Nation"
#define SPECENUM_VALUE7 VUT_UTYPE
#define SPECENUM_VALUE7NAME "UnitType"
#define SPECENUM_VALUE8 VUT_UTFLAG
#define SPECENUM_VALUE8NAME "UnitFlag"
#define SPECENUM_VALUE9 VUT_UCLASS
#define SPECENUM_VALUE9NAME "UnitClass"
#define SPECENUM_VALUE10 VUT_UCFLAG
#define SPECENUM_VALUE10NAME "UnitClassFlag"
#define SPECENUM_VALUE11 VUT_OTYPE
#define SPECENUM_VALUE11NAME "OutputType"
#define SPECENUM_VALUE12 VUT_SPECIALIST
#define SPECENUM_VALUE12NAME "Specialist"
/* Minimum size: at city range means city size */
#define SPECENUM_VALUE13 VUT_MINSIZE
#define SPECENUM_VALUE13NAME "MinSize"
/* AI level of the player */
#define SPECENUM_VALUE14 VUT_AI_LEVEL
#define SPECENUM_VALUE14NAME "AI"
/* More generic terrain type currently "Land" or "Ocean" */
#define SPECENUM_VALUE15 VUT_TERRAINCLASS
#define SPECENUM_VALUE15NAME "TerrainClass"
#define SPECENUM_VALUE16 VUT_BASE
#define SPECENUM_VALUE16NAME "Base"
#define SPECENUM_VALUE17 VUT_MINYEAR
#define SPECENUM_VALUE17NAME "MinYear"
/* Terrain alterations that are possible */
#define SPECENUM_VALUE18 VUT_TERRAINALTER
#define SPECENUM_VALUE18NAME "TerrainAlter"
/* Target tile is used by city. */
#define SPECENUM_VALUE19 VUT_CITYTILE
#define SPECENUM_VALUE19NAME "CityTile"
/* Keep this last. */
#define SPECENUM_VALUE20 VUT_ROAD
#define SPECENUM_VALUE20NAME "Road"
#define SPECENUM_VALUE21 VUT_RESOURCE
#define SPECENUM_VALUE21NAME "Resource"
#define SPECENUM_VALUE22 VUT_TERRFLAG
#define SPECENUM_VALUE22NAME "TerrainFlag"
#define SPECENUM_VALUE23 VUT_NATIONALITY
#define SPECENUM_VALUE23NAME "Nationality"
#define SPECENUM_VALUE24 VUT_TECHFLAG
#define SPECENUM_VALUE24NAME "TechFlag"
#define SPECENUM_COUNT VUT_COUNT
#include "specenum_gen.h"

/* Used in the network protocol. */
struct universal {
  universals_u value;
  enum universals_n kind;		/* formerly .type and .is_unit */
};

struct ai_choice;			/* incorporates universals_u */

/* Used in the network protocol. */
BV_DEFINE(bv_bases, MAX_BASE_TYPES);
BV_DEFINE(bv_roads, MAX_ROAD_TYPES);
BV_DEFINE(bv_startpos_nations, MAX_NUM_STARTPOS_NATIONS);

/* Used in the network protocol. */
#define SPECENUM_NAME gui_type
/* Used for options which do not belong to any gui. */
#define SPECENUM_VALUE0 GUI_STUB
#define SPECENUM_VALUE0NAME "stub"
#define SPECENUM_VALUE1 GUI_GTK2
#define SPECENUM_VALUE1NAME "gtk2"
#define SPECENUM_VALUE2 GUI_GTK3
#define SPECENUM_VALUE2NAME "gtk3"
#define SPECENUM_VALUE3 GUI_SDL
#define SPECENUM_VALUE3NAME "sdl"
#define SPECENUM_VALUE4 GUI_XAW
#define SPECENUM_VALUE4NAME "xaw"
#define SPECENUM_VALUE5 GUI_QT
#define SPECENUM_VALUE5NAME "qt"
#define SPECENUM_VALUE6 GUI_WEB
#define SPECENUM_VALUE6NAME "web"
#include "specenum_gen.h"

/* Used in the network protocol. */
#define SPECENUM_NAME airlifting_style
#define SPECENUM_BITWISE
/* Like classical Freeciv.  One unit per turn. */
#define SPECENUM_ZERO   AIRLIFTING_CLASSICAL
/* Allow airlifting from allied cities. */
#define SPECENUM_VALUE0 AIRLIFTING_ALLIED_SRC
/* Allow airlifting to allied cities. */
#define SPECENUM_VALUE1 AIRLIFTING_ALLIED_DEST
/* Unlimited units to airlift from the source (but always needs an Airport
 * or equivalent). */
#define SPECENUM_VALUE2 AIRLIFTING_UNLIMITED_SRC
/* Unlimited units to airlift to the destination (doesn't require any
 * Airport or equivalent). */
#define SPECENUM_VALUE3 AIRLIFTING_UNLIMITED_DEST
#include "specenum_gen.h"

#define SPECENUM_NAME reveal_map
#define SPECENUM_BITWISE
/* Reveal only the area around the first units at the beginning. */
#define SPECENUM_ZERO   REVEAL_MAP_NONE
/* Reveal the (fogged) map at the beginning of the game. */
#define SPECENUM_VALUE0 REVEAL_MAP_START
/* Reveal (and unfog) the map for dead players. */
#define SPECENUM_VALUE1 REVEAL_MAP_DEAD
#include "specenum_gen.h"

/* Used in the network protocol. */
#define SPECENUM_NAME gameloss_style
#define SPECENUM_BITWISE
/* Like classical Freeciv. No special effects. */
#define SPECENUM_ZERO   GAMELOSS_STYLE_CLASSICAL
/* Remaining cities are taken by barbarians. */
#define SPECENUM_VALUE0 GAMELOSS_STYLE_BARB
#define SPECENUM_VALUE0NAME "Barbarians"
/* Try civil war. */
#define SPECENUM_VALUE1 GAMELOSS_STYLE_CWAR
#define SPECENUM_VALUE1NAME "CivilWar"
/* Do some looting */
#define SPECENUM_VALUE2 GAMELOSS_STYLE_LOOT
#define SPECENUM_VALUE2NAME "Loot"
#include "specenum_gen.h"

/* Used in the network protocol. */
#define SPECENUM_NAME tech_upkeep_style
/* No upkeep */
#define SPECENUM_VALUE0 TECH_UPKEEP_NONE
#define SPECENUM_VALUE0NAME "None"
/* Normal tech upkeep */
#define SPECENUM_VALUE1 TECH_UPKEEP_BASIC
#define SPECENUM_VALUE1NAME "Basic"
/* Tech upkeep multiplied by number of cities */
#define SPECENUM_VALUE2 TECH_UPKEEP_PER_CITY
#define SPECENUM_VALUE2NAME "Cities"
#include "specenum_gen.h"

/* Used in the network protocol. */
enum phase_mode_types {
  PMT_CONCURRENT = 0,
  PMT_PLAYERS_ALTERNATE,
  PMT_TEAMS_ALTERNATE
};

/* Used in the network protocol. */
enum borders_mode {
  BORDERS_DISABLED = 0,
  BORDERS_ENABLED,
  BORDERS_SEE_INSIDE,
  BORDERS_EXPAND,
};

/* Used in the network protocol. */
enum diplomacy_mode {
  DIPLO_FOR_ALL,
  DIPLO_FOR_HUMANS,
  DIPLO_FOR_AIS,
  DIPLO_NO_AIS,
  DIPLO_NO_MIXED,
  DIPLO_FOR_TEAMS,
  DIPLO_DISABLED,
};

/* Used in the network protocol. */
enum tile_special_type {
  S_IRRIGATION,
  S_MINE,
  S_POLLUTION,
  S_HUT,
  S_FARMLAND,
  S_FALLOUT,

  /* internal values not saved */
  S_LAST,
  S_RESOURCE_VALID = S_LAST,

  S_OLD_FORTRESS,
  S_OLD_AIRBASE,
  S_OLD_ROAD,
  S_OLD_RAILROAD,
  S_OLD_RIVER
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

enum test_result {
  TR_SUCCESS,
  TR_OTHER_FAILURE,
  TR_ALREADY_SOLD
};

enum act_tgt_type { ATT_SPECIAL, ATT_BASE, ATT_ROAD };

union act_tgt_obj {
  enum tile_special_type spe;
  Base_type_id base;
  Road_type_id road;
};

struct act_tgt {
  enum act_tgt_type type;
  union act_tgt_obj obj;
};

/* Road type compatibility with old specials based roads.
 * Used in the network protocol. */
enum road_compat { ROCO_ROAD, ROCO_RAILROAD, ROCO_RIVER, ROCO_NONE };

/*
 * Maximum number of trade routes a city can have in any situation.
 * Changing this changes network protocol.
 */
#define MAX_TRADE_ROUTES        5

#endif /* FC__FC_TYPES_H */

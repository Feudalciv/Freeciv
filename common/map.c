/********************************************************************** 
 Freeciv - Copyright (C) 1996 - A Kjeldberg, L Gregersen, P Unold
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <map.h>
#include <shared.h>
#include <log.h>
#include <unit.h>
#include <city.h>

/* the very map */
struct civ_map map;

struct tile_type tile_types[T_LAST]= 
{
  {"Arctic", "Seals",   0,0,0, 2,0,0, 2, 10, 3, T_LAST,0,      T_LAST,0},
  {"Desert", "Oasis",   0,1,0, 3,1,0, 1, 10, 1, T_DESERT,5,    T_DESERT,5},
  {"Forest", "Game" ,   1,2,0, 3,2,0, 2, 15, 3, T_PLAINS,5,    T_LAST,0},
  {"Grassland", "Resources", 2,0,0, 2,1,0, 1, 10, 1, T_GRASSLAND,5, T_FOREST,10},
  {"Hills", "Coals",    1,0,0, 1,2,0, 2, 20, 3, T_HILLS,10,    T_HILLS,10},
  {"Jungle", "Gems",    1,0,0, 1,0,4, 2, 15, 3, T_GRASSLAND,15,T_FOREST,15},
  {"Mountains","Gold",  0,1,0, 0,1,5, 3, 30, 5, T_LAST,0,      T_MOUNTAINS,10},
  {"Ocean", "Fish",     1,0,2, 3,0,2, 1, 10, 0, T_LAST,0,      T_LAST,0},
  {"Plains", "Horses",  1,1,0, 3,1,0, 1, 10, 1, T_PLAINS,5,    T_FOREST,15},
  {"River", "Resources",2,0,1, 2,1,1, 1, 15, 1, T_RIVER,5,     T_LAST,0},
  {"Swamp", "Oil",      1,0,0, 1,4,0, 2, 15, 3, T_GRASSLAND,15,T_FOREST,15},
  {"Tundra", "Game",    1,0,0, 3,0,0, 1, 10, 1, T_LAST,0,      T_LAST,0 }
};

struct isledata islands[100];

struct tile void_tile;

/***************************************************************
...
***************************************************************/
char *map_get_tile_info_text(int x, int y)
{
  static char s[64];
  struct tile *ptile=map_get_tile(x, y);
  
  sprintf(s, "%s%s",
	  (ptile->special&S_POLLUTION ? "Polluted " : ""),
	  tile_types[ptile->terrain].terrain_name);
  if(ptile->special&S_SPECIAL) 
    sprintf(s+strlen(s), "(%s)", tile_types[ptile->terrain].special_name);

  return s;
}

/***************************************************************
...
***************************************************************/
int map_is_empty(void)
{
  return map.tiles==0;
}


/***************************************************************
 put some sensible values into the map structure
***************************************************************/
void map_init(void)
{
  map.xsize=MAP_DEFAULT_WIDTH;
  map.ysize=MAP_DEFAULT_HEIGHT;
  map.seed=MAP_DEFAULT_SEED;
  map.age=0;
  map.riches=MAP_DEFAULT_RICHES;
  map.is_earth=0;
  map.huts=MAP_DEFAULT_HUTS;
  map.landpercent=MAP_DEFAULT_LANDMASS;
  map.swampsize=MAP_DEFAULT_SWAMPS;
  map.deserts=MAP_DEFAULT_DESERTS;
  map.mountains=MAP_DEFAULT_MOUNTAINS;
  map.riverlength=MAP_DEFAULT_RIVERS;
  map.forestsize=MAP_DEFAULT_FORESTS;
  map.generator=MAP_DEFAULT_GENERATOR;
  map.tiles=0;
}

/***************************************************************
...
***************************************************************/
struct tile_type *get_tile_type(enum tile_terrain_type type)
{
  return &tile_types[type];
}


/***************************************************************
...
***************************************************************/
int real_map_distance(int x0, int y0, int x1, int y1)
{
  int tmp;
  x0=map_adjust_x(x0);
  x1=map_adjust_x(x1);
  if(x0>x1)
    tmp=x0, x0=x1, x1=tmp;
  if(y0>y1)
    tmp=y0, y0=y1, y1=tmp;
  return MAX(y1 - y0, MIN(x1-x0, map.xsize-x1+x0));
}
/***************************************************************
...
***************************************************************/
int sq_map_distance(int x0, int y0, int x1, int y1)
{
  int tmp;
  x0=map_adjust_x(x0);
  x1=map_adjust_x(x1);
  if(x0>x1)
    tmp=x0, x0=x1, x1=tmp;
  if(y0>y1)
    tmp=y0, y0=y1, y1=tmp;
  return (((y1 - y0) * (y1 - y0)) +
         (MIN(x1 - x0, map.xsize - x1 + x0) * MIN(x1 - x0, map.xsize - x1 + x0)));
}
/***************************************************************
...
***************************************************************/
int map_distance(int x0, int y0, int x1, int y1)
{
  int tmp;
  x0=map_adjust_x(x0);
  x1=map_adjust_x(x1);
  if(x0>x1)
    tmp=x0, x0=x1, x1=tmp;
  if(y0>y1)
    tmp=y0, y0=y1, y1=tmp;
  return MIN(x1-x0, map.xsize-x1+x0)+y1-y0;
}

/***************************************************************
...
***************************************************************/
int is_terrain_near_tile(int x, int y, enum tile_terrain_type t)
{
  if (map_get_terrain(x, y+1)==t)   return 1;
  if (map_get_terrain(x-1, y-1)==t) return 1;
  if (map_get_terrain(x-1, y)==t)   return 1;
  if (map_get_terrain(x-1, y+1)==t) return 1;
  if (map_get_terrain(x+1, y-1)==t) return 1;
  if (map_get_terrain(x+1, y)==t)   return 1;
  if (map_get_terrain(x+1, y+1)==t) return 1;
  if (map_get_terrain(x, y-1)==t)   return 1;
  return 0;
}

/***************************************************************
...
***************************************************************/
int is_at_coast(int x, int y)
{
  if (map_get_terrain(x-1,y)==T_OCEAN)   return 1;
  if (map_get_terrain(x,y-1)==T_OCEAN)   return 1;
  if (map_get_terrain(x,y+1)==T_OCEAN)   return 1;
  if (map_get_terrain(x+1,y)==T_OCEAN)   return 1;
  return 0;
  
}

int is_coastline(int x,int y) 
{
  if (map_get_terrain(x-1,y)!=T_OCEAN)   return 1;
  if (map_get_terrain(x-1,y-1)!=T_OCEAN) return 1;
  if (map_get_terrain(x-1,y+1)!=T_OCEAN) return 1;
  if (map_get_terrain(x,y-1)!=T_OCEAN)   return 1;
  if (map_get_terrain(x,y+1)!=T_OCEAN)   return 1;
  if (map_get_terrain(x+1,y-1)!=T_OCEAN) return 1;
  if (map_get_terrain(x+1,y)!=T_OCEAN)   return 1;
  if (map_get_terrain(x+1,y+1)!=T_OCEAN) return 1;
  return 0;
}

int terrain_is_clean(int x, int y)
{
  int x1,y1;
  for (x1=x-3;x1<x+3;x1++)
    for (y1=y-3;y1<y+3;y1++) 
      if (map_get_terrain(x1,y1)!=T_GRASSLAND && map_get_terrain(x1,y1)!=T_PLAINS) return 0;
  return 1;
}

int same_island(int x1, int y1, int x2, int y2)
{
  return (map_get_continent(x1,y1) == map_get_continent(x2,y2));
}

int is_starter_close(int x, int y, int nr, int dist) 
{
  int i;

  if (map_get_terrain(x, y)!=T_PLAINS && 
      map_get_terrain(x, y)!=T_GRASSLAND &&
      map_get_terrain(x, y)!=T_RIVER
      )
    return 1;
  /* don't start on a hut */
  if (map_get_tile(x, y)->special&S_HUT)
    return 1;
  if (map_get_continent(x, y)<=2) return 1; /*don't want them starting
						    on the poles */
  for (i=0;i<nr;i++) 
    if ( same_island(x,y, map.start_positions[i].x, map.start_positions[i].y) 
	 && real_map_distance(x, y, map.start_positions[i].x, map.start_positions[i].y) < dist) 
      return 1;
  return 0;
}

int is_good_tile(int x, int y)
{
  int c;
  c=map_get_terrain(x,y);
  switch (c) {

    /* range 0 .. 5 , 2 standard */

  case T_FOREST:
    if (map_get_tile(x, y)->special) return 5;
    return 3;
  case T_RIVER:
    if (map_get_tile(x, y)->special) return 4;
    return 3;
  case T_GRASSLAND:
  case T_PLAINS:
  case T_HILLS:
    if (map_get_tile(x, y)->special) return 4;
    return 2;
  case T_DESERT:
  case T_OCEAN:/* must be called with usable seas */    
    if (map_get_tile(x, y)->special) return 3;
    return 1;
  case T_SWAMP:
  case T_JUNGLE:
  case T_MOUNTAINS:
    if (map_get_tile(x, y)->special) return 3;
    return 0;
  /* case T_ARCTIC: */
  /* case T_TUNDRA: */
  default:
    if (map_get_tile(x,y)->special) return 1;
    break;
  }
  return 0;
}


int is_water_adjacent(int x, int y)
{
  int x1,y1;
  for (y1=y-1;y1<y+2;y1++) 
    for (x1=x-1;x1<x+2;x1++) {
      if (map_get_terrain(x1,y1)==T_OCEAN || map_get_terrain(x1, y1)==T_RIVER)
	return 1;
    } 
  return 0;
}

int is_hut_close(int x, int y)
{
  int x1,y1;
  for (y1=y-3;y1<y+4;y1++) 
    for (x1=x-3;x1<x+4;x1++) {
      if (map_get_tile(x1,y1)->special&S_HUT)
	return 1;
    } 
  return 0;
}


int is_special_close(int x, int y)
{
  int x1,y1;
  for (x1=x-1;x1<x+2;x1++)
    for (y1=y-1;y1<=y+2;y1++) 
      if(map_get_tile(x1,y1)->special)
	return 1;
  return 0;
}

int is_sea_usable(int x, int y)
{
  int x1,y1;
  for (x1=x-2;x1<x+3;x1++)
    for (y1=y-2;y1<=y+3;y1++) 
      if( !( (x==x1-2||x==x1+2) && (y==y1-2||y==y1+2) ) )
	if(map_get_terrain(x1,y1)!=T_OCEAN)
	  return 1;
  return 0;
}


int is_water_adjacent_to_tile(int x, int y)
{
  struct tile *ptile, *ptile_n, *ptile_e, *ptile_s, *ptile_w;

  ptile=map_get_tile(x, y);
  ptile_n=map_get_tile(x, y-1);
  ptile_e=map_get_tile(x+1, y);
  ptile_s=map_get_tile(x, y+1);
  ptile_w=map_get_tile(x-1, y);

  return (ptile->terrain==T_OCEAN   || ptile->terrain==T_RIVER   || ptile->special&S_IRRIGATION ||
	  ptile_n->terrain==T_OCEAN || ptile_n->terrain==T_RIVER || ptile_n->special&S_IRRIGATION ||
	  ptile_e->terrain==T_OCEAN || ptile_e->terrain==T_RIVER || ptile_e->special&S_IRRIGATION ||
	  ptile_s->terrain==T_OCEAN || ptile_s->terrain==T_RIVER || ptile_s->special&S_IRRIGATION ||
	  ptile_w->terrain==T_OCEAN || ptile_w->terrain==T_RIVER || ptile_w->special&S_IRRIGATION);
}



/***************************************************************
..
***************************************************************/
int map_build_road_time(int x, int y)
{
  return tile_types[map_get_terrain(x, y)].road_time;
}

/***************************************************************
...
***************************************************************/
int map_build_irrigation_time(int x, int y)
{
  return tile_types[map_get_terrain(x, y)].irrigation_time;
}

/***************************************************************
...
***************************************************************/
int map_build_mine_time(int x, int y)
{
  return tile_types[map_get_terrain(x, y)].mining_time;
}


/***************************************************************
...
***************************************************************/
void map_irrigate_tile(int x, int y)
{
  enum tile_terrain_type now, result;
  
  now=map_get_terrain(x, y);
  result=tile_types[now].irrigation_result;
  
  if(now==result) {
    map_set_special(x, y, S_IRRIGATION);
  }
  else if(result!=T_LAST) {
    map_set_terrain(x, y, result);
    reset_move_costs(x, y);
  }
  map_clear_special(x, y, S_MINE);
}



/***************************************************************
...
***************************************************************/
void map_mine_tile(int x, int y)
{
  enum tile_terrain_type now, result;
  
  now=map_get_terrain(x, y);
  result=tile_types[now].mining_result;
  
  if(now==result) 
    map_set_special(x, y, S_MINE);
  else if(result!=T_LAST) {
    map_set_terrain(x, y, result);
    reset_move_costs(x, y);
  }
  map_clear_special(x,y, S_IRRIGATION);
}

void reset_move_costs(int x, int y)
{
  int c, i, j, k, xx[3], yy[3];
  int ii[8] = { 0, 1, 2, 0, 2, 0, 1, 2 };
  int jj[8] = { 0, 0, 0, 1, 1, 2, 2, 2 };
  int maxcost = 72; /* should be big enough without being TOO big */
  struct tile *tile0, *tile1;
  
  tile0 = map_get_tile(x, y);
  if((xx[2]=x+1)==map.xsize) xx[2]=0;
  if((xx[0]=x-1)==-1) xx[0]=map.xsize-1;
  xx[1] = x;
  yy[0] = y - 1;
  yy[1] = y; /* if these are out of range, map_get_tile will complain */
  yy[2] = y + 1;

/*  printf("Resetting move costs for (%d, %d) [%x%x%x%x%x%x%x%x]\n", x, y,
tile0->move_cost[0],  tile0->move_cost[1], tile0->move_cost[2], tile0->move_cost[3],
tile0->move_cost[4],  tile0->move_cost[5], tile0->move_cost[6], tile0->move_cost[7]); */
  for (k = 0; k < 8; k++) {
    i = ii[k]; j = jj[k]; /* saves CPU cycles? */
    tile1 = map_get_tile(xx[i], yy[j]);
    if (tile1->terrain == T_UNKNOWN) c = maxcost;
    else if (tile0->terrain == T_OCEAN) {
      if (tile1->city || tile1->terrain == T_OCEAN) c = -3;
      else c = maxcost;
    } else if (tile1->terrain == T_OCEAN) {
      if (tile0->city) c = -3;
      else c = maxcost;
    }
    else if ((tile0->special & tile1->special) & S_RAILROAD) c = 0;
    else if ((tile0->special & tile1->special) & S_ROAD) c = 1;
    else if (tile0->terrain == T_RIVER && tile1->terrain == T_RIVER) c = 1;
    else c = get_tile_type(tile1->terrain)->movement_cost*3;
    tile0->move_cost[k] = c;
  } /* next k */
/*  printf("Reset move costs for (%d, %d) [%x%x%x%x%x%x%x%x]\n", x, y,
tile0->move_cost[0],  tile0->move_cost[1], tile0->move_cost[2], tile0->move_cost[3],
tile0->move_cost[4],  tile0->move_cost[5], tile0->move_cost[6], tile0->move_cost[7]); */
/* reverse!  This is not optimized, and hopefully not obfuscated either -- Syela */
  tile1 = tile0;
  for (k = 0; k < 8; k++) {
    i = ii[k]; j = jj[k]; /* saves CPU cycles? */
    tile0 = map_get_tile(xx[i], yy[j]);
    if (tile1->terrain == T_UNKNOWN) c = maxcost;
    else if (tile0->terrain == T_OCEAN) {
      if (tile1->city || tile1->terrain == T_OCEAN) c = -3;
      else c = maxcost;
    } else if (tile1->terrain == T_OCEAN) {
      if (tile0->city) c = -3;
      else c = maxcost;
    }
    else if ((tile0->special & tile1->special) & S_RAILROAD) c = 0;
    else if ((tile0->special & tile1->special) & S_ROAD) c = 1;
    else if (tile0->terrain == T_RIVER && tile1->terrain == T_RIVER) c = 1;
    else c = get_tile_type(tile1->terrain)->movement_cost*3;
    tile0->move_cost[7 - k] = c; /* this might muck with void_tile, but who cares? */
  } /* next k */
}

void initialize_move_costs(void)
{
  int x, y, c, i, j, k, xx[3], yy[3];
  int ii[8] = { 0, 1, 2, 0, 2, 0, 1, 2 };
  int jj[8] = { 0, 0, 0, 1, 1, 2, 2, 2 };
  int maxcost = 72; /* should be big enough without being TOO big */
  struct tile *tile0, *tile1;

  tile_init(&void_tile); /* maybe this is a weird place but it has to be done. -- Syela */
  for (i = 0; i < 8; i++) void_tile.move_cost[i] = maxcost;

  for (x = 0; x < map.xsize; x++) {
    for (y = 0; y < map.ysize; y++) {
      tile0 = map_get_tile(x, y);
      if((xx[2]=x+1)==map.xsize) xx[2]=0;
      if((xx[0]=x-1)==-1) xx[0]=map.xsize-1;
      xx[1] = x;
      yy[0] = y - 1;
      yy[1] = y; /* if these are out of range, map_get_tile will complain */
      yy[2] = y + 1;
      for (k = 0; k < 8; k++) {
        i = ii[k]; j = jj[k]; /* saves CPU cycles? */
        tile1 = map_get_tile(xx[i], yy[j]);
        if (tile1->terrain == T_UNKNOWN) c = maxcost;
        else if (tile0->terrain == T_OCEAN) {
          if (tile1->city || tile1->terrain == T_OCEAN) c = -3;
          else c = maxcost;
        } else if (tile1->terrain == T_OCEAN) {
          if (tile0->city) c = -3;
          else c = maxcost;
        }
        else if ((tile0->special & tile1->special) & S_RAILROAD) c = 0;
        else if ((tile0->special & tile1->special) & S_ROAD) c = 1;
        else if (tile0->terrain == T_RIVER && tile1->terrain == T_RIVER) c = 1;
        else c = get_tile_type(tile1->terrain)->movement_cost*3;
        tile0->move_cost[k] = c;
      } /* next k */
    } /* next y */
  } /* next x */
}

/***************************************************************
  The cost to move punit from x1,y1 to x2,y2
  The tiles are assumed to be adjacent
***************************************************************/
int tile_move_cost(struct unit *punit, int x1, int y1, int x2, int y2)
{
  struct tile *t1=map_get_tile(x1,y1);
  struct tile *t2=map_get_tile(x2,y2);

  if (!is_ground_unit(punit))
    return 3;
  if( (t1->special&S_RAILROAD) && (t2->special&S_RAILROAD) )
    return 0;
  if(unit_flag(punit->type, F_IGTER)) 
    return 1;
  if( (t1->special&S_ROAD) && (t2->special&S_ROAD) )
    return 1;
  if( (t1->terrain==T_RIVER) && (t2->terrain==T_RIVER) )
    return 1;
  return(get_tile_type(t2->terrain)->movement_cost*3);
}

/***************************************************************
  The cost to move punit from where it is to tile x1,y1.
  It is assumed the move is a valid one, e.g. the tiles are adjacent
***************************************************************/
int map_move_cost(struct unit *punit, int x1, int y1)
{
  return tile_move_cost(punit, punit->x, punit->y, x1, y1);
}

/***************************************************************
...
***************************************************************/

int is_tiles_adjacent(int x0, int y0, int x1, int y1)
{
  x0 = map_adjust_x(x0);
  x1 = map_adjust_x(x1);
  if (same_pos(x0, y0, x1, y1))
    return 1;
  return (((x0<=x1+1 && x0>=x1-1) || (x0==0 && x1==map.xsize-1) ||
	   (x0==map.xsize-1 && x1==0)) && (y0<=y1+1 && y0>=y1-1));
}



/***************************************************************
...
***************************************************************/
void tile_init(struct tile *ptile)
{
  ptile->terrain=T_UNKNOWN;
  ptile->special=S_NONE;
  ptile->known=0;
  ptile->city=NULL;
  unit_list_init(&ptile->units);
  ptile->worked = NULL; /* pointer to city working tile */
  ptile->assigned = 0; /* bitvector */
}


/***************************************************************
...
***************************************************************/
struct tile *map_get_tile(int x, int y)
{
  if(y<0 || y>=map.ysize)
    return &void_tile; /* accurate fix by Syela */
  else
    return map.tiles+map_adjust_x(x)+y*map.xsize;
}

/***************************************************************
...
***************************************************************/
char map_get_continent(int x, int y)
{
  if (y<0 || y>=map.ysize)
    return -1;
  else
    return (map.tiles + map_adjust_x(x) + y * map.xsize)->continent;
}

/***************************************************************
...
***************************************************************/

void map_set_continent(int x, int y, int val)
{
    (map.tiles + map_adjust_x(x) + y * map.xsize)->continent=val;
}


/***************************************************************
...
***************************************************************/

enum tile_terrain_type map_get_terrain(int x, int y)
{
  if(y<0 || y>=map.ysize)
    return T_UNKNOWN;
  else
    return (map.tiles+map_adjust_x(x)+y*map.xsize)->terrain;
}

/***************************************************************
...
***************************************************************/
enum tile_special_type map_get_special(int x, int y)
{
  if(y<0 || y>=map.ysize)
    return S_NONE;
  else
    return (map.tiles+map_adjust_x(x)+y*map.xsize)->special;
}

/***************************************************************
...
***************************************************************/
void map_set_terrain(int x, int y, enum tile_terrain_type ter)
{
  (map.tiles+map_adjust_x(x)+
   map_adjust_y(y)*map.xsize)->terrain=ter;
}

/***************************************************************
...
***************************************************************/
void map_set_special(int x, int y, enum tile_special_type spe)
{
  x = map_adjust_x(x); y = map_adjust_y(y);
  (map.tiles+x+y*map.xsize)->special|=spe;
  if (spe == S_ROAD || spe == S_RAILROAD) reset_move_costs(x, y);
}

/***************************************************************
...
***************************************************************/
void map_clear_special(int x, int y, enum tile_special_type spe)
{
  x = map_adjust_x(x); y = map_adjust_y(y);
  (map.tiles+x+y*map.xsize)->special&=~spe;
  if (spe == S_ROAD || spe == S_RAILROAD) reset_move_costs(x, y);
}


/***************************************************************
...
***************************************************************/
struct city *map_get_city(int x, int y)
{
  return (map.tiles+map_adjust_x(x)+map_adjust_y(y)*map.xsize)->city;
}


/***************************************************************
...
***************************************************************/
void map_set_city(int x, int y, struct city *pcity)
{
  (map.tiles+map_adjust_x(x)+map_adjust_y(y)*map.xsize)->city=pcity;
}


/***************************************************************
...
***************************************************************/
int map_get_known(int x, int y, struct player *pplayer)
{
  return ((map.tiles+map_adjust_x(x)+
	   map_adjust_y(y)*map.xsize)->known)&(1u<<pplayer->player_no);
}

/***************************************************************
...
***************************************************************/
enum known_type tile_is_known(int x, int y)
{
  return ((map.tiles+map_adjust_x(x)+
	   map_adjust_y(y)*map.xsize)->known);
}


/***************************************************************
...
***************************************************************/
void map_set_known(int x, int y, struct player *pplayer)
{
  (map.tiles+map_adjust_x(x)+
   map_adjust_y(y)*map.xsize)->known|=(1u<<pplayer->player_no);
}

/***************************************************************
...
***************************************************************/
void map_clear_known(int x, int y, struct player *pplayer)
{
  (map.tiles+map_adjust_x(x)+
   map_adjust_y(y)*map.xsize)->known&=~(1u<<pplayer->player_no);
}

/***************************************************************
...
***************************************************************/
void map_know_all(struct player *pplayer)
{
  int x, y;
  for (x = 0; x < map.xsize; ++x)
  {
    for (y = 0; y < map.ysize; ++y)
    {
      map_set_known(x, y, pplayer);
    }
  }
}

/***************************************************************
This function might be necessary ALOT of places...
***************************************************************/

int same_pos(int x1, int y1, int x2, int y2)
{
  return (map_adjust_x(x1) == map_adjust_x(x2) && map_adjust_y(y1) == map_adjust_y(y2)); 
}

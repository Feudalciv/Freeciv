#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <intuition/intuitionbase.h>
#include <libraries/mui.h>
#include <guigfx/guigfx.h>

#include <clib/alib_protos.h>

#define USE_SYSBASE
#include <proto/exec.h>
#include <proto/muimaster.h>
#include <proto/utility.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/guigfx.h>

#include "civclient.h"
#include "map.h"
#include "game.h"

#include "overviewclass.h"

#include "muistuff.h"

VOID myWritePixelArray8(struct RastPort *rp, unsigned long xstart,
	     unsigned long ystart, unsigned long xstop, unsigned long ystop,
			UBYTE * array)
{
  if (IntuitionBase->LibNode.lib_Version < 40)
  {
    struct RastPort temprp;
    InitRastPort(&temprp);
    if ((temprp.BitMap = AllocBitMap(xstop - xstart + 1, 1, GetBitMapAttr(rp->BitMap, BMA_DEPTH), NULL, rp->BitMap)))
    {
      WritePixelArray8(rp, xstart, ystart, xstop, ystop, array, &temprp);
      FreeBitMap(temprp.BitMap);
    }
  }
  else
  {
    WriteChunkyPixels(rp, xstart, ystart, xstop, ystop, array, ((xstop - xstart + 1 + 15) / 16) * 16);
  }
}


struct MUI_CustomClass *CL_Overview;

Object *MakeOverview(LONG width, LONG height)
{
  return OverviewObject,
    MUIA_Overview_Width, width,
    MUIA_Overview_Height, height,
    End;
}

struct Overview_Data
{
  LONG ov_MapHeight;
  LONG ov_MapWidth;
  LONG ov_ScaleX;
  LONG ov_ScaleY;

  LONG ov_BufferWidth;
  LONG ov_BufferHeight;
  UBYTE *ov_Buffer;

  BOOL clicked;

  LONG pen_white;
  LONG pen_unknown;
  LONG pen_ground;
  LONG pen_ocean;
  LONG pen_unit;
  LONG pen_city;
  LONG pen_meunit;
  LONG pen_mecity;

  APTR radar_picture;

  LONG rect_left;
  LONG rect_top;
  LONG rect_width;
  LONG rect_height;

  LONG update;			/* 1 Refresh Single */
  LONG x, y, color;		/* 1 */
};

STATIC VOID Overview_HandleMouse(Object * o, struct Overview_Data *data, LONG x, LONG y)
{
  x = x % _mwidth(o);

  x = (x - _mleft(o)) / data->ov_ScaleX - data->rect_width / 2;
  y = (y - _mtop(o)) / data->ov_ScaleY - data->rect_height / 2;

  if (x < 0)
    x = map_adjust_x(x);
  if (y < 0)
    y = 0;
  if (y + data->rect_height > data->ov_MapHeight)
    y = data->ov_MapHeight - data->rect_height;

  if (data->rect_left != x || data->rect_top != y)
  {
    SetAttrs(o,
	     MUIA_Overview_NewPos, TRUE,
	     MUIA_Overview_RectLeft, x,
	     MUIA_Overview_RectTop, y,
	     TAG_DONE);
  }
}
STATIC LONG Overview_GetMapPen(struct Overview_Data *data, LONG x, LONG y)
{
  struct tile *ptile = map_get_tile(x, y);
  struct unit *punit;
  struct city *pcity;

  if (!ptile->known)
  {
    return data->pen_unknown;
  }
  else
  {
    if ((punit = find_visible_unit(ptile)))
    {
      if (punit->owner == game.player_idx)
      {
	return data->pen_meunit;
      }
      else
      {
	return data->pen_unit;
      }
    }
    else
    {
      if ((pcity = map_get_city(x, y)))
      {
	if (pcity->owner == game.player_idx)
	{
	  return data->pen_mecity;
	}
	else
	{
	  return data->pen_city;
	}
      }
      else
      {
	if (ptile->terrain == T_OCEAN)
	{
	  return data->pen_ocean;
	}
	else
	{
	  return data->pen_ground;
	}
      }
    }
  }
}

STATIC VOID Overview_FillBuffer(struct Overview_Data * data)
{
  LONG scalex = data->ov_ScaleX;
  LONG scaley = data->ov_ScaleY;
  int s, x, y;

  for (y = 0; y < map.ysize; y++)
  {
    UBYTE *bufstart = &data->ov_Buffer[(y * scaley) * data->ov_BufferWidth];
    UBYTE *buf = bufstart;

    for (x = 0; x < map.xsize; x++)
    {
      UBYTE penno = Overview_GetMapPen(data, x, y);

      for (s = 0; s < scalex; s++)
	*buf++ = penno;
    }

    for (s = 1; s < scaley; s++)
    {
      UBYTE *nextbufstart = bufstart + data->ov_BufferWidth;
      CopyMem(bufstart, nextbufstart, data->ov_BufferWidth);
      bufstart = nextbufstart;
    }
  }
}

STATIC ULONG Overview_New(struct IClass *cl, Object * o, struct opSet *msg)
{
  if ((o = (Object *) DoSuperMethodA(cl, o, (Msg) msg)))
  {
    struct Overview_Data *data = (struct Overview_Data *) INST_DATA(cl, o);
    struct TagItem *tl = msg->ops_AttrList;
    struct TagItem *ti;

    while ((ti = NextTagItem(&tl)))
    {
      switch (ti->ti_Tag)
      {
      case MUIA_Overview_Width:
	data->ov_MapWidth = ti->ti_Data;
	break;

      case MUIA_Overview_Height:
	data->ov_MapHeight = ti->ti_Data;
	break;
      }
    }

    if (!data->ov_MapHeight || !data->ov_MapWidth)
    {
      CoerceMethod(cl, o, OM_DISPOSE);
      return NULL;
    }
  }
  return (ULONG) o;
}

STATIC ULONG Overview_Dispose(struct IClass * cl, Object * o, Msg msg)
{
  return DoSuperMethodA(cl, o, msg);
}

STATIC ULONG Overview_Set(struct IClass * cl, Object * o, struct opSet * msg)
{
  struct Overview_Data *data = (struct Overview_Data *) INST_DATA(cl, o);
  struct TagItem *tl = msg->ops_AttrList;
  struct TagItem *ti;

  BOOL redraw = FALSE;

  while ((ti = NextTagItem(&tl)))
  {
    switch (ti->ti_Tag)
    {
    case MUIA_Overview_RectLeft:
      data->rect_left = ti->ti_Data;
      redraw = TRUE;
      break;

    case MUIA_Overview_RectTop:
      data->rect_top = ti->ti_Data;
      redraw = TRUE;
      break;

    case MUIA_Overview_RectWidth:
      data->rect_width = ti->ti_Data;
      redraw = TRUE;
      break;

    case MUIA_Overview_RectHeight:
      data->rect_height = ti->ti_Data;
      redraw = TRUE;
      break;

    case MUIA_Overview_RadarPicture:
      data->radar_picture = (APTR) ti->ti_Data;
      redraw = TRUE;
      break;
    }
  }

  if (redraw)
  {
    data->update = 2;
    MUI_Redraw(o, MADF_DRAWUPDATE);
  }
  return DoSuperMethodA(cl, o, (Msg) msg);
}

STATIC ULONG Overview_Get(struct IClass * cl, Object * o, struct opGet * msg)
{
  struct Overview_Data *data = (struct Overview_Data *) INST_DATA(cl, o);
  switch (msg->opg_AttrID)
  {
  case MUIA_Overview_RectLeft:
    *msg->opg_Storage = data->rect_left;
    break;

  case MUIA_Overview_RectTop:
    *msg->opg_Storage = data->rect_top;
    break;

  case MUIA_Overview_Width:
    *msg->opg_Storage = data->ov_MapWidth;
    break;

  case MUIA_Overview_Height:
    *msg->opg_Storage = data->ov_MapHeight;
    break;

  default:
    return DoSuperMethodA(cl, o, (Msg) msg);
  }
  return 1;
}

STATIC ULONG Overview_Setup(struct IClass * cl, Object * o, Msg msg)
{
  struct Overview_Data *data = (struct Overview_Data *) INST_DATA(cl, o);

  if (!DoSuperMethodA(cl, o, msg))
    return FALSE;

  data->ov_ScaleX = 2;
  data->ov_ScaleY = 2;		/* * _dri(o)->dri_Resolution.X/_dri(o)->dri_Resolution.Y; */
/*      if(data->ov_ScaleY<1) data->ov_ScaleY = 1; */

  data->ov_BufferWidth = ((data->ov_MapWidth * data->ov_ScaleX + 15) / 16) * 16;
  data->ov_BufferHeight = data->ov_MapHeight * data->ov_ScaleY;

  if ((data->ov_Buffer = (UBYTE *) AllocVec(data->ov_BufferWidth * data->ov_BufferHeight, 0)))
  {
    struct ColorMap *cm = _screen(o)->ViewPort.ColorMap;
    data->pen_white = ObtainBestPenA(cm, -1, -1, -1, NULL);
    data->pen_unknown = ObtainBestPenA(cm, MAKECOLOR32(10), MAKECOLOR32(10), MAKECOLOR32(10), NULL);
    data->pen_ground = ObtainBestPenA(cm, 0, MAKECOLOR32(200), 0, NULL);
    data->pen_ocean = ObtainBestPenA(cm, 0, 0, MAKECOLOR32(200), NULL);
    data->pen_mecity = ObtainBestPenA(cm, -1, -1, -1, NULL);
    data->pen_meunit = ObtainBestPenA(cm, -1, -1, NULL, NULL);
    data->pen_city = ObtainBestPenA(cm, 0, -1, MAKECOLOR32(200), NULL);
    data->pen_unit = ObtainBestPenA(cm, -1, NULL, NULL, NULL);

    MUI_RequestIDCMP(o, IDCMP_MOUSEBUTTONS);

    if (get_client_state() == CLIENT_GAME_RUNNING_STATE)
    {
      Overview_FillBuffer(data);
    }

    return TRUE;
  }
  CoerceMethod(cl, o, MUIM_Cleanup);
  return FALSE;
}

STATIC ULONG Overview_Cleanup(struct IClass * cl, Object * o, Msg msg)
{
  struct Overview_Data *data = (struct Overview_Data *) INST_DATA(cl, o);
  struct ColorMap *cm = _screen(o)->ViewPort.ColorMap;

  MUI_RejectIDCMP(o, IDCMP_MOUSEBUTTONS);

  ReleasePen(cm, data->pen_white);
  ReleasePen(cm, data->pen_unknown);
  ReleasePen(cm, data->pen_ground);
  ReleasePen(cm, data->pen_ocean);
  ReleasePen(cm, data->pen_mecity);
  ReleasePen(cm, data->pen_meunit);
  ReleasePen(cm, data->pen_city);
  ReleasePen(cm, data->pen_unit);

  if (data->ov_Buffer)
  {
    FreeVec(data->ov_Buffer);
    data->ov_Buffer = NULL;
  }

  DoSuperMethodA(cl, o, msg);
  return 0;
}

STATIC ULONG Overview_AskMinMax(struct IClass * cl, Object * o, struct MUIP_AskMinMax * msg)
{
  struct Overview_Data *data = (struct Overview_Data *) INST_DATA(cl, o);
  LONG scalex = data->ov_ScaleX;
  LONG scaley = data->ov_ScaleY;
  DoSuperMethodA(cl, o, (Msg) msg);

  msg->MinMaxInfo->MinWidth += data->ov_MapWidth * scalex;
  msg->MinMaxInfo->DefWidth += data->ov_MapWidth * scalex;
  msg->MinMaxInfo->MaxWidth += data->ov_MapWidth * scalex;

  msg->MinMaxInfo->MinHeight += data->ov_MapHeight * scaley;
  msg->MinMaxInfo->DefHeight += data->ov_MapHeight * scaley;
  msg->MinMaxInfo->MaxHeight += data->ov_MapHeight * scaley;
  return 0;
}

STATIC ULONG Overview_Draw(struct IClass * cl, Object * o, struct MUIP_Draw * msg)
{
  struct Overview_Data *data = (struct Overview_Data *) INST_DATA(cl, o);
  struct RastPort *rp = _rp(o);
  LONG scalex = data->ov_ScaleX;
  LONG scaley = data->ov_ScaleY;

  DoSuperMethodA(cl, o, (Msg) msg);

  if (get_client_state() == CLIENT_GAME_RUNNING_STATE)
  {
    BOOL draw_overview = TRUE;
    if (msg->flags & MADF_DRAWUPDATE)
    {
      if (data->update == 1)
      {
	LONG x1 = _mleft(o) + data->x;
	LONG y1 = _mtop(o) + data->y;
	SetAPen(_rp(o), data->color);
	RectFill(_rp(o), x1, y1, x1 + scalex - 1, y1 + scaley - 1);

	draw_overview = FALSE;
      }
    }

    if (draw_overview)
    {
      LONG x1 = _mleft(o) + map_adjust_x(data->rect_left) * scalex;
      LONG x2 = _mleft(o) + map_adjust_x(data->rect_left + data->rect_width) * scalex;	//x1 + data->rect_width * scale - 1;

      LONG y1 = _mtop(o) + data->rect_top * scaley;
      LONG y2 = y1 + data->rect_height * scaley - 1;

      BOOL twoparts;

      myWritePixelArray8(rp, _mleft(o), _mtop(o),
			 _left(o) + data->ov_MapWidth * scalex - 1,
			 _mtop(o) + data->ov_MapHeight * scaley - 1,
			 data->ov_Buffer);

      if (x2 < x1)
	twoparts = TRUE;
      else
	twoparts = FALSE;

      Move(_rp(o), x1, y2);
      SetAPen(_rp(o), data->pen_white);
      Draw(_rp(o), x1, y1);

      if (!twoparts)
      {
	Draw(_rp(o), x2, y1);
	Draw(_rp(o), x2, y2);
	Draw(_rp(o), x1, y2);
      }
      else
      {
	Draw(_rp(o), _mright(o), y1);
	Move(_rp(o), x1, y2);
	Draw(_rp(o), _mright(o), y2);

	Move(_rp(o), _mleft(o), y1);
	Draw(_rp(o), x2, y1);
	Draw(_rp(o), x2, y2);
	Draw(_rp(o), _mleft(o), y2);
      }
    }
  }
  else
  {
    if (!(msg->flags & MADF_DRAWUPDATE))
    {
      IMPORT APTR pen_shared_map;	// should happen with Tags...

      if (pen_shared_map && data->radar_picture)
      {
	APTR dh = ObtainDrawHandleA(pen_shared_map, _rp(o), _screen(o)->ViewPort.ColorMap, NULL);
	if (dh)
	{
	  DrawPicture(dh, data->radar_picture, _mleft(o), _mtop(o),
		      GGFX_DestWidth, _mwidth(o),
		      GGFX_DestHeight, _mheight(o),
		      TAG_DONE);

	  ReleaseDrawHandle(dh);
	}
      }
    }
  }

  return 0;
}

STATIC ULONG Overview_HandleInput(struct IClass * cl, Object * o, struct MUIP_HandleInput * msg)
{
  struct Overview_Data *data = (struct Overview_Data *) INST_DATA(cl, o);
  if (msg->imsg)
  {
    switch (msg->imsg->Class)
    {
    case IDCMP_MOUSEMOVE:
      Overview_HandleMouse(o, data, msg->imsg->MouseX, msg->imsg->MouseY);
      break;

    case IDCMP_MOUSEBUTTONS:
      if (msg->imsg->Code == SELECTDOWN)
      {
	if (_isinobject(msg->imsg->MouseX, msg->imsg->MouseY))
	{
	  Overview_HandleMouse(o, data, msg->imsg->MouseX, msg->imsg->MouseY);
	  data->clicked = TRUE;
	  MUI_RequestIDCMP(o, IDCMP_MOUSEMOVE);
	}
      }
      else
      {
	if (msg->imsg->Code == SELECTUP && data->clicked)
	{
	  Overview_HandleMouse(o, data, msg->imsg->MouseX, msg->imsg->MouseY);
	  MUI_RejectIDCMP(o, IDCMP_MOUSEMOVE);
	  data->clicked = FALSE;
	}
      }
      break;
    }
  }
  return 0;
}

STATIC ULONG Overview_Refresh(struct IClass * cl, Object * o)
{
  struct Overview_Data *data = (struct Overview_Data *) INST_DATA(cl, o);
  Overview_FillBuffer(data);
  data->update = 2;
  MUI_Redraw(o, MADF_DRAWUPDATE);
  return 0;
}

STATIC ULONG Overview_RefreshSingle(struct IClass * cl, Object * o, struct MUIP_Overview_RefreshSingle * msg)
{
  struct Overview_Data *data = (struct Overview_Data *) INST_DATA(cl, o);
  LONG x, y;
  LONG scalex = data->ov_ScaleX;
  LONG scaley = data->ov_ScaleY;
  LONG x1 = msg->x * scalex;
  LONG y1 = msg->y * scaley;
  LONG color = Overview_GetMapPen(data, msg->x, msg->y);

  for (x = 0; x < scalex; x++)
  {
    for (y = 0; y < scaley; y++)
    {
      data->ov_Buffer[(x1 + x) + (y1 + y) * data->ov_BufferWidth] = color;
    }
  }

  data->update = 1;
  data->x = x1;
  data->y = y1;
  data->color = color;

  MUI_Redraw(o, MADF_DRAWUPDATE);
  return NULL;
}

STATIC __asm __saveds ULONG Overview_Dispatcher(register __a0 struct IClass * cl, register __a2 Object * obj, register __a1 Msg msg)
{
  switch (msg->MethodID)
  {
  case OM_NEW:
    return Overview_New(cl, obj, (struct opSet *) msg);
  case OM_DISPOSE:
    return Overview_Dispose(cl, obj, msg);
  case OM_GET:
    return Overview_Get(cl, obj, (struct opGet *) msg);
  case OM_SET:
    return Overview_Set(cl, obj, (struct opSet *) msg);
  case MUIM_Setup:
    return Overview_Setup(cl, obj, msg);
  case MUIM_Cleanup:
    return Overview_Cleanup(cl, obj, msg);
  case MUIM_AskMinMax:
    return Overview_AskMinMax(cl, obj, (struct MUIP_AskMinMax *) msg);
  case MUIM_Draw:
    return Overview_Draw(cl, obj, (struct MUIP_Draw *) msg);
  case MUIM_HandleInput:
    return Overview_HandleInput(cl, obj, (struct MUIP_HandleInput *) msg);
  case MUIM_Overview_Refresh:
    return Overview_Refresh(cl, obj);
  case MUIM_Overview_RefreshSingle:
    return Overview_RefreshSingle(cl, obj, (struct MUIP_Overview_RefreshSingle *) msg);
  }
  return (DoSuperMethodA(cl, obj, msg));
}

BOOL create_overview_class(void)
{
  if ((CL_Overview = MUI_CreateCustomClass(NULL, MUIC_Area, NULL, sizeof(struct Overview_Data), Overview_Dispatcher)))
      return TRUE;
  return FALSE;
}

VOID delete_overview_class(void)
{
  if (CL_Overview)
    MUI_DeleteCustomClass(CL_Overview);
}

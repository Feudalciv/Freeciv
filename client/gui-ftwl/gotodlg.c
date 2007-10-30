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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "game.h"

#include "civclient.h"
#include "control.h"

#include "gotodlg.h"

/**************************************************************************
  Popup a dialog to have the focus unit goto to a city.
**************************************************************************/
void popup_goto_dialog(void)
{
  if (get_client_state() != CLIENT_GAME_RUNNING_STATE) {
    return;
  }
  if (0 == get_num_units_in_focus()) {
    return;
  }
  /* PORTME */
}

/* gb-gdk.c
 *
 * Copyright (C) 2015 Christian Hergert <christian@hergert.me>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gb-gdk.h"

gboolean
gb_gdk_event_key_is_escape (const GdkEventKey *event)
{
  g_return_val_if_fail (event != NULL, FALSE);

  return event->keyval == GDK_KEY_Escape;
}

gboolean
gb_gdk_event_key_is_keynav (const GdkEventKey *event)
{
  g_return_val_if_fail (event != NULL, FALSE);

  switch (event->keyval)
    {
    case GDK_KEY_Escape:
    case GDK_KEY_Up:
    case GDK_KEY_KP_Up:
    case GDK_KEY_Down:
    case GDK_KEY_KP_Down:
    case GDK_KEY_Left:
    case GDK_KEY_KP_Left:
    case GDK_KEY_Right:
    case GDK_KEY_KP_Right:
    case GDK_KEY_Home:
    case GDK_KEY_KP_Home:
    case GDK_KEY_End:
    case GDK_KEY_KP_End:
    case GDK_KEY_Page_Up:
    case GDK_KEY_KP_Page_Up:
    case GDK_KEY_Page_Down:
    case GDK_KEY_KP_Page_Down:
      return TRUE;
    default:
      break;
    }

  if ((event->state & GDK_MOD1_MASK) || (event->state & GDK_CONTROL_MASK))
    return TRUE;

  return FALSE;
}

gboolean
gb_gdk_event_key_is_space (const GdkEventKey *event)
{
  g_return_val_if_fail (event != NULL, FALSE);

  return event->keyval == GDK_KEY_space;
}

gboolean
gb_gdk_event_key_is_tab (const GdkEventKey *event)
{
  g_return_val_if_fail (event != NULL, FALSE);

  return (event->keyval == GDK_KEY_Tab) || (event->keyval == GDK_KEY_KP_Tab);
}

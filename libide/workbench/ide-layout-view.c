/* ide-layout-view.c
 *
 * Copyright (C) 2015 Christian Hergert <christian@hergert.me>
 *
 * This file is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define G_LOG_DOMAIN "ide-layout-view"

#include <glib/gi18n.h>

#include "application/ide-application.h"
#include "workbench/ide-layout-view.h"

G_DEFINE_TYPE (IdeLayoutView, ide_layout_view, GTK_TYPE_BOX)

enum {
  PROP_0,
  PROP_CAN_SPLIT,
  PROP_MODIFIED,
  PROP_SPECIAL_TITLE,
  PROP_TITLE,
  LAST_PROP
};

static GParamSpec *properties [LAST_PROP];

/**
 * ide_layout_view_get_can_preview:
 * @self: A #IdeLayoutView.
 *
 * Checks if @self can create a preview view (such as html, markdown, etc).
 *
 * Returns: %TRUE if @self can create a preview view.
 */
gboolean
ide_layout_view_get_can_preview (IdeLayoutView *self)
{
  g_return_val_if_fail (IDE_IS_LAYOUT_VIEW (self), FALSE);

  if (IDE_LAYOUT_VIEW_GET_CLASS (self)->get_can_preview)
    return IDE_LAYOUT_VIEW_GET_CLASS (self)->get_can_preview (self);

  return FALSE;
}

/**
 * ide_layout_view_get_can_split:
 * @self: A #IdeLayoutView.
 *
 * Checks if @self can create a split view. If so, %TRUE is returned. Otherwise, %FALSE.
 *
 * Returns: %TRUE if @self can create a split.
 */
gboolean
ide_layout_view_get_can_split (IdeLayoutView *self)
{
  g_return_val_if_fail (IDE_IS_LAYOUT_VIEW (self), FALSE);

  if (IDE_LAYOUT_VIEW_GET_CLASS (self)->get_can_split)
    return IDE_LAYOUT_VIEW_GET_CLASS (self)->get_can_split (self);

  return FALSE;
}

/**
 * ide_layout_view_create_split:
 * @self: A #IdeLayoutView.
 * @file: A #GFile already loaded by the #IdeBufferManager, or %NULL to use the
 * existing buffer.
 *
 * Creates a new view that can be displayed in a split, potentially with a different
 * buffer. If the view does not support splits, %NULL will be returned.
 *
 * Returns: (transfer full): A #IdeLayoutView.
 */
IdeLayoutView *
ide_layout_view_create_split (IdeLayoutView *self,
                              GFile         *file)
{
  g_return_val_if_fail (IDE_IS_LAYOUT_VIEW (self), NULL);

  if (IDE_LAYOUT_VIEW_GET_CLASS (self)->create_split)
    return IDE_LAYOUT_VIEW_GET_CLASS (self)->create_split (self, file);

  return NULL;
}

/**
 * ide_layout_view_set_split_view:
 * @self: A #IdeLayoutView.
 * @split_view: if the split should be enabled.
 *
 * Set a split view using GtkPaned style split with %GTK_ORIENTATION_VERTICAL.
 */
void
ide_layout_view_set_split_view (IdeLayoutView   *self,
                                gboolean         split_view)
{
  g_return_if_fail (IDE_IS_LAYOUT_VIEW (self));

  if (IDE_LAYOUT_VIEW_GET_CLASS (self)->set_split_view)
    IDE_LAYOUT_VIEW_GET_CLASS (self)->set_split_view (self, split_view);
}

gchar *
ide_layout_view_get_title (IdeLayoutView *self)
{
  g_return_val_if_fail (IDE_IS_LAYOUT_VIEW (self), NULL);

  if (IDE_LAYOUT_VIEW_GET_CLASS (self)->get_title)
    return IDE_LAYOUT_VIEW_GET_CLASS (self)->get_title (self);

  return g_strdup (_("untitled document"));
}

void
ide_layout_view_set_back_forward_list (IdeLayoutView      *self,
                                       IdeBackForwardList *back_forward_list)
{
  g_return_if_fail (IDE_IS_LAYOUT_VIEW (self));
  g_return_if_fail (IDE_IS_BACK_FORWARD_LIST (back_forward_list));

  if (IDE_LAYOUT_VIEW_GET_CLASS (self)->set_back_forward_list)
    IDE_LAYOUT_VIEW_GET_CLASS (self)->set_back_forward_list (self, back_forward_list);
}

void
ide_layout_view_navigate_to (IdeLayoutView     *self,
                             IdeSourceLocation *location)
{
  g_return_if_fail (IDE_IS_LAYOUT_VIEW (self));
  g_return_if_fail (location != NULL);

  if (IDE_LAYOUT_VIEW_GET_CLASS (self)->navigate_to)
    IDE_LAYOUT_VIEW_GET_CLASS (self)->navigate_to (self, location);
}

gboolean
ide_layout_view_get_modified (IdeLayoutView *self)
{
  g_return_val_if_fail (IDE_IS_LAYOUT_VIEW (self), FALSE);

  if (IDE_LAYOUT_VIEW_GET_CLASS (self)->get_modified)
    return IDE_LAYOUT_VIEW_GET_CLASS (self)->get_modified (self);

  return FALSE;
}

static void
ide_layout_view_notify (GObject    *object,
                        GParamSpec *pspec)
{
  /*
   * XXX:
   *
   * This should get removed after 3.18 when path bar lands.
   * This also notifies of special-title after title is emitted.
   */
  if (pspec == properties [PROP_TITLE])
    g_object_notify_by_pspec (object, properties [PROP_SPECIAL_TITLE]);

  if (G_OBJECT_CLASS (ide_layout_view_parent_class)->notify)
    G_OBJECT_CLASS (ide_layout_view_parent_class)->notify (object, pspec);
}

static void
ide_layout_view_get_property (GObject    *object,
                              guint       prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  IdeLayoutView *self = IDE_LAYOUT_VIEW (object);

  switch (prop_id)
    {
    case PROP_CAN_SPLIT:
      g_value_set_boolean (value, ide_layout_view_get_can_split (self));
      break;

    case PROP_MODIFIED:
      g_value_set_boolean (value, ide_layout_view_get_modified (self));
      break;

    case PROP_SPECIAL_TITLE:
      g_value_take_string (value, ide_layout_view_get_special_title (self));
      break;

    case PROP_TITLE:
      g_value_take_string (value, ide_layout_view_get_title (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
ide_layout_view_class_init (IdeLayoutViewClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = ide_layout_view_get_property;
  object_class->notify = ide_layout_view_notify;

  properties [PROP_CAN_SPLIT] =
    g_param_spec_boolean ("can-split",
                          "Can Split",
                          "If the view can be split.",
                          FALSE,
                          (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

  properties [PROP_MODIFIED] =
    g_param_spec_boolean ("modified",
                          "Modified",
                          "If the document has been modified.",
                          FALSE,
                          (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

  properties [PROP_TITLE] =
    g_param_spec_string ("title",
                         "Title",
                         "The view title.",
                         NULL,
                         (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

  /*
   * TODO:
   *
   * This property should be removed once we move to path bar.  It's purpose is
   * to allow us to special format filenames which may not be needed by other
   * views such as terminal/devhelp/etc.
   */
  properties [PROP_SPECIAL_TITLE] =
    g_param_spec_string ("special-title",
                         "Special Title",
                         "The special title to be displayed in the document menu button.",
                         NULL,
                         (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, LAST_PROP, properties);
}

static void
ide_layout_view_init (IdeLayoutView *self)
{
}

/*
 * TODO:
 *
 * This function is a hack in place for 3.18 until we get the path bar
 * which will provide a better view of file paths. It should be removed
 * after 3.18 when path bar lands. Also remove the "special-title"
 * property.
 */
gchar *
ide_layout_view_get_special_title (IdeLayoutView *self)
{
  gchar *ret = NULL;

  g_return_val_if_fail (IDE_IS_LAYOUT_VIEW (self), NULL);

  if (IDE_LAYOUT_VIEW_GET_CLASS (self)->get_special_title)
    ret = IDE_LAYOUT_VIEW_GET_CLASS (self)->get_special_title (self);

  if (ret == NULL)
    ret = ide_layout_view_get_title (self);

  return ret;
}

gboolean
ide_layout_view_agree_to_close (IdeLayoutView *self)
{
  g_return_val_if_fail (IDE_IS_LAYOUT_VIEW (self), FALSE);

  if (IDE_LAYOUT_VIEW_GET_CLASS (self)->agree_to_close)
    return IDE_LAYOUT_VIEW_GET_CLASS (self)->agree_to_close (self);

  return TRUE;
}

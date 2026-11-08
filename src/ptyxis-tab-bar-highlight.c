/*
 * ptyxis-tab-bar-highlight.c
 *
 * Copyright 2026 Erik Lissel <erik@lissel.net>
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
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/* Draws a shape behind the selected tab that joins it to the header above,
 * curving out into the header on both sides. It is drawn behind the tab bar
 * rather than by the tab, since a tab can't draw outside its own space.
 *
 * The color comes from CSS on a "highlight" child, which takes no space. It
 * is transparent unless the window dressing colors it for a tint.
 */

#include "config.h"

#include "ptyxis-tab-bar-highlight.h"

#define RADIUS 9.f

struct _PtyxisTabBarHighlight
{
  AdwBin     parent_instance;
  GtkWidget *style;
};

G_DEFINE_FINAL_TYPE (PtyxisTabBarHighlight, ptyxis_tab_bar_highlight, ADW_TYPE_BIN)

#define PTYXIS_TYPE_HIGHLIGHT_STYLE (ptyxis_highlight_style_get_type())
G_DECLARE_FINAL_TYPE (PtyxisHighlightStyle, ptyxis_highlight_style, PTYXIS, HIGHLIGHT_STYLE, GtkWidget)

struct _PtyxisHighlightStyle
{
  GtkWidget parent_instance;
};

G_DEFINE_FINAL_TYPE (PtyxisHighlightStyle, ptyxis_highlight_style, GTK_TYPE_WIDGET)

static void
ptyxis_highlight_style_css_changed (GtkWidget         *widget,
                                    GtkCssStyleChange *change)
{
  GTK_WIDGET_CLASS (ptyxis_highlight_style_parent_class)->css_changed (widget, change);

  gtk_widget_queue_draw (gtk_widget_get_parent (widget));
}

static void
ptyxis_highlight_style_class_init (PtyxisHighlightStyleClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  widget_class->css_changed = ptyxis_highlight_style_css_changed;

  gtk_widget_class_set_css_name (widget_class, "highlight");
}

static void
ptyxis_highlight_style_init (PtyxisHighlightStyle *self)
{
  gtk_widget_set_can_target (GTK_WIDGET (self), FALSE);
}

static GtkWidget *
find_selected_tab (GtkWidget *widget,
                   GType      tab_type)
{
  if (G_TYPE_CHECK_INSTANCE_TYPE (widget, tab_type))
    return (gtk_widget_get_state_flags (widget) & GTK_STATE_FLAG_SELECTED) ? widget : NULL;

  for (GtkWidget *child = gtk_widget_get_first_child (widget);
       child != NULL;
       child = gtk_widget_get_next_sibling (child))
    {
      GtkWidget *found;

      if (gtk_widget_get_mapped (child) &&
          (found = find_selected_tab (child, tab_type)))
        return found;
    }

  return NULL;
}

static void
ptyxis_tab_bar_highlight_snapshot (GtkWidget   *widget,
                                   GtkSnapshot *snapshot)
{
  PtyxisTabBarHighlight *self = PTYXIS_TAB_BAR_HIGHLIGHT (widget);
  g_autoptr(GskPathBuilder) builder = NULL;
  g_autoptr(GskPath) path = NULL;
  GtkWidget *child = adw_bin_get_child (ADW_BIN (self));
  GtkWidget *tab;
  GtkWidget *scroller;
  graphene_rect_t tab_bounds;
  graphene_rect_t clip;
  GdkRGBA color;
  GType tab_type;
  float x0, x1, bottom;

  gtk_widget_get_color (self->style, &color);

  if (color.alpha > 0 &&
      child != NULL &&
      (tab_type = g_type_from_name ("AdwTab")) != G_TYPE_INVALID &&
      (tab = find_selected_tab (child, tab_type)) &&
      gtk_widget_compute_bounds (tab, widget, &tab_bounds))
    {
      x0 = tab_bounds.origin.x;
      x1 = x0 + tab_bounds.size.width;
      bottom = tab_bounds.origin.y + tab_bounds.size.height;

      /* Hide the parts of a tab scrolled out of view, but let the curves
       * reach past the ends of the scrolled area.
       */
      if ((scroller = gtk_widget_get_ancestor (tab, GTK_TYPE_SCROLLED_WINDOW)) &&
          gtk_widget_compute_bounds (scroller, widget, &clip))
        graphene_rect_inset (&clip, -RADIUS, 0);
      else
        graphene_rect_init (&clip, 0, 0, gtk_widget_get_width (widget), gtk_widget_get_height (widget));

      builder = gsk_path_builder_new ();
      gsk_path_builder_move_to (builder, x0 - RADIUS, 0);
      gsk_path_builder_conic_to (builder, x0, 0, x0, RADIUS, M_SQRT1_2);
      gsk_path_builder_line_to (builder, x0, bottom - RADIUS);
      gsk_path_builder_conic_to (builder, x0, bottom, x0 + RADIUS, bottom, M_SQRT1_2);
      gsk_path_builder_line_to (builder, x1 - RADIUS, bottom);
      gsk_path_builder_conic_to (builder, x1, bottom, x1, bottom - RADIUS, M_SQRT1_2);
      gsk_path_builder_line_to (builder, x1, RADIUS);
      gsk_path_builder_conic_to (builder, x1, 0, x1 + RADIUS, 0, M_SQRT1_2);
      gsk_path_builder_close (builder);
      path = gsk_path_builder_free_to_path (g_steal_pointer (&builder));

      gtk_snapshot_push_clip (snapshot, &clip);
      gtk_snapshot_append_fill (snapshot, path, GSK_FILL_RULE_WINDING, &color);
      gtk_snapshot_pop (snapshot);
    }

  if (child != NULL)
    gtk_widget_snapshot_child (widget, child, snapshot);
}

static void
ptyxis_tab_bar_highlight_size_allocate (GtkWidget *widget,
                                        int        width,
                                        int        height,
                                        int        baseline)
{
  PtyxisTabBarHighlight *self = PTYXIS_TAB_BAR_HIGHLIGHT (widget);

  GTK_WIDGET_CLASS (ptyxis_tab_bar_highlight_parent_class)->size_allocate (widget, width, height, baseline);

  gtk_widget_allocate (self->style, 0, 0, -1, NULL);
}

static void
ptyxis_tab_bar_highlight_dispose (GObject *object)
{
  PtyxisTabBarHighlight *self = PTYXIS_TAB_BAR_HIGHLIGHT (object);

  g_clear_pointer (&self->style, gtk_widget_unparent);

  G_OBJECT_CLASS (ptyxis_tab_bar_highlight_parent_class)->dispose (object);
}

static void
ptyxis_tab_bar_highlight_class_init (PtyxisTabBarHighlightClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = ptyxis_tab_bar_highlight_dispose;

  widget_class->snapshot = ptyxis_tab_bar_highlight_snapshot;
  widget_class->size_allocate = ptyxis_tab_bar_highlight_size_allocate;

  gtk_widget_class_set_css_name (widget_class, "tabbarhighlight");
}

static void
ptyxis_tab_bar_highlight_init (PtyxisTabBarHighlight *self)
{
  self->style = g_object_new (PTYXIS_TYPE_HIGHLIGHT_STYLE, NULL);
  gtk_widget_set_parent (self->style, GTK_WIDGET (self));
}

/* Ruby bindings for the Clutter 'interactive canvas' library.
 * Copyright (C) 2007-2008  Neil Roberts
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301  USA
 */

#include <rbgobject.h>
#include <clutter/clutter.h>

#include "rbclutter.h"

static VALUE
rbclt_stage_initialize (VALUE self)
{
  ClutterActor *stage;

  stage = clutter_stage_new ();

  rbclt_initialize_unowned (self, stage);

  return Qnil;
}

static VALUE
rbclt_stage_get_default ()
{
  ClutterActor *actor = clutter_stage_get_default ();

  if (actor == NULL)
    return Qnil;
  else
    return GOBJ2RVAL (actor);
}

static VALUE
rbclt_stage_show_cursor (VALUE self)
{
  ClutterStage *stage = CLUTTER_STAGE (RVAL2GOBJ (self));
  clutter_stage_show_cursor (stage);
  return self;
}

static VALUE
rbclt_stage_hide_cursor (VALUE self)
{
  ClutterStage *stage = CLUTTER_STAGE (RVAL2GOBJ (self));
  clutter_stage_hide_cursor (stage);
  return self;
}

static VALUE
rbclt_stage_get_actor_at_pos (VALUE self, VALUE pick_mode, VALUE x, VALUE y)
{
  ClutterStage *stage = CLUTTER_STAGE (RVAL2GOBJ (self));
  ClutterPickMode pick_mode_value = RVAL2GENUM (pick_mode,
                                                CLUTTER_TYPE_PICK_MODE);
  return GOBJ2RVAL (clutter_stage_get_actor_at_pos (stage,
                                                    pick_mode_value,
                                                    NUM2INT (x),
                                                    NUM2INT (y)));
}

static VALUE
rbclt_stage_event (VALUE self, VALUE event_arg)
{
  ClutterStage *stage = CLUTTER_STAGE (RVAL2GOBJ (self));
  ClutterEvent *event = (ClutterEvent *) RVAL2BOXED (event_arg,
                                                     CLUTTER_TYPE_EVENT);

  clutter_stage_event (stage, event);

  return self;
}

static VALUE
rbclt_stage_read_pixels (VALUE self, VALUE x, VALUE y,
                         VALUE width_arg, VALUE height_arg)
{
  ClutterStage *stage = CLUTTER_STAGE (RVAL2GOBJ (self));
  guchar *pixels;
  VALUE ret;
  gint width, height;

  width = NUM2INT (width_arg);
  height = NUM2INT (height_arg);

  pixels = clutter_stage_read_pixels (stage, NUM2INT (x), NUM2INT (y),
                                      width, height);

  if (pixels == NULL)
    ret = Qnil;
  else
    {
      ret = rb_str_new ((char *) pixels, width * 4 * height);

      g_free (pixels);
    }

  return ret;
}

static VALUE
rbclt_stage_set_key_focus (VALUE self, VALUE actor)
{
  ClutterStage *stage = CLUTTER_STAGE (RVAL2GOBJ (self));

  clutter_stage_set_key_focus (stage, RVAL2GOBJ (actor));

  return self;
}

static VALUE
rbclt_stage_get_key_focus (VALUE self)
{
  ClutterStage *stage = CLUTTER_STAGE (RVAL2GOBJ (self));

  return GOBJ2RVAL (clutter_stage_get_key_focus (stage));
}

static VALUE
rbclt_stage_ensure_current (VALUE self)
{
  ClutterStage *stage = CLUTTER_STAGE (RVAL2GOBJ (self));

  clutter_stage_ensure_current (stage);

  return self;
}

static VALUE
rbclt_stage_is_default (VALUE self)
{
  ClutterStage *stage = CLUTTER_STAGE (RVAL2GOBJ (self));

  return clutter_stage_is_default (stage) ? Qtrue : Qfalse;
}

static VALUE
rbclt_stage_ensure_redraw (VALUE self)
{
  ClutterStage *stage = CLUTTER_STAGE (RVAL2GOBJ (self));
  clutter_stage_ensure_redraw (stage);
  return Qnil;
}

static VALUE
rbclt_stage_ensure_viewport (VALUE self)
{
  ClutterStage *stage = CLUTTER_STAGE (RVAL2GOBJ (self));
  clutter_stage_ensure_viewport (stage);
  return Qnil;
}


void
rbclt_stage_init ()
{
  VALUE klass = G_DEF_CLASS (CLUTTER_TYPE_STAGE, "Stage", rbclt_c_clutter);

  rb_define_singleton_method (klass, "get_default", rbclt_stage_get_default, 0);

  rb_define_method (klass, "initialize", rbclt_stage_initialize, 0);
  rb_define_method (klass, "show_cursor", rbclt_stage_show_cursor, 0);
  rb_define_method (klass, "hide_cursor", rbclt_stage_hide_cursor, 0);
  rb_define_method (klass, "get_actor_at_pos", rbclt_stage_get_actor_at_pos, 3);
  rb_define_method (klass, "event", rbclt_stage_event, 1);
  rb_define_method (klass, "read_pixels", rbclt_stage_read_pixels, 4);
  rb_define_method (klass, "set_key_focus", rbclt_stage_set_key_focus, 1);
  rb_define_method (klass, "key_focus", rbclt_stage_get_key_focus, 0);
  rb_define_method (klass, "ensure_current", rbclt_stage_ensure_current, 0);
  rb_define_method (klass, "default?", rbclt_stage_is_default, 0);
  rb_define_method (klass, "ensure_redraw", rbclt_stage_ensure_redraw, 0);
  rb_define_method (klass, "ensure_viewport", rbclt_stage_ensure_viewport, 0);

  G_DEF_SETTERS (klass);
}

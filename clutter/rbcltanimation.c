/* Ruby bindings for the Clutter 'interactive canvas' library.
 * Copyright (C) 2010  Intel Corporation
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
#include <st.h>

#include "rbclutter.h"
#include "rbcltactor.h"

static VALUE
rbclt_animation_intialize (VALUE self)
{
  G_INITIALIZE (self, clutter_animation_new ());

  return Qnil;
}

static VALUE
rbclt_animation_bind_property (VALUE self, VALUE property_name, VALUE final)
{
  ClutterAnimation *anim = CLUTTER_ANIMATION (RVAL2GOBJ (self));
  GValue gval = { 0, };

  g_value_init (&gval, RVAL2GTYPE (final));
  rbgobj_rvalue_to_gvalue (final, &gval);
  clutter_animation_bind (anim, StringValuePtr (property_name), &gval);
  g_value_unset (&gval);

  return self;
}

static VALUE
rbclt_animation_bind_interval (VALUE self, VALUE property_name, VALUE interval)
{
  ClutterAnimation *animation = CLUTTER_ANIMATION (RVAL2GOBJ (self));

  clutter_animation_bind_interval (animation,
                                   StringValuePtr (property_name),
                                   RVAL2GOBJ (interval));

  return self;
}

static VALUE
rbclt_animation_has_property (VALUE self, VALUE property_name)
{
  ClutterAnimation *animation = CLUTTER_ANIMATION (RVAL2GOBJ (self));

  return (clutter_animation_has_property (animation,
                                         StringValuePtr (property_name))
          ? Qtrue : Qfalse);
}

static VALUE
rbclt_animation_update_property (VALUE self, VALUE property_name, VALUE final)
{
  ClutterAnimation *anim = CLUTTER_ANIMATION (RVAL2GOBJ (self));
  GValue gval = { 0, };

  g_value_init (&gval, RVAL2GTYPE (final));
  rbgobj_rvalue_to_gvalue (final, &gval);
  clutter_animation_update (anim, StringValuePtr (property_name), &gval);
  g_value_unset (&gval);

  return self;
}

static VALUE
rbclt_animation_update_interval (VALUE self,
                                 VALUE property_name,
                                 VALUE interval)
{
  ClutterAnimation *animation = CLUTTER_ANIMATION (RVAL2GOBJ (self));

  clutter_animation_update_interval (animation,
                                     StringValuePtr (property_name),
                                     RVAL2GOBJ (interval));

  return self;
}

static VALUE
rbclt_animation_unbind_property (VALUE self, VALUE property_name)
{
  ClutterAnimation *animation = CLUTTER_ANIMATION (RVAL2GOBJ (self));

  clutter_animation_unbind_property (animation, StringValuePtr (property_name));

  return self;
}

static VALUE
rbclt_animation_get_interval (VALUE self, VALUE property_name)
{
  ClutterAnimation *animation = CLUTTER_ANIMATION (RVAL2GOBJ (self));
  ClutterInterval *interval;

  interval = clutter_animation_get_interval (animation,
                                             StringValuePtr (property_name));
  return GOBJ2RVAL (interval);
}

static VALUE
rbclt_animation_completed (VALUE self)
{
  ClutterAnimation *animation = CLUTTER_ANIMATION (RVAL2GOBJ (self));

  clutter_animation_completed (animation);

  return self;
}

static VALUE
rbclt_animation_get_animation (VALUE self)
{
  ClutterActor *actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  return GOBJ2RVAL (clutter_actor_get_animation (actor));
}

typedef struct _AnimateData AnimateData;

struct _AnimateData
{
  VALUE hash;
  ClutterActor *actor;
  ClutterTimeline *timeline;
  ClutterAlpha *alpha;
  gulong mode;
  guint duration;

  GArray *name_array;
  GValueArray *value_array;
};

static int
rbclt_animation_hash_cb (VALUE key, VALUE value, VALUE arg)
{
  AnimateData *data = (AnimateData *) arg;
  char *name;
  GValue gval = { 0, };

  name = g_strdup (StringValuePtr (key));
  g_array_append_val (data->name_array, name);

  g_value_init (&gval, RVAL2GTYPE (value));
  rbgobj_rvalue_to_gvalue (value, &gval);
  g_value_array_append (data->value_array, &gval);
  g_value_unset (&gval);

  return ST_CONTINUE;
}

static VALUE
rbclt_animation_do_animate (VALUE arg)
{
  AnimateData *data = (AnimateData *) arg;
  ClutterAnimation *anim;
  const gchar * const* names;
  GValue *values;

  rb_hash_foreach (data->hash,
                   rbclt_animation_hash_cb,
                   (VALUE) data);

  names = &g_array_index (data->name_array, const char *, 0);
  values = data->value_array->values;


  if (data->alpha)
    anim = clutter_actor_animate_with_alphav (data->actor,
                                              data->alpha,
                                              data->name_array->len,
                                              names, values);
  else if (data->timeline)
    anim = clutter_actor_animate_with_timelinev (data->actor,
                                                 data->mode,
                                                 data->timeline,
                                                 data->name_array->len,
                                                 names, values);
  else
    anim = clutter_actor_animatev (data->actor,
                                   data->mode,
                                   data->duration,
                                   data->name_array->len,
                                   names, values);

  return GOBJ2RVAL (anim);
}

static VALUE
rbclt_animation_free_animate_data (VALUE arg)
{
  AnimateData *data = (AnimateData *) arg;
  int i;

  for (i = 0; i < data->name_array->len; i++)
    g_free (g_array_index (data->name_array, char *, i));
  g_array_free (data->name_array, TRUE);
  g_value_array_free (data->value_array);

  return Qnil;
}

static VALUE
rbclt_animation_animate_with_data (AnimateData *data)
{
  data->name_array = g_array_new (FALSE, FALSE, sizeof (char *));
  data->value_array = g_value_array_new (0);

  return rb_ensure (rbclt_animation_do_animate, (VALUE) data,
                    rbclt_animation_free_animate_data, (VALUE) data);
}

static VALUE
rbclt_animation_animate (VALUE self, VALUE mode, VALUE duration, VALUE hash)
{
  AnimateData data;

  data.actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  data.timeline = NULL;
  data.alpha = NULL;
  data.mode = NUM2ULONG (mode);
  data.duration = NUM2UINT (duration);
  data.hash = hash;

  return rbclt_animation_animate_with_data (&data);
}

static VALUE
rbclt_animation_animate_with_timeline (VALUE self, VALUE mode,
                                       VALUE timeline, VALUE hash)
{
  AnimateData data;

  data.actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  data.timeline = RVAL2GOBJ (timeline);
  data.alpha = NULL;
  data.mode = NUM2ULONG (mode);
  data.duration = 0;
  data.hash = hash;

  return rbclt_animation_animate_with_data (&data);
}

static VALUE
rbclt_animation_animate_with_alpha (VALUE self, VALUE alpha, VALUE hash)
{
  AnimateData data;

  data.actor = CLUTTER_ACTOR (RVAL2GOBJ (self));
  data.timeline = NULL;
  data.alpha = RVAL2GOBJ (alpha);
  data.mode = 0;
  data.duration = 0;
  data.hash = hash;

  return rbclt_animation_animate_with_data (&data);
}

void
rbclt_animation_init ()
{
  VALUE klass = G_DEF_CLASS (CLUTTER_TYPE_ANIMATION,
                             "Animation", rbclt_c_clutter);

  rb_define_method (klass, "intialize", rbclt_animation_intialize, 0);
  rb_define_method (klass, "bind", rbclt_animation_bind_property, 2);
  rb_define_method (klass, "bind_interval", rbclt_animation_bind_interval, 2);
  rb_define_method (klass, "has_property?", rbclt_animation_has_property, 1);
  rb_define_method (klass, "update", rbclt_animation_update_property, 2);
  rb_define_method (klass, "update_interval",
                    rbclt_animation_update_interval, 2);
  rb_define_method (klass, "unbind_property",
                    rbclt_animation_unbind_property, 1);
  rb_define_method (klass, "get_interval", rbclt_animation_get_interval, 1);
  rb_define_method (klass, "completed", rbclt_animation_completed, 0);

  /* Actor methods */
  rb_define_method (rbclt_c_actor, "animation",
                    rbclt_animation_get_animation, 0);
  rb_define_method (rbclt_c_actor, "animate", rbclt_animation_animate, 3);
  rb_define_method (rbclt_c_actor, "animate_with_timeline",
                    rbclt_animation_animate_with_timeline, 3);
  rb_define_method (rbclt_c_actor, "animate_with_alpha",
                    rbclt_animation_animate_with_alpha, 2);
}

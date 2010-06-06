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

#include "rbclutter.h"

typedef struct _InitializeData InitializeData;

struct _InitializeData
{
  VALUE self;
  GType value_type;
  GValue initial_gval, final_gval;
  VALUE initial_value, final_value;
};

static VALUE
rbclt_interval_do_initialize (VALUE arg)
{
  ClutterInterval *interval;
  InitializeData *data = (InitializeData *) arg;

  rbgobj_rvalue_to_gvalue (data->initial_value, &data->initial_gval);
  rbgobj_rvalue_to_gvalue (data->final_value, &data->final_gval);

  interval = clutter_interval_new_with_values (data->value_type,
                                               &data->initial_gval,
                                               &data->final_gval);

  rbclt_initialize_unowned (data->self, interval);

  return Qnil;
}

static VALUE
rbclt_interval_free_init_data (VALUE arg)
{
  InitializeData *data = (InitializeData *) arg;

  g_value_unset (&data->initial_gval);
  g_value_unset (&data->final_gval);

  return Qnil;
}

static VALUE
rbclt_interval_initialize (VALUE self, VALUE klass,
                           VALUE initial_value, VALUE final_value)
{
  InitializeData data;

  data.self = self;
  data.value_type = rbgobj_gtype_get (klass);
  data.initial_value = initial_value;
  data.final_value = final_value;

  memset (&data.initial_gval, 0, sizeof (GValue));
  memset (&data.final_gval, 0, sizeof (GValue));
  g_value_init (&data.initial_gval, data.value_type);
  g_value_init (&data.final_gval, data.value_type);

  return rb_ensure (rbclt_interval_do_initialize, (VALUE) &data,
                    rbclt_interval_free_init_data, (VALUE) &data);
}

static VALUE
rbclt_interval_dup (VALUE self)
{
  ClutterInterval *interval = CLUTTER_INTERVAL (RVAL2GOBJ (self));
  return GOBJ2RVAL (clutter_interval_clone (interval));
}

static VALUE
rbclt_interval_get_initial_value (VALUE self)
{
  ClutterInterval *interval = CLUTTER_INTERVAL (RVAL2GOBJ (self));

  return GVAL2RVAL (clutter_interval_peek_initial_value (interval));
}

static VALUE
rbclt_interval_get_final_value (VALUE self)
{
  ClutterInterval *interval = CLUTTER_INTERVAL (RVAL2GOBJ (self));

  return GVAL2RVAL (clutter_interval_peek_final_value (interval));
}

typedef struct _SetValueData SetValueData;

struct _SetValueData
{
  VALUE value;
  GValue gval;
  ClutterInterval *interval;
  void (* set_value_func) (ClutterInterval *, const GValue *);
};

static VALUE
rbclt_interval_do_set_value (VALUE arg)
{
  SetValueData *data = (SetValueData *) arg;

  rbgobj_rvalue_to_gvalue (data->value, &data->gval);

  data->set_value_func (data->interval, &data->gval);

  return Qnil;
}

static VALUE
rbclt_interval_free_set_value_data (VALUE arg)
{
  SetValueData *data = (SetValueData *) arg;

  g_value_unset (&data->gval);

  return Qnil;
}

static VALUE
rbclt_interval_set_initial_value (VALUE self, VALUE initial_value)
{
  ClutterInterval *interval = CLUTTER_INTERVAL (RVAL2GOBJ (self));
  SetValueData data;

  data.interval = interval;
  data.set_value_func = clutter_interval_set_initial_value;
  g_value_init (&data.gval, clutter_interval_get_value_type (interval));

  rb_ensure (rbclt_interval_do_set_value, (VALUE) &data,
             rbclt_interval_free_set_value_data, (VALUE) &data);

  return self;
}

static VALUE
rbclt_interval_set_final_value (VALUE self, VALUE final_value)
{
  ClutterInterval *interval = CLUTTER_INTERVAL (RVAL2GOBJ (self));
  SetValueData data;

  data.interval = interval;
  data.set_value_func = clutter_interval_set_final_value;
  g_value_init (&data.gval, clutter_interval_get_value_type (interval));

  rb_ensure (rbclt_interval_do_set_value, (VALUE) &data,
             rbclt_interval_free_set_value_data, (VALUE) &data);

  return self;
}

static VALUE
rbclt_interval_set_interval (VALUE self, VALUE initial_value, VALUE final_value)
{
  rbclt_interval_set_initial_value (self, initial_value);
  rbclt_interval_set_final_value (self, final_value);

  return self;
}

static VALUE
rbclt_interval_compute_value (VALUE self, VALUE factor)
{
  ClutterInterval *interval = CLUTTER_INTERVAL (RVAL2GOBJ (self));
  GValue gval = { 0, };
  VALUE result;

  g_value_init (&gval, clutter_interval_get_value_type (interval));
  clutter_interval_compute_value (interval, NUM2DBL (factor), &gval);
  result = GVAL2RVAL (&gval);
  g_value_unset (&gval);

  return result;
}

static VALUE
rbclt_interval_validate (VALUE self, VALUE pspec_arg)
{
  ClutterInterval *interval = CLUTTER_INTERVAL (RVAL2GOBJ (self));
  GParamSpec *pspec = G_PARAM_SPEC (RVAL2GOBJ (pspec_arg));

  return clutter_interval_validate (interval, pspec) ? Qtrue : Qfalse;
}

static VALUE
rbclt_interval_get_interval (VALUE self)
{
  return rb_ary_new3 (2,
                      rbclt_interval_get_initial_value (self),
                      rbclt_interval_get_final_value (self));
}

void
rbclt_interval_init ()
{
  VALUE klass = G_DEF_CLASS (CLUTTER_TYPE_INTERVAL,
                             "Interval", rbclt_c_clutter);

  rb_define_method (klass, "initialize", rbclt_interval_initialize, 3);
  rb_define_method (klass, "dup", rbclt_interval_dup, 0);
  rb_define_method (klass, "initial_value",
                    rbclt_interval_get_initial_value, 0);
  rb_define_method (klass, "set_initial_value",
                    rbclt_interval_set_initial_value, 1);
  rb_define_method (klass, "final_value",
                    rbclt_interval_get_final_value, 0);
  rb_define_method (klass, "set_final_value",
                    rbclt_interval_set_final_value, 1);
  rb_define_method (klass, "set_interval", rbclt_interval_set_interval, 2);
  rb_define_method (klass, "compute_value", rbclt_interval_compute_value, 1);
  rb_define_alias (klass, "compute", "compute_value");
  rb_define_method (klass, "validate", rbclt_interval_validate, 1);
  rb_define_method (klass, "interval", rbclt_interval_get_interval, 0);

  G_DEF_SETTERS (klass);
}

/* Ruby bindings for the Clutter 'interactive canvas' library.
 * Copyright (C) 2007  Neil Roberts
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
#include "rbcltcallbackfunc.h"

static ID id_call;

static gdouble
rbclt_alpha_proc_invoke (ClutterAlpha *alpha, RBCLTCallbackFunc *callback)
{
  VALUE alpha_value = GOBJ2RVAL (alpha);

  return NUM2DBL (rbclt_callback_func_invoke (callback, 1, &alpha_value));
}

static GClosure *
rbclt_alpha_proc_to_closure (VALUE proc)
{
  if (NIL_P (proc))
    {
      rb_need_block ();
      proc = rb_block_proc ();
    }

  return g_cclosure_new (G_CALLBACK (rbclt_alpha_proc_invoke),
                         rbclt_callback_func_new (proc),
                         (GClosureNotify) rbclt_callback_func_destroy);
}

static VALUE
rbclt_alpha_initialize (int argc, VALUE *argv, VALUE self)
{
  VALUE timeline, mode;
  ClutterAlpha *alpha;

  rb_scan_args (argc, argv, "02", &timeline, &mode);

  alpha = clutter_alpha_new ();
  rbclt_initialize_unowned (self, alpha);

  if (timeline != Qnil)
    clutter_alpha_set_timeline (alpha, RVAL2GOBJ (timeline));

  if (NIL_P (mode))
    {
      if (rb_block_given_p ())
        clutter_alpha_set_closure (alpha,
                                   rbclt_alpha_proc_to_closure (Qnil));
    }
  /* If the mode quacks like a Proc then we'll treat it as one */
  else if (rb_respond_to (mode, id_call))
    clutter_alpha_set_closure (alpha, rbclt_alpha_proc_to_closure (mode));
  /* Otherwise we'll assume it's an integer mode */
  else
    clutter_alpha_set_mode (alpha, NUM2UINT (mode));

  return Qnil;
}

static VALUE
rbclt_alpha_register_func (int argc, VALUE *argv, VALUE self)
{
  VALUE proc;
  GClosure *closure;
  gulong id;

  rb_scan_args (argc, argv, "01", &proc);

  closure = rbclt_alpha_proc_to_closure (proc);

  id = clutter_alpha_register_closure (closure);

  return ULONG2NUM (id);
}

void
rbclt_alpha_init ()
{
  VALUE klass = G_DEF_CLASS (CLUTTER_TYPE_ALPHA, "Alpha", rbclt_c_clutter);

  id_call = rb_intern ("call");

  rb_define_method (klass, "initialize", rbclt_alpha_initialize, -1);

  rb_define_singleton_method (klass, "register_func",
                              rbclt_alpha_register_func, -1);
  rb_define_singleton_method (klass, "register_closure",
                              rbclt_alpha_register_func, -1);

  G_DEF_CLASS (CLUTTER_TYPE_ANIMATION_MODE, "AnimationMode", rbclt_c_clutter);
  G_DEF_CONSTANTS (rbclt_c_clutter, CLUTTER_TYPE_ANIMATION_MODE, "CLUTTER_");
}

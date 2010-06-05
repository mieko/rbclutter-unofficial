/* Ruby bindings for the Clutter 'interactive canvas' library.
 * Copyright (C) 2007  Neil Roberts
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
rbclt_media_set_filename (VALUE self, VALUE filename)
{
  ClutterMedia *media = CLUTTER_MEDIA (RVAL2GOBJ (self));
  clutter_media_set_filename (media, StringValuePtr (filename));
  return self;
}

static VALUE
rbclt_media_filename_equals (VALUE self, VALUE filename)
{
  ClutterMedia *media = CLUTTER_MEDIA (RVAL2GOBJ (self));
  clutter_media_set_filename (media, StringValuePtr (filename));
  return filename;
}

void
rbclt_media_init ()
{
  VALUE klass = G_DEF_INTERFACE (CLUTTER_TYPE_MEDIA, "Media", rbclt_c_clutter);

  rb_define_method (klass, "set_filename", rbclt_media_set_filename, 1);
  rb_define_method (klass, "filename=", rbclt_media_filename_equals, 1);

  /* All of the other methods are properties so they will get defined
     automatically */
}

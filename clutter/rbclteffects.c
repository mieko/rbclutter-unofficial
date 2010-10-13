#include <ruby.h>
#include <rbgobject.h>
#include <glib-object.h>
#include <clutter/clutter.h>

#include "rbclutter.h"

VALUE rbclt_c_clutter_effect;

void rbclt_effects_init ()
{
  VALUE mod;

  G_DEF_CLASS (CLUTTER_TYPE_ACTOR_META, "ActorMeta",
               rbclt_c_clutter);

  mod = rb_define_module_under (rbclt_c_clutter, "Effect");
  rbclt_c_clutter_effect = mod;

  G_DEF_CLASS (CLUTTER_TYPE_EFFECT, "Effect", mod);
  G_DEF_CLASS (CLUTTER_TYPE_OFFSCREEN_EFFECT, "Offscreen", mod);
  G_DEF_CLASS (CLUTTER_TYPE_BLUR_EFFECT, "Blur", mod);
  G_DEF_CLASS (CLUTTER_TYPE_SHADER_EFFECT, "Shader", mod);
  G_DEF_CLASS (CLUTTER_TYPE_COLORIZE_EFFECT, "Colorize", mod);
  G_DEF_CLASS (CLUTTER_TYPE_DESATURATE_EFFECT, "Desaturate", mod);
  G_DEF_CLASS (CLUTTER_TYPE_DEFORM_EFFECT, "Deform", mod);
  G_DEF_CLASS (CLUTTER_TYPE_PAGE_TURN_EFFECT, "PageTurn", mod);
}
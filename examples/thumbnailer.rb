#!/usr/bin/ruby

# Ruby bindings for the Clutter 'interactive canvas' library.
# Copyright (C) 2007-2008  Neil Roberts
# Copyright (C) 2010  Intel Corporation
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA  02110-1301  USA

require 'clutter'
require 'gdk_pixbuf2'

class Thumbnailer
  THUMBNAIL_SIZE = 160

  def initialize(filenames)
    @filenames = filenames.dup

    # Next image to load
    @next_image = 0
    # Next position to load to
    @next_position = 0

    # Keep track of all the actors we added so we can remove them
    # easily
    @actors = []

    stage = Clutter::Stage.get_default
    # Number of actors to fit horizontally
    @actors_x = (stage.width / THUMBNAIL_SIZE).to_i
    # Number of actors to fit vertically
    @actors_y = (stage.height / THUMBNAIL_SIZE).to_i

    @start_x = stage.width / 2.0 - THUMBNAIL_SIZE / 2.0
    @start_y = stage.height

    # Load the first actor
    load_actor
  end

  private
  def load_actor
    if @next_image < @filenames.size
      # Pre-scale the image using GDK pixbuf so that it doesn't have
      # to be scaled every frame by clutter
      pixbuf = Gdk::Pixbuf.new(@filenames[@next_image],
                               THUMBNAIL_SIZE, THUMBNAIL_SIZE)
      @next_image += 1

      actor = Clutter::Texture.new
      actor.set_from_rgb_data(pixbuf.pixels,
                              pixbuf.has_alpha?,
                              pixbuf.width,
                              pixbuf.height,
                              pixbuf.rowstride,
                              pixbuf.has_alpha? ? 4 : 3, 0)

      # Set the actor to animate itself into the right position
      actor.set_position(@start_x, @start_y)
      anim = actor.animate(Clutter::EASE_OUT_SINE, 400,
                           "x" => (@next_position % @actors_x) * THUMBNAIL_SIZE,
                           "y" => (@next_position / @actors_x) * THUMBNAIL_SIZE)

      Clutter::Stage.get_default << actor

      # Listen for when the animation is complete so we can start another
      anim.signal_connect_after("completed") { animate_in_completed_cb(actor) }

      @actors << actor

      @next_position += 1
    end
  end

  private
  def animate_in_completed_cb(actor)
    # Actor has finished being added
    # If the screen is full then start removing all of the actors
    if @next_position >= @actors_x * @actors_y
      @actors.each do |actor|
        anim = actor.animate(Clutter::EASE_OUT_SINE, 1000,
                             "x" => @start_x,
                             "y" => @start_y)
        anim.signal_connect_after("completed") do
          animate_out_completed_cb(anim.object)
        end
      end
    else
      # Start another actor
      load_actor
    end
  end

  private
  def animate_out_completed_cb(actor)
    # Actor has finished being removed
    @actors.delete(actor)
    actor.destroy
    # If it's the last actor then we can start loading another with
    # a clean screen
    if @actors.length == 0
      @next_position = 0
      load_actor
    end
  end
end

Clutter::init

if ARGV.size < 1
  bn = File.basename($0)
  STDERR.print("usage: #{File.basename($0)} <image>...\n")
  exit(1)
end

stage = Clutter::Stage.get_default
stage.color = Clutter::Color.new(0, 0, 0)

stage.signal_connect("key_press_event") do |stage, event|
  if event.keyval == Clutter::Key_q || event.keyval == Clutter::Key_Q \
    || event.keyval == Clutter::Key_Escape
    Clutter::main_quit
  end
end

# The main app instance needs to be stored in a variable to keep it
# alive
thumbnailer = Thumbnailer.new(ARGV)

stage.show

Clutter::main

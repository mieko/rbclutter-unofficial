#!/usr/bin/ruby

# This is needed to ensure that clutter.so is loaded before
# clutter_ply.so so that it can reference symbols in clutter.so

require 'clutter'
require 'mash.so'

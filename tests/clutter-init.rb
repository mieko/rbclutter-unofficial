# Convenience module to require clutter and then initialize
# it. Clutter::init can't be called directly in the test cases because
# we only want to call it once and the test cases could be run
# together

require 'clutter'
Clutter::init

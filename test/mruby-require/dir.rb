control_dir = File.dirname(__FILE__)
compare_dir = __dir__

puts <<TEXT

__dir__ (should be #{control_dir.inspect})
  Value: #{compare_dir.inspect}

TEXT


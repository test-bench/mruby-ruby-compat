file = 'examples/some_file'

puts <<TEXT

Require a file relative to __FILE__
  -> require_relative '#{file}'
TEXT

result = require_relative(file)

puts <<TEXT
  => #{result}

Require a file relative to __FILE__ again (should not load a second time)
  -> require_relative '#{file}'
TEXT

result = require_relative(file)

puts <<TEXT
  => #{result}

Require a file relative to __FILE__ with .rb extension (extension should not matter)
  -> require_relative '#{file}.rb'
TEXT

result = require_relative("#{file}.rb")

puts <<TEXT
  => #{result}

Require a file not found relative to __FILE__ (should raise a LoadError)
  -> require_relative './file/not/found'
TEXT

begin
  require_relative('./file/not/found')
rescue LoadError => load_error
end

puts <<TEXT
  #{load_error.backtrace[0]}: #{load_error.to_s} (#{load_error.class})

TEXT

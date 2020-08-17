$LOAD_PATH.push(File.dirname(__FILE__))

file = 'examples/some_file'

puts <<TEXT

Require a file
  -> require '#{file}'
TEXT

result = require(file)

puts <<TEXT
  => #{result}

Require a file again (should not load a second time)
  -> require '#{file}'
TEXT

result = require(file)

puts <<TEXT
  => #{result}

Require a file with .rb extension (extension should not matter)
  -> require '#{file}.rb'
TEXT

result = require("#{file}.rb")

puts <<TEXT
  => #{result}

Require a file not found in load path (should raise a LoadError)
  -> require 'file/not/found'
TEXT

begin
  require('file/not/found')
rescue LoadError => load_error
end

puts <<TEXT
  #{load_error.backtrace[0]}: #{load_error.to_s} (#{load_error.class})

TEXT

loader_file = 'some_namespace/some_file'.freeze

$MRUBY_COMPILED_LIBRARIES[loader_file] = true

puts <<TEXT

Require a library compiled into mruby
  -> require '#{loader_file}'
TEXT

result = require(loader_file)

puts <<TEXT
  => #{result}

Require a library compiled into mruby, with .rb extension (extension should not matter)
  -> require '#{loader_file}.rb'
TEXT

result = require("#{loader_file}.rb")

puts <<TEXT
  => #{result}

Require a library not compiled into mruby (should raise a LoadError)
  -> require 'file/not/found'
TEXT

begin
  require('file/not/found')
rescue LoadError => load_error
end

puts <<TEXT
  #{load_error.backtrace[0]}: #{load_error.to_s} (#{load_error.class})

TEXT

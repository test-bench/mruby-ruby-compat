file = File.join('test', 'examples', 'some_file.rb')

puts <<TEXT

Load a file
  -> load '#{file}'
TEXT

result = load(file)

puts <<TEXT
  => #{result}

Load a file again (should load the second time)
  -> load '#{file}'
TEXT

result = load(file)

puts <<TEXT
  => #{result}

Load a file that is not found (should raise a LoadError)
  -> load 'file/not/found.rb'
TEXT

begin
  load('file/not/found.rb')
rescue LoadError => load_error
end

puts <<TEXT
  #{load_error.backtrace[0]}: #{load_error.to_s} (#{load_error.class})

TEXT

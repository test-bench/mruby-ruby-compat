begin
  raise "Some error"
rescue RuntimeError => error
end

puts <<TEXT
Error cause (should be nil): #{error.cause.inspect}

TEXT

puts <<TEXT

SystemExit without arguments
  -> raise SystemExit
TEXT

begin
  raise SystemExit
rescue SystemExit => system_exit
end

puts <<TEXT
  SystemExit (Message should be 'SystemExit', status should be 0)
    Message: #{system_exit.message}
    Status: #{system_exit.status}

SystemExit with one argument (status)
  -> raise SystemExit.new(1)
TEXT

begin
  raise SystemExit.new(1)
rescue SystemExit => system_exit
end

puts <<TEXT
  SystemExit (Message should be 'SystemExit', status should be 1)
    Message: #{system_exit.message}
    Status: #{system_exit.status}

SystemExit with two arguments (status and message)
  -> raise SystemExit.new(2, 'Some message')
TEXT

begin
  raise SystemExit.new(2, 'Some message')
rescue SystemExit => system_exit
end

puts <<TEXT
  SystemExit (Message should be 'Some message', status should be 2)
    Message: #{system_exit.message}
    Status: #{system_exit.status}

SystemExit with second argument to raise (status)
  -> raise SystemExit, 3
TEXT

begin
  raise SystemExit, 3
rescue SystemExit => system_exit
end

puts <<TEXT
  SystemExit (Message should be 'SystemExit', status should be 3)
    Message: #{system_exit.message}
    Status: #{system_exit.status}

TEXT

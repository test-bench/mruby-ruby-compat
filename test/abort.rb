puts <<TEXT

Abort without message (should print nothing)
  -> abort
TEXT

begin
  abort
rescue SystemExit => system_exit
end

puts <<TEXT
  SystemExit (Message should be 'SystemExit', status should be 1)
    Message: #{system_exit.message}
    Status: #{system_exit.status}

Abort with message (should print the message)
  -> abort "Some abort message"
TEXT

begin
  abort "Some abort message"
rescue SystemExit => system_exit
end

puts <<TEXT
  SystemExit (Message should be 'Some abort message', status should be 1)
    Message: #{system_exit.message}
    Status: #{system_exit.status}

TEXT

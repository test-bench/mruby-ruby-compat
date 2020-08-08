begin
  raise SystemExit.new
rescue SystemExit => system_exit
  puts "Exit status: #{system_exit.status}, Message: #{system_exit.message || '(none)'}"
end

begin
  raise SystemExit.new(1)
rescue SystemExit => system_exit
  puts "Exit status: #{system_exit.status}, Message: #{system_exit.message || '(none)'}"
end

begin
  raise SystemExit.new(11, "Some message")
rescue SystemExit => system_exit
  puts "Exit status: #{system_exit.status}, Message: #{system_exit.message || '(none)'}"
end

begin
  begin
    fail "Inner error"
  rescue
    abort
  end

  STDERR.puts "Should not reach here"

rescue SystemExit => system_exit
  puts "SystemExit (Status: #{system_exit.status}, Message: #{system_exit.message || '(none)'})"
end

puts

begin
  abort(nil)
  STDERR.puts "Should not reach here"

rescue SystemExit => system_exit
  puts "SystemExit (Status: #{system_exit.status}, Message: #{system_exit.message || '(none)'})"

rescue TypeError => type_error
  puts "TypeError (Message: #{type_error.to_s})"
end

puts

begin
  begin
    fail "Inner error"
  rescue => error
    abort(error.message)
  end

  STDERR.puts "Should not reach here"

rescue SystemExit => system_exit
  puts "SystemExit (Status: #{system_exit.status}, Message: #{system_exit.message || '(none)'})"
end

puts

assert('abort') do
  assert_raise(SystemExit) do
    abort
  end

  assert_raise(SystemExit) do
    abort "Some message"
  end
end

assert('raise SystemExit') do
  assert_raise(SystemExit) do
    raise SystemExit.new
  end

  assert_raise(SystemExit) do
    raise SystemExit.new(1)
  end

  assert_raise(SystemExit) do
    raise SystemExit.new(11, 'Some message')
  end
end

assert('SystemExit (no arguments)') do
  system_exit = SystemExit.new

  assert(system_exit.status == 1)
  assert(system_exit.message == 'SystemExit')
end

assert('SystemExit (one argument)') do
  system_exit = SystemExit.new(11)

  assert(system_exit.status == 11)
  assert(system_exit.message == 'SystemExit')
end

assert('SystemExit (two arguments)') do
  system_exit = SystemExit.new(11, 'Some message')

  assert(system_exit.status == 11)
  assert(system_exit.message == 'Some message')
end

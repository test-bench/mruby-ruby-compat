assert('Exception cause') do
  exception = RuntimeError.new

  assert_true(exception.cause.nil?)
end

assert('Exception cause') do
  exception = RuntimeError.new

  assert(exception.cause.nil?)
end

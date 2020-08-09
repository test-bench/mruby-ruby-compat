assert('TOPLEVEL_BINDING.receiver') do
  assert_true(TOPLEVEL_BINDING.receiver.instance_of?(Object))
end

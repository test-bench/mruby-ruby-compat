assert('Public send') do
  obj = :some_obj

  assert(obj.public_send(:to_s) == 'some_obj')
end

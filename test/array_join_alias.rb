assert('Array join alias (*)') do
  result = ['foo', 'bar', 'baz'] * ', '

  assert(result == 'foo, bar, baz')
end

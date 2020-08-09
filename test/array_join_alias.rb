assert('Array join alias (*)') do
  result = ['foo', 'bar', 'baz'] * ', '

  assert_equal(result, 'foo, bar, baz')
end

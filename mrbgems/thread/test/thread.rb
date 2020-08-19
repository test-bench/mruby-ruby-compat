##
# Thread test

assert('Object.const_defined? :Thread') do
  assert_true(Object.const_defined?(:Thread))
end

assert('Thread returns Fixnum') do
  a = Thread.new{100}
  assert_equal(100, a.join)
end

assert('Thread returns true') do
  a = Thread.new{true}
  assert_equal(true, a.join)
end

assert('Thread returns false') do
  a = Thread.new{false}
  assert_equal(false, a.join)
end

assert('Thread returns Float') do
  a = Thread.new{99.99}
  assert_equal(99.99, a.join)
end

assert('Thread returns String') do
  a = Thread.new{"hello"}
  assert_equal("hello", a.join)
end

assert('Thread returns Symbol') do
  a = Thread.new{:context}
  assert_equal(:context, a.join)
end

assert('Thread returns Array') do
  a = Thread.new{[1,2,3]}
  assert_equal([1,2,3], a.join)
end

assert('Thread returns Hash') do
  a = Thread.new{{'abc_key' => 'abc_value', 'cba_key' => 'cba_value'}}
  assert_equal({'abc_key' => 'abc_value', 'cba_key' => 'cba_value'}, a.join)
end

assert('Thread migrates Fixnum') do
  a = Thread.new(100){|a| a}
  assert_equal(100, a.join)
end

assert('Thread migrates ture') do
  a = Thread.new(true){|a| a}
  assert_equal(true, a.join)
end

assert('Thread migrates false') do
  a = Thread.new(false){|a| a}
  assert_equal(false, a.join)
end

assert('Thread migrates Float') do
  a = Thread.new(99.99){|a| a}
  assert_equal(99.99, a.join)
end

assert('Thread migrates String') do
  a = Thread.new("hello"){|a| a}
  assert_equal("hello", a.join)
end

assert('Thread migrates Symbol') do
  a = Thread.new(:context){|a| a}
  assert_equal(:context, a.join)
end

assert('Thread migrates Symbol in a complex context') do
  a = :cxt1
  t = Thread.new(:cxt2){|b| [a, b, :ctx3] }
  assert_equal([:cxt1, :cxt2, :ctx3], t.join)
end

assert('Thread migrates Array') do
  skip "skip because COPY_VALUES is disabled" unless Thread::COPY_VALUES
  a = Thread.new([1,2,3]){|a| a}
  assert_equal([1,2,3], a.join)
end

assert('Thread migrates Hash') do
  skip "skip because COPY_VALUES is disabled" unless Thread::COPY_VALUES
  a = Thread.new({'abc_key' => 'abc_value', 'cba_key' => 'cba_value'}){|a| a}
  assert_equal({'abc_key' => 'abc_value', 'cba_key' => 'cba_value'}, a.join)
end

assert('Thread migrates Proc') do
  pr = Proc.new { 1 }
  a = Thread.new(pr){|pr| pr.call }
  assert_equal(1, a.join)
end

assert('Thread raises an error') do
  thread = Thread.new do
    raise "Some error message"

    :null
  end

  assert_raise(RuntimeError) do
    thread.join
  end
end

assert('Thread migrates Object') do
  skip

  class DummyObj
    attr_accessor :foo, :bar, :buz
  end

  obj = DummyObj.new

  t = Thread.new(obj) do |a|
    a.foo = "foo"
    a.bar = 123
    a.buz = :buz
    a
    :foo
  end

  obj = t.join

  assert_equal(DummyObj, obj.class)
  assert_equal("foo", obj.foo)
  assert_equal(123, obj.bar)
  assert_equal(:buz, obj.buz)
end

assert('Fixed test of issue #36') do
  a = Thread.new do
    "".is_a?(String)
  end
  assert_true(a.join)
end

assert('Thread GC') do
  t = Thread.new { GC.start; :end }
  assert_equal(:end, t.join)
end

assert('Thread sleep') do
  assert_nil(Thread.sleep(1))
end

assert('Thread usleep') do
  begin
    assert_nil(Thread.usleep(1))
  rescue NotImplementedError => e
    skip e.message
  end
end

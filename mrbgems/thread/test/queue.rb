##
# Queue test

assert('Object.const_defined? :Queue') do
  assert_true(Object.const_defined?(:Queue))
end

assert('Queue#empty?') do
  q = Queue.new
  assert_equal(true, q.empty?)
end

assert('Queue#push and #shift') do
  q = Queue.new
  q.push 1
  q.push 2
  q.push 3
  assert_equal(1, q.shift)
  assert_equal(2, q.shift)
  assert_equal(3, q.shift)
end

assert('Queue#clear') do
  q = Queue.new
  q.push 42

  q.clear

  assert_true(q.empty?)
end

assert('Queue#push and #pop') do
  q = Queue.new
  q.push 1
  q.push 2
  q.push 3
  assert_equal(3, q.pop)
  assert_equal(2, q.pop)
  assert_equal(1, q.pop)
end

assert('Queue#enq and #deq') do
  q = Queue.new
  q.enq 1
  q.enq 2
  q.enq 3
  assert_equal(1, q.deq)
  assert_equal(2, q.deq)
  assert_equal(3, q.deq)
end

assert('Queue#size') do
  q = Queue.new
  assert_equal(0, q.size)
  q.enq 42
  assert_equal(1, q.size)
  q.deq
  assert_equal(0, q.size)
end

assert('Queue#push alloced') do
  q = Queue.new
  t = Thread.new do
    q.push ['test1'.dup]
    'test2'
  end

  assert_equal ['test1'], q.pop
  assert_equal 'test2', t.join
end

require_relative './automated_init'

context "MRuby" do
  context "TestBench" do
    test_bench_defined = Object.const_defined?(:TestBench)

    test "Constant is defined" do
      assert(test_bench_defined)
    end
  end
end

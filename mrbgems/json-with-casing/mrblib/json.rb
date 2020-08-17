class Object
  def to_json
    JSON.generate(self)
  end
end

module JSON
  JSONError = Class.new(StandardError)
  ParserError = Class.new(JSONError)
end

# Before run this test case, You should run following commands.
# $ dropdb mrb_pg_test
# $ createdb -e mrb_pg_test

assert('Set type map') do
  @conn = PG::Connection.new('dbname=mrb_pg_test')

  type_map_for_results = PG::BasicTypeMapForResults.new(@conn)
  @conn.type_map_for_results = type_map_for_results
  assert_equal(@conn.type_map_for_results, type_map_for_results)

  type_map_for_queries = PG::BasicTypeMapForQueries.new(@conn)
  @conn.type_map_for_queries = type_map_for_queries
  assert_equal(@conn.type_map_for_queries, type_map_for_queries)

  @conn.exec(<<SQL)
DROP TABLE IF EXISTS some_table;

DROP TYPE IF EXISTS some_enum_type;
CREATE TYPE some_enum_type AS ENUM (
  'some-enum-value',
  'other-enum-value'
);

CREATE TABLE some_table (
  id uuid PRIMARY KEY,

  -- Numbers
  some_smallint smallint,
  some_integer integer,
  some_bigint bigint,
  some_decimal_3_1 decimal(4, 1),
  some_numeric_3_1 numeric(4, 1),
  some_real real,
  some_double_precision double precision,
  some_smallserial smallserial,
  some_serial serial,
  some_bigserial bigserial,

  -- Character
  some_varchar_3 character varying(3),
  some_char_3 character(3),
  some_text text,

  -- Binary
  some_bytea bytea,

  -- Date/Time
  some_timestamp_3 timestamp(3) without time zone,
  some_timestamp_3_with_tz timestamp(3) with time zone,
  some_date date,
  some_time_3 time(3) without time zone,
  some_time_3_with_tz time(3) with time zone,

  -- Boolean
  some_boolean bool,

  -- Enum
  some_enum some_enum_type,

  -- UUID
  some_uuid uuid,

  -- JSON
  some_json_text json,
  some_json_binary jsonb
);
SQL

  GC.start
end

def put_data(column_name, value, typename=nil)
  id = SecureRandom.uuid

  if typename.nil?
    second_value = '$2'
  else
    second_value = "$2::#{typename}"
  end

  @conn.exec_params("INSERT INTO some_table (id, #{column_name}) VALUES ($1, #{second_value})", [id, value])

  id
end

def get_column(column_name, id)
  @conn.exec_params("SELECT #{column_name} FROM some_table WHERE id = $1", [id])
end

def put_and_get_column(column_name, input_value, typename=nil, &block)
  id = put_data(column_name, input_value, typename)

  result = get_column(column_name, id)

  assert_equal(1, result.length)

  output_value = result[0][column_name.to_s]

  block.(output_value)
end

def test_column(column_name, input_value, control: nil, typename: nil, &block)
  control = input_value if control.nil?

  put_and_get_column(column_name, input_value, typename) do |output_value|
    assert_equal(control, output_value)
  end
end

def test_column_float(column_name, input_value, control: nil, typename: nil)
  control = input_value if control.nil?

  put_and_get_column(column_name, input_value, typename) do |output_value|
    delta = (control - output_value).abs

    if delta > 0
      delta -= Float::EPSILON
    end

    if delta == 0
      assert_equal(1, 1)
    else
      assert_equal(control, output_value)
    end
  end
end

[
  ['smallint', :some_smallint],
  ['smallserial', :some_smallserial]
].each do |(type, column_name)|
  assert(type) do
    test_column(column_name, 1)
    test_column(column_name, -32768)
    test_column(column_name, 32767)

    assert_raise(PG::NumericValueOutOfRange) do
      get_column(column_name, put_data(column_name, -32769))
    end

    assert_raise(PG::NumericValueOutOfRange) do
      get_column(column_name, put_data(column_name, 32768))
    end
  end
end

[
  ['integer', :some_integer],
  ['serial', :some_serial]
].each do |(type, column_name)|
  assert(type) do
    test_column(column_name, 1)
    test_column(column_name, -2147483648)
    test_column(column_name, 2147483647)

    assert_raise(PG::NumericValueOutOfRange) do
      get_column(column_name, put_data(column_name, -2147483649))
    end

    assert_raise(PG::NumericValueOutOfRange) do
      get_column(column_name, put_data(column_name, 2147483648))
    end
  end
end

[
  ['bigint', :some_bigint],
  ['bigserial', :some_bigserial]
].each do |(type, column_name)|
  assert(type) do
    test_column(column_name, 1)
    test_column(column_name, -9223372036854775808)
    test_column(column_name, 9223372036854775807)

    assert_raise(PG::NumericValueOutOfRange) do
      get_column(column_name, put_data(column_name, -9223372036854775809))
    end

    assert_raise(PG::NumericValueOutOfRange) do
      get_column(column_name, put_data(column_name, 9223372036854775808))
    end
  end
end

assert('decimal') do
  test_column(:some_decimal_3_1, 111)
  test_column(:some_decimal_3_1, 111.1)
  test_column(:some_decimal_3_1, 111.11, control: 111.1)
  test_column(:some_decimal_3_1, 111.19, control: 111.2)

  assert_raise(PG::NumericValueOutOfRange) do
    get_column(:some_decimal_3_1, put_data(:some_decimal_3_1, 1111.1))
  end
end

assert('numeric') do
  test_column(:some_numeric_3_1, 111)
  test_column(:some_numeric_3_1, 111.1)
  test_column(:some_numeric_3_1, 111.11, control: 111.1)
  test_column(:some_numeric_3_1, 111.19, control: 111.2)

  assert_raise(PG::NumericValueOutOfRange) do
    get_column(:some_numeric_3_1, put_data(:some_numeric_3_1, 1111.1))
  end
end

assert('real') do
  # TODO: 1e-37 and 1e37 should be used, not 1e36, but 1e37 produces strange behavior
  test_column_float(:some_real, 1e-36)
  test_column_float(:some_real, 1e36)
  # TODO: rounding is incorrect
  test_column_float(:some_real, 1.11111111, control: 1.1111112)
  test_column_float(:some_real, 1.11111119, control: 1.1111112)
end

assert('double_precision') do
  test_column_float(:some_double_precision, 1e-307)
  test_column_float(:some_double_precision, 1e308)
  test_column_float(:some_double_precision, 1.1111111111111111, control: 1.111111111111111)
  test_column_float(:some_double_precision, 1.1111111111111119, control: 1.111111111111112)
end

assert('smallserial') do
  test_column(:some_smallserial, 1)
  test_column(:some_smallserial, -32768)
  test_column(:some_smallserial, 32767)

  assert_raise(PG::NumericValueOutOfRange) do
    get_column(:some_smallserial, put_data(:some_smallserial, -32769))
  end

  assert_raise(PG::NumericValueOutOfRange) do
    get_column(:some_smallserial, put_data(:some_smallserial, 32768))
  end
end

assert('varchar') do
  test_column(:some_varchar_3, '.')
  test_column(:some_varchar_3, '..')
  test_column(:some_varchar_3, '...')

  assert_raise(PG::StringDataRightTruncation) do
    test_column(:some_varchar_3, '....')
  end
end

assert('char') do
  test_column(:some_char_3, '.', control: '.  ')
  test_column(:some_char_3, '..', control: '.. ')
  test_column(:some_char_3, '...', control: '...')

  assert_raise(PG::StringDataRightTruncation) do
    test_column(:some_char_3, '....')
  end
end

assert('text') do
  test_column(:some_text, 'Textual data')
end

# TODO: needs conversion before being given to PG
=begin
assert('bytea') do
  test_column(:some_bytea, "\x03\x02\x01")
end
=end

assert('timestamp') do
  test_column(
    :some_timestamp_3,
    Time.local(2000, 1, 1, 11, 11, 11)
  )

  test_column(
    :some_timestamp_3,
    Time.utc(2000, 1, 1, 11, 11, 11),
    control: Time.local(2000, 1, 1, 11, 11, 11)
  )

# TODO: needs conversion before being given to PG
=begin
  test_column(
    :some_timestamp_3,
    Time.local(2000, 1, 1, 11, 11, 11, 111111),
    control: Time.local(2000, 1, 1, 11, 11, 11, 111000)
  )

  test_column(
    :some_timestamp_3,
    Time.utc(2000, 1, 1, 11, 11, 11, 111111),
    control: Time.local(2000, 1, 1, 11, 11, 11, 111000)
  )
=end
end

assert('timestamp_tz') do
  test_column(
    :some_timestamp_3_with_tz,
    Time.local(2000, 1, 1, 11, 11, 11)
  )

# TODO: needs conversion before being given to PG
=begin
  test_column(
    :some_timestamp_3_with_tz,
    Time.local(2000, 1, 1, 11, 11, 11, 111111),
    control: Time.local(2000, 1, 1, 11, 11, 11, 111000)
  )

  test_column(
    :some_timestamp_3_with_tz,
    Time.utc(2000, 1, 1, 11, 11, 11)
  )

  test_column(
    :some_timestamp_3_with_tz,
    Time.utc(2000, 1, 1, 11, 11, 11, 111111),
    control: Time.utc(2000, 1, 1, 11, 11, 11, 111000)
  )
=end
end

assert('date') do
  test_column(:some_date, Time.local(2000, 1, 1))
end

assert('time') do
  test_column(:some_time_3, Time.local(2000, 1, 1, 11, 11, 11), control: "11:11:11")

# TODO: needs conversion before being given to PG
=begin
  test_column(:some_time_3, Time.local(2000, 1, 1, 11, 11, 11, 111111), control: "11:11:11.111")
=end
end

assert('time_tz') do
  test_column(:some_time_3_with_tz, Time.local(2000, 1, 1, 11, 11, 11), control: "11:11:11-06")
  test_column(:some_time_3_with_tz, Time.utc(2000, 1, 1, 11, 11, 11), control: "11:11:11+00")

# TODO: needs conversion before being given to PG
=begin
  test_column(:some_time_3, Time.local(2000, 1, 1, 11, 11, 11, 111111), control: "11:11:11.111")
  test_column(:some_time_3, Time.utc(2000, 1, 1, 11, 11, 11, 111111), control: "11:11:11.111")
=end
end

assert('bool') do
  test_column(:some_boolean, true)
  test_column(:some_boolean, false)
end

assert('enum') do
  test_column(:some_enum, 'some-enum-value')
  test_column(:some_enum, 'other-enum-value')

  assert_raise(PG::InvalidTextRepresentation) do
    test_column(:some_enum, 'unknown-enum-value')
  end
end

assert('enum') do
  test_column(:some_uuid, '00000001-0000-4000-8000-000000000000')

  assert_raise(PG::InvalidTextRepresentation) do
    test_column(:some_uuid, 'not-a-uuid')
  end
end

assert('json') do
  raw_data = {
    'someString' => 'some-string',
    'someNumber' => 1,
    'someList' => ['a', 'b', 'c'],
    'someNestedObject' => {
      'someNestedAttribute' => 'some-nested-value'
    }
  }

  json_text = JSON.generate(raw_data)

  test_column(:some_json_text, json_text, control: raw_data)
# TODO: needs conversion before being given to PG
=begin
  test_column(:some_json_text, raw_data)
=end
end

assert('jsonb') do
  raw_data = {
    'someString' => 'some-string',
    'someNumber' => 1,
    'someList' => ['a', 'b', 'c'],
    'someNestedObject' => {
      'someNestedAttribute' => 'some-nested-value'
    }
  }

  json_text = JSON.generate(raw_data)

  test_column(:some_json_binary, json_text, control: raw_data)
# TODO: needs conversion before being given to PG
=begin
  test_column(:some_json_binary, raw_data)
=end
end

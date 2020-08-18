# Before run this test case, You should run following commands.
# $ dropdb mrb_pg_test
# $ createdb -e mrb_pg_test

assert('Set type map') do
  @conn = PG::Connection.new('dbname=mrb_pg_test')

  type_map_for_results = PG::BasicTypeMapForResults.new(@conn)
  @conn.type_map_for_results = type_map_for_results

  assert_equal(type_map_for_results, @conn.type_map_for_results)

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

  -- Money
  some_money money,

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
  some_interval_year interval YEAR,
  some_interval_second_3 interval SECOND(3),

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
end

def put_data(column_name, value)
  id = SecureRandom.uuid

  @conn.exec_params("INSERT INTO some_table (id, #{column_name}) VALUES ($1, $2)", [id, value])

  id
end

def get_column(column_name, id)
  @conn.exec_params("SELECT #{column_name} FROM some_table WHERE id = $1", [id])
end

def test_column(column_name, value)
  id = put_data(column_name, value)

  result = get_column(column_name, id)

  assert_equal(1, result.length)
  assert_equal(value, result[0][column_name.to_s])
end

assert('smallint') do
  test_column(:some_smallint, 1)
  test_column(:some_smallint, -32768)
  test_column(:some_smallint, 32767)

  assert_raise(PG::NumericValueOutOfRange) do
    get_column(:some_smallint, put_data(:some_smallint, -32769))
  end

  assert_raise(PG::NumericValueOutOfRange) do
    get_column(:some_smallint, put_data(:some_smallint, 32768))
  end
end

assert('integer') do
  test_column(:some_integer, 1)
  test_column(:some_integer, -2147483648)
  test_column(:some_integer, 2147483647)

  assert_raise(PG::NumericValueOutOfRange) do
    get_column(:some_integer, put_data(:some_integer, -2147483649))
  end

  assert_raise(PG::NumericValueOutOfRange) do
    get_column(:some_integer, put_data(:some_integer, 2147483648))
  end
end

assert('bigint') do
  test_column(:some_bigint, 1)
  test_column(:some_bigint, -9223372036854775808)
  test_column(:some_bigint, 9223372036854775807)

  assert_raise(PG::NumericValueOutOfRange) do
    get_column(:some_bigint, put_data(:some_bigint, -9223372036854775809))
  end

  assert_raise(PG::NumericValueOutOfRange) do
    get_column(:some_bigint, put_data(:some_bigint, 9223372036854775808))
  end
end

=begin
assert('decimal') do
  test_column(:some_decimal_3_1, 1)
  test_column(:some_decimal_3_1, 1.1)
end

assert('numeric') do
  test_column(:some_numeric_3_1, 1)
  test_column(:some_numeric_3_1, 1.1)
end

assert('real') do
  test_column(:some_real, 1)
  test_column(:some_real, 1.1)
end

assert('double_precision') do
  test_column(:some_double_precision, 1)
  test_column(:some_double_precision, 1.1)
end

assert('smallserial') do
  test_column(:some_smallserial, 1)
end

assert('serial') do
  test_column(:some_serial, 11)
end

assert('bigserial') do
  test_column(:some_bigserial, 111)
end

assert('UUID') do
  test_column(:some_uuid, SecureRandom.uuid)
end
=end

assert("utc_offset") do
  time = Time.local(2000, 1, 1, 11, 11, 11, 111111)

  utc_offset = time.utc_offset

  assert_kind_of(Integer, utc_offset)

  # Calculate local time offset (e.g. -06:00 if local time is CST) for subsequent tests
  utc_offset_minutes, _ = utc_offset.divmod(60)
  utc_offset_hours, utc_offset_minutes = utc_offset_minutes.divmod(60)

  utc_offset_sign = utc_offset_hours >= 0 ? '+' : '-'

  utc_offset_hours = utc_offset_hours.abs.to_s.rjust(2, '0')
  utc_offset_minutes = utc_offset_minutes.to_s.rjust(2, '0')

  @utc_offset_iso8601 = "#{utc_offset_sign}#{utc_offset_hours}:#{utc_offset_minutes}"
end

assert("ISO8601 generation (local)") do
  time = Time.local(2000, 1, 1, 11, 11, 11, 111111)

  assert_equal("2000-01-01T11:11:11#{@utc_offset_iso8601}", time.iso8601)
  assert_equal("2000-01-01T11:11:11.111111#{@utc_offset_iso8601}", time.iso8601(6))
end

assert("ISO8601 generation (UTC)") do
  time = Time.utc(2000, 1, 1, 11, 11, 11, 111111)

  assert_equal("2000-01-01T11:11:11Z", time.iso8601)
  assert_equal("2000-01-01T11:11:11.1111110Z", time.iso8601(7))
end

assert("ISO8601 generation (precision does not round)") do
  time = Time.utc(2000, 1, 1, 11, 11, 11, 190000)

  assert_equal("2000-01-01T11:11:11.1Z", time.iso8601(1))
end

assert("ISO8601 generation (zero padding when sub-microsecond precision requested)") do
  time = Time.utc(2000, 1, 1, 11, 11, 11, 111111)

  assert_equal("2000-01-01T11:11:11.111111000Z", time.iso8601(9))
end

assert("Parse ISO8601 timestamp (local)") do
  parsed_time = Time.parse("2000-01-01T11:11:11.111111#{@utc_offset_iso8601}")

  assert_equal(Time.local(2000, 1, 1, 11, 11, 11, 111111), parsed_time)
end

assert("Parse ISO8601 timestamp (UTC)") do
  parsed_time = Time.parse("2000-01-01T11:11:11.111111Z")

  assert_equal(Time.utc(2000, 1, 1, 11, 11, 11, 111111), parsed_time)
end

assert("Parse ISO8601 timestamp (precision)") do
  parsed_time = Time.parse("2000-01-01T11:11:11.111Z")

  assert_equal(Time.utc(2000, 1, 1, 11, 11, 11, 111000), parsed_time)
end

assert("Parse ISO8601 timestamp (no subsecond precision)") do
  parsed_time = Time.parse("2000-01-01T11:11:11Z")

  assert_equal(Time.utc(2000, 1, 1, 11, 11, 11, 0), parsed_time)
end

assert("Parse ISO8601 timestamp that is not UTC or local (not supported on MRuby)") do
  utc_offset = @utc_offset_iso8601.dup

  if utc_offset[0] == '+'
    utc_offset[0] = '-'
  else
    utc_offset[0] = '+'
  end

  assert_raise(ArgumentError) do
    Time.parse("2000-01-01T11:11:11#{utc_offset}")
  end
end

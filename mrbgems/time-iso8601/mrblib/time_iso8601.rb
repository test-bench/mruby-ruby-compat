class Time
  SECOND_PATTERN = %r{(?<second>[[:digit:]]{1,2})(?:\.(?<microsecond>[[:digit:]]+))?}

  HOUR_MINUTE_SECOND_PATTERN = %r{(?<hour>[[:digit:]]{1,2}):(?<minute>[[:digit:]]{1,2}):#{SECOND_PATTERN}}

  UTC_OFFSET_PATTERN = %r{(?<utc_offset>Z|(?<utc_offset_hours>-?[[:digit:]]{1,2}):(?<utc_offset_minutes>[[:digit:]]{1,2}))}

  ISO8601_UTC_PATTERN = %r{
    (?<year>[[:digit:]]{4})
    -
    (?<month>[[:digit:]]{1,2})
    -
    (?<day>[[:digit:]]{1,2})
    [T ]
    #{HOUR_MINUTE_SECOND_PATTERN}
    (?:#{UTC_OFFSET_PATTERN})?
  }x

  def self.parse(timestamp)
    match_data = ISO8601_UTC_PATTERN.match(timestamp)

    if match_data.nil?
      raise ArgumentError, "no time information in #{timestamp.inspect}"
    end

    year = match_data[:year].to_i
    month = match_data[:month].to_i
    day = match_data[:day].to_i
    hour = match_data[:hour].to_i
    minute = match_data[:minute].to_i
    second = match_data[:second].to_i
    microsecond = (match_data[:microsecond].ljust(6, '0')).to_i

    utc_offset = match_data[:utc_offset]

    if utc_offset.nil? || utc_offset == 'Z'
      utc(year, month, day, hour, minute, second, microsecond)
    else
      time = local(year, month, day, hour, minute, second, microsecond)

      offset_hours = match_data[:utc_offset_hours].to_i
      offset_minutes = match_data[:utc_offset_minutes].to_i

      offset_seconds = (offset_hours * 60*60) + (offset_minutes * 60)

      unless offset_seconds == time.utc_offset
        raise ArgumentError, "Only local and UTC timezones are supported (Timestamp: #{timestamp.inspect})"
      end

      time
    end
  end

  def iso8601(precision=nil)
    precision ||= 3

    sec_fraction = usec.to_s

    if sec_fraction.length > precision
      final_digit = sec_fraction[precision - 1].to_i
      round_digit = sec_fraction[precision].to_i

      if round_digit >= 5
        final_digit += 1
      end

      sec_fraction = sec_fraction.slice!(0, precision - 1)
      sec_fraction << final_digit.to_s
    else
      sec_fraction = sec_fraction.ljust(precision, '0')
    end

    if gmt?
      tz = 'Z'
    else
      offset = gmt_offset

      minutes, _ = offset.divmod(60)
      hours, minutes = minutes.divmod(60)

      tz = String.new

      if hours < 0
        tz << '-'
        hours = 0 - hours
      end

      tz << hours.to_s.rjust(2, '0')
      tz << ':'
      tz << minutes.to_s.rjust(2, '0')
    end

    strftime("%Y-%m-%dT%H:%M:%S.#{sec_fraction}#{tz}")
  end
end

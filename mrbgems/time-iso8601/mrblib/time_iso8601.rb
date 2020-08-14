class Time
  def iso8601(precision=nil)
    precision ||= 3

    unless zone == 'UTC'
      return getutc.iso8601(precision)
    end

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

    strftime("%Y-%m-%dT%H:%M:%S.#{sec_fraction}Z")
  end
end

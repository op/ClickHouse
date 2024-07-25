SELECT lagInFrame(2::UInt128, 2, number) OVER w FROM numbers(10) WINDOW w AS (ORDER BY number); -- { serverError BAD_ARGUMENTS }
SELECT leadInFrame(2::UInt128, 2, number) OVER w FROM numbers(10) WINDOW w AS (ORDER BY number); -- { serverError BAD_ARGUMENTS }
SELECT lagInFrame(2::UInt64, 2, number) OVER w FROM numbers(10) WINDOW w AS (ORDER BY number);
SELECT leadInFrame(2::UInt64, 2, number) OVER w FROM numbers(10) WINDOW w AS (ORDER BY number);

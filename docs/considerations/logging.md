Test suites/cases should be careful not to write arbitrary data to
<varname>stdout</varname>, as this is the mechanism by which GTU communicates
with the test harness. Whilst writing to <varname>stderr</varname> should be
safe (as per the [TAP specification](http://testanything.org/tap-specification.html)),
it's recommended that this is avoided for the sake of test log legibility and
(potentially) compatibility with buggy test harnesses.

Test cases wishing to log additional information should do so via g_message() or
g_log(); GTU installs default log handlers to emit messages to the test log
correctly. Test suites should avoid installing their own default log handlers.

Test case writers are encouraged (but not required) to consider whether some
logged information is essential to the test log. Most test case output can
likely be hidden behind a check for %GTU_TEST_MODE_FLAGS_VERBOSE (done
automatically for messages logged with g_info() and g_debug()). No such
consideration is required for %GTU_TEST_MODE_FLAGS_QUIET, as this is handled
automatically by GTU.

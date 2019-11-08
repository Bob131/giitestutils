GTU will take one of four different actions when a message is logged via GLib's
logging system:
 - Log: messages are emitted to the test log (depending on their log level and
   %GTU_TEST_MODE_FLAGS_VERBOSE)
 - Suppress: messages are ignored, unless %GTU_TEST_MODE_FLAGS_VERBOSE is set in
   which case a suppression message is logged
 - Fail: the currently executing test is stopped and marked as failed, just as
   if it failed an assert
 - Trap: `SIGABRT` is raised and the test program dumps core

GTU's log message handling policy is as follows:
 - Outside of tests:
   - %G_LOG_LEVEL_ERROR always traps
   - %G_LOG_FLAG_FATAL always traps
   - %G_LOG_LEVEL_CRITICAL and %G_LOG_LEVEL_WARNING are always treated as fatal
     (thus also traps)
   - all other messages are logged
 - When a running test does not expect a message (i.e., the message does not
   match any previous call to gtu_test_case_expect_message()):
   - %G_LOG_LEVEL_ERROR always traps
   - %G_LOG_FLAG_FATAL always traps
   - %G_LOG_LEVEL_CRITICAL and %G_LOG_LEVEL_WARNING from a GTU logging domain
     are treated as fatal and trigger a trap
   - %G_LOG_LEVEL_CRITICAL and %G_LOG_LEVEL_WARNING cause a test to fail
   - all other messages are logged
 - When a running test does expect a message:
   - %G_LOG_LEVEL_ERROR always traps
   - %G_LOG_LEVEL_CRITICAL and %G_LOG_LEVEL_WARNING from a GTU logging domain
     trigger a trap
   - all other messages are suppressed

The rationale:
 - GLib always considers %G_LOG_LEVEL_ERROR fatal and we inherit this behaviour.
 - Warnings and criticals outside of test cases or originating from a GTU
   logging domain likely indicate a problem with GTU or the test program, so
   testing should be aborted and a core dump produced to aid in debugging.
 - A running test case producing an unexpected critical or warning likely
   indicates that the test case is exercising some undefined/buggy behaviour in
   the functionality under test; in such a case, said functionality cannot be
   said to adequately pass a test. To aid in debugging such test failures
   %G_DEBUG may be used to elevate warnings and criticals to fatal, dumping core
   when logged.
 - A running test case may purposely trigger criticals and warnings or otherwise
   expect such messages as part of a successful execution; trapping on such a
   message would be inappropriate. %G_LOG_FLAG_FATAL is necessarily ignored on
   these messages, as otherwise altering %G_DEBUG may prevent test cases from
   suppressing newly-fatal messages.

Test suites/cases should be careful not to write arbitrary data to
<varname>stdout</varname>, as this is the mechanism by which GTU communicates
with the test harness. Whilst writing to <varname>stderr</varname> should be
safe (as per the [TAP specification](https://testanything.org/tap-specification.html)),
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

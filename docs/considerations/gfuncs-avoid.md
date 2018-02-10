<table>
  <title>GLib functions to avoid</title>

  <tgroup cols="2" colsep="1" rowsep="1">
    <colspec align="center"/>

    <thead>
      <row>
        <entry>GLib symbol</entry>
        <entry>GTU replacement</entry>
      </row>
    </thead>

    <tbody>

      <row>
        <entry>
          <para>g_test_minimized_result()</para>
          <para>g_test_maximized_result()</para>
        </entry>
        <entry><emphasis role="bold">NONE</emphasis></entry>
      </row>

      <row>
        <entry>g_test_init()</entry>
        <entry>gtu_init()</entry>
      </row>

      <row>
        <entry>g_test_initialized()</entry>
        <entry>gtu_has_initialized()</entry>
      </row>

      <row>
        <entry>
          <para>g_test_quick()</para>
          <para>g_test_slow()</para>
          <para>g_test_thorough()</para>
          <para>g_test_perf()</para>
          <para>g_test_verbose()</para>
          <para>g_test_undefined()</para>
          <para>g_test_quiet()</para>
        </entry>
        <entry>gtu_test_mode_flags_get_flags()</entry>
      </row>

      <row>
        <entry>g_test_subprocess()</entry>
        <entry><emphasis role="bold">NONE</emphasis></entry>
      </row>

      <row>
        <entry>
          <para>g_test_run()</para>
          <para>g_test_run_suite()</para>
        </entry>
        <entry>gtu_test_suite_run()</entry>
      </row>

      <row>
        <entry>
          <para>g_test_add_func()</para>
          <para>g_test_add_data_func()</para>
          <para>g_test_add_data_func_full()</para>
          <para>g_test_add()</para>
        </entry>
        <entry>
          gtu_test_case_new(), gtu_test_suite_add()
        </entry>
      </row>

      <row>
        <entry>
          <para>g_test_fail()</para>
          <para>g_test_skip()</para>
        </entry>
        <entry>
          <xref linkend="gtu-Asserts" endterm="gtu-Asserts.top_of_page"/>,
          <xref linkend="gtu-Skips" endterm="gtu-Skips.top_of_page"/>
        </entry>
      </row>

      <row>
        <entry>g_test_incomplete()</entry>
        <entry><emphasis role="bold">NONE</emphasis></entry>
      </row>

      <row>
        <entry>g_test_failed()</entry>
        <entry>None: aborting a test automatically backs out of its execution.</entry>
      </row>

      <row>
        <entry>g_test_message()</entry>
        <entry>None: g_message() does the right thing after gtu_init().</entry>
      </row>

      <row>
        <entry>
          <para>g_test_bug_base()</para>
          <para>g_test_bug()</para>
        </entry>
        <entry><emphasis role="bold">NONE</emphasis></entry>
      </row>

      <row>
        <entry>g_test_log_set_fatal_handler()</entry>
        <entry>None: see gtu_test_case_expect_message().</entry>
      </row>

      <row>
        <entry>
          <para>g_test_timer_start()</para>
          <para>g_test_timer_elapsed()</para>
          <para>g_test_timer_last()</para>
        </entry>
        <entry><emphasis role="bold">NONE</emphasis></entry>
      </row>

      <row>
        <entry>
          <para>g_test_queue_free()</para>
          <para>g_test_queue_destroy()</para>
          <para>g_test_queue_unref()</para>
        </entry>
        <entry>
          None: data required by test cases should be tied to the lifetime of
          its #GtuTestCase.
        </entry>
      </row>

      <row>
        <entry>
          <para>g_test_expect_message()</para>
          <para>g_test_assert_expected_messages()</para>
        </entry>
        <entry>
          <para>gtu_test_case_expect_message()</para>
          <para>gtu_test_case_expect_check()</para>
        </entry>
      </row>

      <row>
        <entry>
          <para>g_test_trap_subprocess()</para>
          <para>g_test_trap_has_passed()</para>
          <para>g_test_trap_reached_timeout()</para>
          <para>g_test_trap_assert_passed()</para>
          <para>g_test_trap_assert_failed()</para>
          <para>g_test_trap_assert_stdout()</para>
          <para>g_test_trap_assert_stdout_unmatched()</para>
          <para>g_test_trap_assert_stderr()</para>
          <para>g_test_trap_assert_stderr_unmatched()</para>
        </entry>
        <entry><emphasis role="bold">NONE</emphasis></entry>
      </row>

      <row>
        <entry>
          <para>g_assert_cmpstr()</para>
          <para>g_assert_cmpint()</para>
          <para>g_assert_cmpuint()</para>
          <para>g_assert_cmphex()</para>
          <para>g_assert_cmpfloat()</para>
          <para>g_assert_cmpmem()</para>
          <para>g_assert_no_error()</para>
          <para>g_assert_error()</para>
          <para>g_assert_true()</para>
          <para>g_assert_false()</para>
          <para>g_assert_null()</para>
          <para>g_assert_nonnull()</para>
        </entry>
        <entry>
          <para>
            <xref linkend="gtu-Asserts" endterm="gtu-Asserts.top_of_page"/>.
            Many of these assert variants don't have equivalents in GTU; use
            gtu_assert() instead.
          </para>
          <para>
            Also note that gtu_assert() is not directly equivalent to
            g_assert(). gtu_assert() is specifically for use within test cases
            and is non-fatal by default, whereas g_assert() is always fatal and
            not appropriate for use in test cases.
          </para>
        </entry>
      </row>

      <row>
        <entry>g_test_set_nonfatal_assertions()</entry>
        <entry>
          None: GTU assertions are only fatal if %GTU_DEBUG has been set.
        </entry>
      </row>

      <row>
        <entry>g_test_create_case()</entry>
        <entry>gtu_test_case_new()</entry>
      </row>

      <row>
        <entry>g_test_create_suite()</entry>
        <entry>gtu_test_suite_new()</entry>
      </row>

      <row>
        <entry>
          <para>g_test_suite_add()</para>
          <para>g_test_suite_add_suite()</para>
        </entry>
        <entry>gtu_test_suite_add()</entry>
      </row>

    </tbody>
  </tgroup>
</table>

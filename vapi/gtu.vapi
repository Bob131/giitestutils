[NoReturn]
[CCode (cheader_filename = "gtu.h", cname = "gtu_assert_not_reached")]
public void assert_not_reached ();

[CCode (cheader_filename = "gtu.h", cname = "gtu_assert")]
public void assert (bool expr);

[CCode (cheader_filename = "gtu.h", cname = "gtu_assert")]
public void assert_true (bool expr);

[CCode (cheader_filename = "gtu.h", cname = "gtu_assert")]
public void assert_false (bool expr);

[CCode (cheader_filename = "gtu.h", cname = "gtu_assert")]
public void assert_null (...);

[CCode (cheader_filename = "gtu.h", cname = "gtu_assert")]
public void assert_nonnull (...);

[CCode (cheader_filename = "gtu.h")]
namespace Gtu {
    [Flags]
    [CCode (has_type_id = false)]
    public enum TestModeFlags {
        PERF,
        SLOW,
        QUICK,
        UNDEFINED,
        VERBOSE,
        QUIET,
        THOROUGH;

        public static TestModeFlags get_flags ();
    }

    [NoReturn]
    public void skip_if_reached (string? message = null);
    public void skip_if_fail (bool condition, string? message = null);
    public void skip_if_not_flags (TestModeFlags flags,
                                   string? message = null);
    public void skip_if_not_thorough ();
    public void skip_if_not_perf ();
    public void skip_if_not_undefined ();

    [CCode (ref_sink_function = "gtu_test_object_ref")]
    public abstract class TestObject {
        private TestObject ();
    }

    public class TestCase : TestObject {
        public delegate void Func ();

        public TestCase (string name, owned Func func);
    }

    public class TestSuite : TestObject {
        public void add (TestObject test_object);
        public int run ();

        public TestSuite (string name);
    }

    public void init (string[] args);
    public bool has_initialized ();
}

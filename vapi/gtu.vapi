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

    [Compact]
    public class Path {
        public bool is_valid {get;}
        public unowned string to_string ();

        public void prepend_element (string element);
        public void append_element (string element);

        public void prepend_path (Path path);
        public void append_path (Path path);

        public bool has_prefix (Path prefix);

        public Path copy ();

        public Path ();

        public static Path? new_parse (string path);
    }

    [CCode (ref_sink_function = "gtu_test_object_sink")]
    public abstract class TestObject {
        public string name {get;}
        public Path path {get;}
        public TestSuite? parent_suite {get;}

        public signal void ancestry_changed ();

        private TestObject ();
    }

    [SimpleType]
    public struct ExpectHandle {}

    public class TestCase : TestObject {
        public delegate void Func ();

        protected virtual void test_impl ();

        public void add_dependency (TestSuite.Child test_object);
        public unowned TestCase with_dep (TestSuite.Child test_object);

        public ExpectHandle expect_message (string domain,
                                            GLib.LogLevelFlags level,
                                            owned GLib.Regex regex);
        public bool expect_check (ExpectHandle handle);

        [CCode (has_construct_function = false)]
        public TestCase (string name, owned Func func);

        [CCode (has_new_function = false, construct_function = "gtu_test_case_construct")]
        protected TestCase.@construct (string name);
    }

    public class TestSuite : TestObject {
        [Compact]
        public class Child {
            private Child (); // doesn't exist
        }

        public unowned Child add (TestObject test_object);
        [DestroysInstance]
        public int run ();

        public TestSuite (string name);
    }

    public void init (string[] args);
    public bool has_initialized ();
}

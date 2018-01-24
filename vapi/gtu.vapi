[NoReturn]
[CCode (cheader_filename = "gtu.h", cname = "gtu_assert_not_reached")]
public void assert_not_reached ();

[Assert]
[CCode (cheader_filename = "gtu.h", cname = "gtu_assert")]
public void assert (bool expr);

[Assert]
[CCode (cheader_filename = "gtu.h", cname = "gtu_assert")]
public void assert_true (bool expr);

[Assert]
[CCode (cheader_filename = "gtu.h", cname = "gtu_assert")]
public void assert_false (bool expr);

[Assert]
[CCode (cheader_filename = "gtu.h", cname = "gtu_assert")]
public void assert_null (...);

[Assert]
[CCode (cheader_filename = "gtu.h", cname = "gtu_assert")]
public void assert_nonnull (...);

[CCode (cheader_filename = "gtu.h")]
namespace Gtu {
    [NoReturn]
    public void skip_if_reached (string? message = null);
    public void skip_if (bool condition, string? message = null);
    public void skip_if_not_thorough ();
    public void skip_if_not_perf ();

    public delegate void TestFunc ();
    public GLib.TestCase create_test_case (string name, owned TestFunc func);

    public int run_suite (GLib.TestSuite suite);

    public void init (string[] args);
    public bool has_initialized ();
}

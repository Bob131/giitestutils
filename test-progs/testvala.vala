class TestCase : Gtu.TestCase {
    public string adsf {set; get;}

    protected override void test_impl () {
        base.test_impl ();
        message ("asdf");
        assert_not_reached ();
    }

    public TestCase (string name) {
        base.@construct (name);

        this.ancestry_changed.connect (() => {
            message (@"$(this.name) says: new parent!");
        });
    }
}

class ExpectCase : Gtu.TestCase {
    protected override void test_impl () {
        var handle = this.expect_message ("asdf", LogLevelFlags.LEVEL_CRITICAL,
                                          /klasd./);
        log ("asdf", LogLevelFlags.LEVEL_CRITICAL, "klasdf");
        log_structured ("asdf", LogLevelFlags.LEVEL_CRITICAL, "MESSAGE", "klasde");
        assert (this.expect_check (handle));
        assert (this.expect_count (handle) == 2);
        assert (!this.expect_check (handle));
    }

    public ExpectCase (string name) {
        base.@construct (name);
    }
}

class TestSuite : Gtu.TestSuite {
    public TestCase a {set; get;}
    public TestCase b {set; get;}

    ~TestSuite () {
        message ("gone");
    }

    public TestSuite (string name) {
        base (name);

        a = new TestCase ("a");
        b = new TestCase ("b");

        this.add (a);
        this.add (b);

        this.add (new ExpectCase ("regex"));

        this.add (new Gtu.TestCase ("log-fail", () => {
            log ("ff", LogLevelFlags.LEVEL_WARNING, "ffffsdf");
        }));
    }
}

class ComplexCase : Gtu.ComplexCase<Subunit> {
    public enum Subunit {
        A, B
    }

    protected override void test_impl (Subunit subunit) {
        message (subunit.to_string ());

        switch (subunit) {
            case Subunit.A:
                assert_not_reached ();

            case Subunit.B:
                return;
        }
    }

    public ComplexCase (string name) {
        base (name);
    }
}

int main (string[] args) {
    Gtu.init (args);

    var suite = new Gtu.TestSuite ("tests");
    suite.add (new TestCase ("asdf"));

    suite.add (new TestSuite ("suite"));

    suite.add (new ComplexCase ("complex"));

    return suite.run ();
}

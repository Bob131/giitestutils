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

class TestSuite : Gtu.TestSuite {
    public TestCase a {set; get;}
    public TestCase b {set; get;}

    public TestSuite (string name) {
        base (name);

        a = new TestCase ("a");
        b = new TestCase ("b");

        this.add (a);
        this.add (b);
    }
}

int main (string[] args) {
    Gtu.init (args);

    var suite = new Gtu.TestSuite ("tests");
    suite.add (new TestCase ("asdf"));

    unowned Gtu.TestSuite.Child child = suite.add (new TestSuite ("suite"));
    suite.add (new TestCase ("depends-on-suite").with_dep (child));

    return suite.run ();
}

Gii Test Utilities
===

The Gii Test Utilities are a (very small) library of utility functions intended
to make writing test suites with the [GLib testing framework][gtest] a little
less painful, particularly from Vala code. The main feature provided by GTU is a
system for automatically backing out of test cases once they've failed, removing
the need for extra boilerplate around assertions.

See [the docs][docs] for more information.

'Gii' is pronounced just like '[Gee][gee]', with a bizarre spelling to avoid
confusion and/or an implication of association.

[gtest]: https://developer.gnome.org/glib/stable/glib-Testing.html
[docs]:  https://docs.bob131.so/giitestutils/
[gee]:   https://wiki.gnome.org/Projects/Libgee

# Contributing to Quark

Thank you for your interest in contributing to Quark!

The easiest way for you to help is just by writing programs and testing them with each new early version of the
compiler. Feel free to open an issue for anything that may seem off or if you just simply want to request a feature.

If you are confused on where to start, try implementing basic data structures or make spin-offs of existing standard
library [source files](lib).

Any pull requests should change code in the [dev](https://github.com/quark-programming/quark/tree/dev) branch so they
are up-to-date with any new changes.

Once finished with a pull request, you can run the unit-tests through `make`:

```sh
make test
./unit-tests/unit-tests
```
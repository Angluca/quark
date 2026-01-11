<div align="center">
    <img alt="Quark Programming Language" src="assets/title.png" width="600px" />

[Homepage](https://quar.k.vu) | [Docs](https://quar.k.vu/docs.html) | [Contributing](CONTRIBUTING.md)
</div>

> [!IMPORTANT]
> This langauge is in early stages of development, everything is subject to change, when building the compiler make
> sure to use the [latest release](https://github.com/quark-programming/quark/releases) You can see and submit issues
> on the repository's [issues page](https://github.com/quark-programming/quark/issues)

## Building the Compiler

To build the Compiler:

```sh
make build # or specify output with `OUT` (defaults to `qc`)
make build OUT=/path/to/executable

./qc -h
```

## Compiling a Source File to C:

```sh
$ ./qc -h
$ ./qc <path> -o <out-path>
```

## Learn More

See the [standard library](lib) for examples of the language in use or visit the [docs](https://quar.k.vu/docs.html).

### Other Repositories

- [quark-programming/quark-docs](https://github.com/quark-programming/quark-docs) - Documentation Website for the Quark
Programming Language


- [Angluca/quark.vim](https://github.com/Angluca/quark.vim) - Vim plugin for Quark language
- [Angluca/quark_template](https://github.com/Angluca/quark_template) - Project template for Quark-lang

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md)

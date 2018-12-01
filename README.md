# PRESENT

C implementation of the PRESENT Block Cipher.

## Table of Contents
- [Introduction](#introduction)
- [How to Use](#how-to-use)
- [Examples](#examples)
- [Configuration](#configuration)
- [License](#license)
- [See Also](#see-also)

## Introduction

PRESENT is a lightweight block cipher algorithm designed in 2007 by Andrey
Bogdanov, Lars R. Knudsen, Gregor Leander, Christof Paar, Axel Poschmann,
Matthew J. B. Robshaw, Yannick Seurin, and C. Vikkelsoe.

The project was designed mainly for embedded systems but it is suitable to
use in any platform.

## How to Use

The module could be used by either adding source files directly to the
project, or compiling as a static library via pre-included build system.

To build the module as a static library, `TYPE` tag in the
[Config.mk](build/Config.mk) file must be chosen as `SLIB`. Additional source
files could be added by appending to `SRC` list in the
[Source.mk](build/Source.mk) file.

Object files(and assembly files if the `ASM_GEN` flag is set) and out file
are placed to `bin` folder.

To build the project, at the project root folder, run:

```
$ make
```

By default, `make` command compile files only when something changes in the
source code. To build whole project in any case, run:

```
$ make rebuild
```

To remove all generated files, run:

```
$ make clean
```

## Examples

The project has a test code which could be found under `test` folder. The code
demonstrates encrypt and decrypt operations with text blocks and keys given in
appendix I of the article.

Test outputs are placed to `bin/test` folder. Results could be seen by running
test out files.

To build test files, run:

```
$ make test
```

## Configuration

Project configurations grouped under two category; module configurations and
build configurations. If the module is not compiled as static library, build
configurations are negligible.

Module configurations are found under [conf.h](conf/conf.h) file. All options
related with modules are documented in that file.

Build configurations are found under [Config.mk](build/Config.mk) file. All
options related with build system are documented in that file.

Additionally, tools used to build project could be configured via
[Tools.mk](build/Tools.mk) file and build rules could be configured via
[Rules.mk](build/Rules.mk) file.

__NOTE:__ Every compiler has different features and flags. Therefore,
[Target.mk](build/Target.mk) and [Rules.mk](build/Rules.mk) files may have
to be rewritten.

## License

This project is licensed under the MIT License - see the
[LICENSE](LICENSE) file for details.

## See Also

- [PRESENT: An Ultra-Lightweight Block Cipher](
https://link.springer.com/chapter/10.1007%2F978-3-540-74735-2_31)

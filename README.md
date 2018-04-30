# meshToolbox
# 3D Surface Mesh Toolbox

An open-source toolbox for post-processing and analysis of 3D meshes obtained from SLAM or Structure-from-Motion solutions of underwater structures such as: shipwrecks, seabed, coral colonies and reefs, etc. Yet, it can be applied to any generic surface. It's aimed to process OBJ or PLY meshes exported from Agisoft Photoscan PSX or other 3D mesh processing tool such as Meshlab.

**meshToolbox** is a free and open source software [licensed](#license) under the [GNU GPLv3.0 License](https://en.wikipedia.org/wiki/GNU_General_Public_License), unless otherwise specified in particular modules or libraries (see LICENSE and README.md).

## Table of Contents
- [Modules list](#modules-list)
- [Requirements](#requirements)
- [Getting Started](#getting-started)
- [Software details](#software-details)
- [Contributing](#contributing)
- [License](#license)

## Modules list
- [objLoader](https://github.com/cappelletto/meshToolbox) Basic OBJ import module for debugging purposes. Relies on OBJ-Loader library (see #requirements)

Each module contains information describing its usage, with (usually) some useful README file. Code documentation is expected to be provided in Doxygen-compatible format. The current release contains C/C++, Python and/or Matlab implementations.

## Requirements

The current release has been developed and tested in Ubuntu 16.04 LTS 64 bits.

- [cmake 2.8](https://cmake.org/) - cmake making it happen again
- [OBJ-Loader](https://github.com/Bly7/OBJ-Loader) - OBJ Loading header file, released under MIT license.

## Getting Started

This repository provides an incremental collection of compatible, yet independent, modules for 3D mesh analysis. For more extensive and user-friendly solution please see other alternatives as Meshlab. To start using this toolbox, proceed to the standard *clone* steps:

```bash
cd <some_directory>
git clone https://github.com/cappelletto/meshToolbox.git
```

Then enter the desired module and compile it:
```bash
cd <module_directory>
mkdir build
cd build
cmake ..
make
```

## Software Details

- Implementation done in C++, Python and/or Matlab/Octave.

## Contributing

Summary of contributing guidelines (based on those of other projects such as OpenCV):

* One pull request per issue;
* Choose the right base branch;
* Include tests and documentation;
* Use small datasets for testing purposes;
* Follow always the same coding style guide. If possible, apply code formating with any IDE.

Feel free to fork, clone or join the dev team. For further information please contact J. Cappelletto (<cappelletto@gmail.com>) with the subject: **meshToolbox**.

## License

Copyright (c) 2017-2018 José Cappelletto (<cappelletto@gmail.com>).
Released under the [GNU GPLv3.0 License](LICENSE).

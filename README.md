# libtropic-util

> [!WARNING]
> This software shall not be used in production. Use with care. Basically this is a C wrapper for libtropic library. Once compiled, it can be executed from bash and used for direct access to TROPIC01 features.

Contributors, please follow [guidelines](https://github.com/tropicsquare/libtropic-util/blob/main/CONTRIBUTING.md).

> [!TIP]
> To evaluate the lt-util on Raspberry Pi using SPI communication, check out [the tutorial](./RPI_TUTORIAL.md).

# Clone

Use following command to clone repository:
```
git clone --recurse-submodules https://github.com/tropicsquare/libtropic-util
```

and follow building instruction based on what hardware you have.


### Supported Hardware

Because [Tropic Square](https://www.tropicsquare.com) provides not only [TROPIC01](https://www.tropicsquare.com/tropic01) in a form of a **silicon chip**, but also in a form of **various shields** and **devkits**, building instructions differ.

Choose instruction based on hardware you want to use:
* [Raspberrypi shield](./docs/Linux_SPI.md) (also compatible with linux systems where SPI interface is connected directly to chip)
* [USB devkit TS1302](./docs/TS1302_devkit.md)

### License

See the [LICENSE.md](LICENSE.md) file in the root of this repository or consult license information at [Tropic Square website](https://tropicsquare.com/license).
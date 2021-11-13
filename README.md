# RockÉTSd

---

- [RockÉTSd](#rockétsd)
  - [Dependencies](#dependencies)
  - [Installing third parties](#installing-third-parties)
  - [Compiling](#compiling)
  - [Running](#running)
  - [Configuration](#configuration)
    - [Modules](#modules)
      - [FakeModule](#fakemodule)
      - [Serial](#serial)
      - [Cute](#cute)
      - [InfluxDB](#influxdb)

---

`rocketsd` is a "daemon" that bridges between serial input (UART) and external
software, namely InfluxDB and [CuteStation](https://github.com/ngc7293/cutestation).
It loads protocol message/node IDs from an XML file but not packet structure.

`rocketsd` is not intented to be general-purpose software, it is tailored to the
need of [RockÉTS](https://clubrockets.ca), but I do however strive to display it
as an example of CuteStation daemon implementation, with the fool's hope that
someone else will use my software one day :).

This repository also includes `xmlprotoc.py`, a Python3 script that reads the
same protocol XML file and outputs a C-language with messge/node id defines
(`#define MESSAGE_ID`).

## Dependencies

This project has the following dependencies:

- CMake 3.17
- GCC 10 or later, MSVC 19 or later
- Ninja (optional)
- Python 3
- Conan
- `lxml` for `xmlprotoc.py`

Additionnally:

- [CuteStation](https://github.com/ngc7293/cutestation)
- InfluxDB 1.x (not compatible with InfluxDB 2)

## Installing third parties

```bash
pip install -r requirements.txt

mkdir build; cd build;
conan install .. -s compiler.libcxx=libstdc++11 -s build_type=Debug
```

You will most likely need to call conan with `--build=missing` because no matching Qt package is found in conancenter.

## Compiling

```bash
cd build;
cmake .. -GNinja -DCMAKE_BUILD_TYPE=Debug
ninja
```

## Running

rocketsd takes the following arguments

| argument        | type   | description                 |
|-----------------|--------|-----------------------------|
| `-c/--config`   | `path` | Path to configuration JSON  |
| `-p/--protocol` | `path` | Path to protocol definition |

These default to `config.json` and `protocol.xml` respectively.

```bash
# e.g.
build/bin/rocketsd --config config.json --protocol anirniq.xml
```

## Configuration

### Modules

All modules take the following parameters:

| element     | type           | description                                     |
|-------------|--------------- |-------------------------------------------------|
| "id"        | `string`       | Unique identifier for a module                  |
| "broadcast" | `list<string>` | IDs of mdule that this module will send data to |

#### Fake

Generates "fake" data points with the formula `α·sin(ωt) + φ`

| element     | type     | description                           |
|-------------|----------|---------------------------------------|
| `module`    | `string` | `"fake"`                              |
| `frequency` | `float`  | Frequency at which to generate values |
| `alpha`     | `float`  | Amplitude                             |
| `omega`     | `float`  | Horizontal scaling factor             |
| `phi`       | `float`  | Vertical offset                       |

#### Serial

Receives and transmits data on a UART (serial) port

| element    | type     | description                            |
|------------|----------|----------------------------------------|
| `module`   | `string` | `"cute"`                               |
| `port`     | `string` | Serial port to use (e.g.: `/dev/ttyX`) |
| `baudrate` | `uint`   | Baudrate                               |

#### Cute

Packages and sends data to CuteStation via UNIX or TCP sockets.

| element  | type     | description                                         |
|----------|----------|-----------------------------------------------------|
| `module` | `string` | `"cute"`                                            |
| `socket` | `string` | Path to the UNIX socket CuteStation is listening on |
| `host`   | `string` | TCP host address CuteStation is listening on        |
| `port`   | `uint`   | TCP port CuteStation is listening on                |

If both "socket" and "host"+"port" are configured, the Cute module will prefer
the UNIX socket.

#### InfluxDB

| element       | type     | description                                       |
|---------------|----------|---------------------------------------------------|
| `module`      | `string` | `"influx"`                                        |
| `url`         | `string` | The HTTP URL InfluxDB is listening on.            |
| `buffer_size` | `string` | Batch size for HTTP Posts to InfluxDB. See [here](https://docs.influxdata.com/influxdb/v1.7/tools/api/#request-body-1) |

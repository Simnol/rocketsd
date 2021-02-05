# RockÉTSd

`rocketsd` is a "daemon" that bridges between serial input (UART) and external software, namely InfluxDB and [CuteStation](https://github.com/ngc7293/cutestation). It loads protocol message/node IDs from an XML file but not packet structure.

`rocketsd` is not intented to be general-purpose software, it is tailored to the need of [RockÉTS](https://clubrockets.ca), but I do however strive to display it as an example of CuteStation daemon implementation, with the fool's hope that someone else will use my software one day :).

This repository also includes `xmlprotoc.py`, a Python3 script that reads the same protocol XML file and outputs a C-language with messge/node id defines (`#define MESSAGE_ID`).

## Dependencies

This project has the following dependencies:

- Qt 5.14 (the path is currently hard-coded in the makefile, you will have to update it locally)
- Protobuf 3.11 (for CuteStation endpoint)
- nlohmann::json
- CMake 3.17
- Ninja (optionnal)
- Python 3 for `xmlprotoc.py`

Additionnally:

- CuteStation
- InfluxDB

## Compiling

```bash
mkdir build; cd build;
cmake .. -GNinja
ninja
```

## Running

Configuration is not yet done, so `protocol.xml` must be in your current directory.

```bash
build/bin/rocketsd
```

## Configuration

### Modules

All modules will be connected with the first element of the "modules" array.

#### FakeModule

FakeModule generates "fake" data points with the formula `α × sin(ω × t)`

| element     | type     | description                           |
|-------------|----------|---------------------------------------|
| `module`    | `string` | `"fake"`                              |
| `frequency` | `float`  | Frequency at which to generate values |
| `alpha`     | `float`  | Amplitude                             |
| `omega`     | `float`  | Horizontal scaling factor             |

#### Serial

Receives and transmits data on a UART (serial) port

| element    | type     | description                            |
|------------|----------|----------------------------------------|
| `module`   | `string` | `"cute"`                               |
| `port`     | `string` | Serial port to use (e.g.: `/dev/ttyX`) |
| `baudrate` | `uint`   | Baudrate                               |

#### Cute

Packages and sends data to CuteStation via UNIX sockets

| element  | type     | description                                         |
|----------|----------|-----------------------------------------------------|
| `module` | `string` | `"cute"`                                            |
| `socket` | `string` | Path to the UNIX socket CuteStation is listening on |

#### InfluxDB

| element       | type     | description                                         |
|---------------|----------|-----------------------------------------------------|
| `module`      | `string` | `"influx"`                                          |
| `url`         | `string` | The HTTP URL InfluxDB is listening on.              |
| `buffer_size` | `string` | Batch size for HTTP Posts to InfluxDB. See [here](https://docs.influxdata.com/influxdb/v1.7/tools/api/#request-body-1) |

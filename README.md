# Quant Engine

This project is a high-frequency rolling statistics engine written in C++ with Python bindings using `pybind11`. It's designed for low-latency data ingestion and statistical computation.

## Requirements

*   A C++ compiler that supports C++20 (e.g., GCC, Clang, MSVC)
*   Python 3.9+
*   CMake 3.18+

## Installation

The project uses `scikit-build-core` to manage the C++ build process. You can install it directly using `pip`:

```bash
python -m pip install .
```

This command will compile the C++ extension and install the `quantengine` Python package into your environment.

## Usage

The primary interface to the engine is through the `quantengine` Python module. The engine is designed to ingest tick data and compute rolling statistics over a time window.

### Data Format

The engine ingests tick data from a CSV file. A sample data file is provided at `data/ticks.csv`. The CSV file must have the following columns:

*   `ts_ns`: Timestamp in nanoseconds (integer)
*   `price`: Tick price (float)
*   `size`: Tick size (float)
*   `symbol_id`: Symbol identifier (integer)

### Example

An example script is provided in the `examples` directory. To run the example:

```bash
python examples/example.py
```

The script will read the data from `data/ticks.csv`, ingest it into the engine, and print the computed statistics. The script will output the tick count, mean, standard deviation, and volume-weighted average price (VWAP) of the ingested data.

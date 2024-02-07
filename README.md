# aslm-creo

ASLM Plugin for Creo written in Rust.

## Prerequisites

- [Rust](https://www.rust-lang.org/tools/install)
- Creo Parametric 10

## Settings

There are a couple of environment variables that must be set to correctly build the plugin.

```
CREO_TOOLKIT=<dir-of-protktools> # Example: C:\Program Files\PTC\Creo 10.0.0.0\Common Files\protoolkit
PROTOOL_DEBUG=<debug-level> # Example: crash
PROTOOL_LOG_DIR=<debug-dir> # Example: C:\projects\aslm-creo\build
PRO_COMM_MSG_EXE=<dir-of-pro_comm_msg.exe> # Example: C:\Program Files\PTC\Creo 10.0.0.0\Common Files\x86e_win64\obj\pro_comm_msg.exe
```

## Building and Testing

After building the project, the DLL can be loaded into Creo via the creotk.dat file. Make sure to adjust it with the correct paths to the DLL and `text` folder.

```bash
cargo build
cargo test # This will only test the FFI interface, not the plugin
```

## Synchronous and Asynchronous Mode

Creo offers different modes for the plugin to run in. The synchronous mode is the default and the fastest, and is loaded via the Creo UI as DLL. The asynchronous mode is started individually and communicates with the Creo process.

In a next iteration we might want to split this up in separate projects, a `sync` and a `async` project. Each projects links against the correct libraries. The `async` project will contain all tests, and we spawn the Creo process from the tests.

## Docs

Some interesting articles and documentation about Creo and the Creo Toolkit:

- Geometry - /protkdoc/manual0/Geometry.html
- Features and Sections - /protkdoc/manual0/Sketched_features_create.html

## Strategy

1. Create sketch
1. Fill 2D sketch with Datum points for the lattice structure using the plugin
1. Extrude 2D sketch so that we have a volume and a Datum axis (for all points) using Creo
1. Depending on the cell and strategy we fill the volume with points along the axis using the plugin
1. Generate 3D lattice structure from points using the plugin. We can sweep between the different points to create a lattice structure and copy the structure

# creo-rs

This project aims to provide a safe and ergonomic Rust interface to the Creo Parametric Toolkit.

## Prerequisites

- Creo Parametric 10 with Toolkit installed

## Building and Testing

To build this as a DLL plugin, add this to your `Cargo.toml`:

```toml
[lib]
crate-type = ["cdylib"]
```

There are a couple of environment variables that must be set to correctly build the plugin.

```
CREO_TOOLKIT=<dir-of-protktools> # Example: C:\Program Files\PTC\Creo 10.0.0.0\Common Files\protoolkit
```

After building the project, the DLL can be loaded into Creo via a `creotk.dat` file. Make sure to adjust it with the correct paths to the DLL and `text` folder.

## Example

An example can be found in [creo-example](crates/creo-example).
The most barebone application would look like this:

```rust
use creo::ffi;

/// Initializes the plugin
#[no_mangle]
pub extern "C" fn user_initialize() -> ffi::ProErrors {
    ffi::ProErrors_PRO_TK_NO_ERROR
}

/// User exits
#[no_mangle]
pub extern "C" fn user_terminate() -> ffi::ProErrors {
    ffi::ProErrors_PRO_TK_NO_ERROR
}
```

## Synchronous and Asynchronous Mode

Creo offers different modes for the plugin to run in. The synchronous mode is the default and the fastest, and is loaded via the Creo UI as DLL. The asynchronous mode is started individually and communicates with the Creo process.

For now we only support synchronous mode. In a next iteration we probably split this up in separate projects, a `sync` and a `async` project. Each projects links against the correct libraries.

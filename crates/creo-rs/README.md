# creo-rs

This project aims to provide a safe and ergonomic Rust interface to the Creo Parametric Toolkit.

## Prerequisites

- [Rust](https://www.rust-lang.org/tools/install)
- Creo Parametric 10 with Toolkit installed

## Example

This is a simple example of a Creo plugin that selects a feature and prints it to the console. Note that 

```rust
use creo::pro_feature::*;
use creo::pro_selection::*;
use creo;

/// Initializes the plugin
#[no_mangle]
pub extern "C" fn user_initialize() -> ffi::ProError {
    let selection = pro_select("feature", 1, None, None, None, None)?;
    if selection.n_sel != 1 {
        return Err(ffi::ProErrors_PRO_TK_GENERAL_ERROR);
    }
    let feature: ffi::ProFeature = selection[0].modelitem_get()?;
    println!("Selected feature: {:?}", feature);
    ffi::ProErrors_PRO_TK_NO_ERROR
}

/// User exits
#[no_mangle]
pub extern "C" fn user_terminate() -> i32 {
    return 0;
}
```

To build this as a DLL plugin, add this to your `Cargo.toml`:

```toml
[lib]
crate-type = ["cdylib"]
```

## Building and Testing

There are a couple of environment variables that must be set to correctly build the plugin.

```
CREO_TOOLKIT=<dir-of-protktools> # Example: C:\Program Files\PTC\Creo 10.0.0.0\Common Files\protoolkit
```

After building the project, the DLL can be loaded into Creo via the creotk.dat file. Make sure to adjust it with the correct paths to the DLL and `text` folder.

```bash
cargo build
cargo test
```

## Synchronous and Asynchronous Mode

Creo offers different modes for the plugin to run in. The synchronous mode is the default and the fastest, and is loaded via the Creo UI as DLL. The asynchronous mode is started individually and communicates with the Creo process.

For now we only support synchronous mode. In a next iteration we might want to split this up in separate projects, a `sync` and a `async` project. Each projects links against the correct libraries.

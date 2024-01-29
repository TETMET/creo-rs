# creo-example

This is an example crate for the Creo project. Please update the `example.dat` file with the correct absolute paths before it can be used.

Creo Parametric Toolkit must be present in the path. To do so, add:

```console
CREO_TOOLKIT=<dir-of-protktools> # Example: C:\Program Files\PTC\Creo 10.0.0.0\Common Files\protoolkit
```

Then build the project and register the DLL in Creo Parametric.

```
cargo build
```

The logs should be written to the public Documents folder in Windows. In my case `C:\Users\Public`.
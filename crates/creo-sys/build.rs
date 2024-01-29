use std::env;
use std::path::PathBuf;

#[cfg(not(target_os = "windows"))]
fn main() {
    compile_error!("This crate only supports Windows");
}

#[cfg(target_os = "windows")]
fn main() {
    // Get the Creo path from the CREO_TOOLKIT env variable
    let creo_path = env::var("CREO_TOOLKIT").expect("CREO_TOOLKIT is not set");
    let libdir_path = PathBuf::from(creo_path)
        // Canonicalize the path as `rustc-link-search` requires an absolute path
        .canonicalize()
        .expect("Cannot canonicalize path");
    let creo_headers = libdir_path.join("includes");
    let lib_path = libdir_path.join("x86e_win64").join("obj");
    let header_path = PathBuf::from("creo").join("wrapper.h");

    // Tell cargo to look for shared libraries in the specified directory
    println!("cargo:rustc-link-search={}", lib_path.to_str().unwrap());

    // Link all Creo libraries
    println!("cargo:rustc-link-lib=protk_dll_NU");
    println!("cargo:rustc-link-lib=ucore");
    println!("cargo:rustc-link-lib=udata");
    println!("cargo:rustc-link-lib=kernel32");
    println!("cargo:rustc-link-lib=user32");
    println!("cargo:rustc-link-lib=wsock32");
    println!("cargo:rustc-link-lib=advapi32");
    println!("cargo:rustc-link-lib=mpr");
    println!("cargo:rustc-link-lib=winspool");
    println!("cargo:rustc-link-lib=netapi32");
    println!("cargo:rustc-link-lib=psapi");
    println!("cargo:rustc-link-lib=gdi32");
    println!("cargo:rustc-link-lib=shell32");
    println!("cargo:rustc-link-lib=comdlg32");
    println!("cargo:rustc-link-lib=ole32");
    println!("cargo:rustc-link-lib=ws2_32");

    let bindings = bindgen::Builder::default()
        .header(header_path.to_str().unwrap())
        .clang_arg(format!("-I{}", creo_headers.to_str().unwrap()))
        .parse_callbacks(Box::new(bindgen::CargoCallbacks::new()))
        .generate()
        .expect("Unable to generate bindings");

    // Write the bindings to the $OUT_DIR/bindings.rs file.
    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap()).join("bindings.rs");
    bindings
        .write_to_file(out_path)
        .expect("Couldn't write bindings!");
}

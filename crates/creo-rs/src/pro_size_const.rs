use creo_sys;

use std::{ffi::OsString, os::windows::ffi::OsStrExt};

pub struct ProMdlName {
    /// The name of the model in wide characters
    pub name_w: Vec<u16>,
}

impl ProMdlName {
    /// Creates a new `ProMdlName` object.
    pub fn new(name: &str) -> ProMdlName {
        let mut name_w = OsString::from(name).encode_wide().collect::<Vec<_>>();
        name_w.push(0);
        name_w.resize(creo_sys::PRO_MDLNAME_SIZE as usize, 0);
        ProMdlName { name_w }
    }
}

#[derive(Debug, Clone)]
pub struct ProPath {
    /// The path in wide characters
    pub path_w: Vec<u16>,
}

impl ProPath {
    pub fn new(path: &str) -> ProPath {
        let mut path_w = OsString::from(path).encode_wide().collect::<Vec<_>>();
        path_w.push(0);
        path_w.resize(creo_sys::PRO_PATH_SIZE as usize, 0);
        ProPath { path_w }
    }
}

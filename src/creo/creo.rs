#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(unused_imports)]
#![allow(dead_code)]

use std::convert::Into;

pub mod ffi {
    include!(concat!(env!("OUT_DIR"), "/bindings.rs"));
}

use crate::creo::creo::ffi;

/// A wrapper around the `ProElempath` object to manage its memory.
pub struct ProElempath {
    pub pro_elempath_ptr: ffi::ProElempath,
}

impl ProElempath {
    pub fn new() -> ProElempath {
        unsafe {
            let mut pro_elempath_ptr: ffi::ProElempath = std::mem::zeroed();
            ffi::ProElempathAlloc(&mut pro_elempath_ptr);
            ProElempath { pro_elempath_ptr }
        }
    }
}

impl Drop for ProElempath {
    fn drop(&mut self) {
        unsafe {
            ffi::ProElempathFree(&mut self.pro_elempath_ptr);
        }
    }
}

use creo_sys;

/// A wrapper around the `ProElempath` object to manage its memory.
pub struct ProElempath {
    pub pro_elempath_ptr: creo_sys::ProElempath,
}

impl ProElempath {
    /// Creates a new `ProElempath` object.
    /// Memory is allocated for the `ProElempath` object and is automatically freed when it goes out of scope.
    pub fn new() -> ProElempath {
        unsafe {
            let mut pro_elempath_ptr: creo_sys::ProElempath = std::mem::zeroed();
            creo_sys::ProElempathAlloc(&mut pro_elempath_ptr);
            ProElempath { pro_elempath_ptr }
        }
    }
}

impl Drop for ProElempath {
    fn drop(&mut self) {
        unsafe {
            creo_sys::ProElempathFree(&mut self.pro_elempath_ptr);
        }
    }
}

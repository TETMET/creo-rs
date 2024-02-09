use crate::creo::creo::ffi;

pub trait ProReferenceInterface {
    /// Sets the referenced model item and path in the reference handle.
    ///
    /// # Arguments
    ///
    /// * `path` - The component path. If `None`, the reference handle will be set as local reference.
    /// * `item` - The model item handle. If `None`, the model item from `ProAsmcomppathMdlGet() -> ProMdlToModelitem()` will be set to the reference handle.
    ///
    /// # Errors
    ///
    /// * `PRO_TK_NO_ERROR` - The function succeeded.
    /// * `PRO_TK_BAD_INPUTS` - One or more arguments was invalid.
    fn set(
        &self,
        path: Option<*mut ffi::ProAsmcomppath>,
        item: Option<*mut ffi::ProModelitem>,
    ) -> Result<(), ffi::ProError>;

    /// Gets the item id of a reference handle.
    ///
    /// # Errors
    ///
    /// * `PRO_TK_NO_ERROR` - The function succeeded.
    /// * `PRO_TK_BAD_INPUTS` - One or more arguments was invalid.
    /// * `PRO_TK_EMPTY` - This value is not set in the reference.
    fn id_get(&self) -> i32;
}

impl ProReferenceInterface for ProReference {
    fn set(
        &self,
        path: Option<*mut ffi::ProAsmcomppath>,
        item: Option<*mut ffi::ProModelitem>,
    ) -> Result<(), ffi::ProError> {
        self.pro_reference_ptr.set(path, item)
    }

    fn id_get(&self) -> i32 {
        self.pro_reference_ptr.id_get()
    }
}

impl ProReferenceInterface for ffi::ProReference {
    fn set(
        &self,
        path: Option<*mut ffi::ProAsmcomppath>,
        item: Option<*mut ffi::ProModelitem>,
    ) -> Result<(), ffi::ProError> {
        unsafe {
            let path_ptr = match path {
                Some(path) => path,
                None => std::mem::zeroed(),
            };
            let item_ptr = match item {
                Some(item) => item,
                None => std::mem::zeroed(),
            };
            let status = ffi::ProReferenceSet(*self, path_ptr, item_ptr);
            if status != ffi::ProErrors_PRO_TK_NO_ERROR {
                return Err(status);
            }
            Ok(())
        }
    }

    fn id_get(&self) -> i32 {
        unsafe {
            let mut id = 0;
            ffi::ProReferenceIdGet(*self, &mut id);
            id
        }
    }
}

pub struct ProReference {
    pub pro_reference_ptr: ffi::ProReference,
}

impl ProReference {
    pub fn new() -> ProReference {
        unsafe {
            let mut pro_reference_ptr = std::mem::zeroed();
            ffi::ProReferenceAlloc(&mut pro_reference_ptr);
            ProReference { pro_reference_ptr }
        }
    }
}

impl Drop for ProReference {
    fn drop(&mut self) {
        unsafe {
            ffi::ProReferenceFree(self.pro_reference_ptr);
        }
    }
}

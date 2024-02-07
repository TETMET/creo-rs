use crate::creo::creo::ffi;

pub trait ProAxisInterface {
    fn to_geomitem(&self, solid: &ffi::ProSolid) -> Result<ffi::ProGeomitem, ffi::ProError>;
}

impl ProAxisInterface for ffi::ProAxis {
    /// Converts a `ProAxis` handle to its `ProGeomitem` handle.
    ///
    /// # Arguments
    ///
    /// * `solid` - The owner of the `axis_handle`.
    ///
    /// # Errors
    ///
    /// * `PRO_TK_NO_ERROR` - The function successfully converted the axis handle to its geometry item handle.
    /// * `PRO_TK_BAD_INPUTS` - One or more of the input arguments are invalid.
    fn to_geomitem(&self, solid: &ffi::ProSolid) -> Result<ffi::ProGeomitem, ffi::ProError> {
        unsafe {
            let mut geomitem: ffi::ProGeomitem = std::mem::zeroed();
            let status = ffi::ProAxisToGeomitem(*solid, *self, &mut geomitem);
            if status != ffi::ProErrors_PRO_TK_NO_ERROR {
                return Err(status);
            }
            Ok(geomitem)
        }
    }
}

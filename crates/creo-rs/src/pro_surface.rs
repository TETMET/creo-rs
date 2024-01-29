use creo_sys;

use crate::pro_toolkit_errors::ProErrors;

pub trait ProSurfaceInterface {
    /// Converts a `ProSurface` handle to its `ProGeomitem` handle.
    ///
    /// # Arguments
    ///
    /// * `solid` - The owner of the `surface_handle`.
    ///
    /// # Errors
    ///
    /// * `PRO_TK_BAD_INPUTS` - One or more of the input arguments are invalid.
    fn to_geomitem(&self, solid: &creo_sys::ProSolid) -> Result<creo_sys::ProGeomitem, ProErrors>;
}

impl ProSurfaceInterface for creo_sys::ProSurface {
    fn to_geomitem(&self, solid: &creo_sys::ProSolid) -> Result<creo_sys::ProGeomitem, ProErrors> {
        unsafe {
            let mut geomitem: creo_sys::ProGeomitem = std::mem::zeroed();
            let status = creo_sys::ProSurfaceToGeomitem(*solid, *self, &mut geomitem);
            match status {
                creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(geomitem),
                _ => Err(status.into()),
            }
        }
    }
}

use creo_sys;

use crate::pro_toolkit_errors::ProErrors;

pub trait ProPointInterface {
    /// Retrieves the XYZ coordinates of the specified point.
    ///
    /// # Errors
    ///
    /// * `PRO_TK_BAD_INPUTS` - If the input argument is invalid.
    fn coord_get(&self) -> Result<creo_sys::ProVector, ProErrors>;
}

impl ProPointInterface for creo_sys::ProPoint {
    fn coord_get(&self) -> Result<creo_sys::ProVector, ProErrors> {
        unsafe {
            let mut coord = creo_sys::ProVector::default();
            let status = creo_sys::ProPointCoordGet(*self, &mut coord as *mut _);
            match status {
                creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(coord),
                _ => Err(status.into()),
            }
        }
    }
}

use crate::{pro_geomitem::ProGeomitemdata, pro_toolkit_errors::ProErrors};
use creo_sys;

pub trait ProAxisInterface {
    /// Retrieves the geometric representation data for the specified axis.
    ///
    /// # Errors
    ///
    /// * `PRO_TK_BAD_INPUTS` - The input argument is invalid.
    fn data_get(&self) -> Result<ProGeomitemdata, ProErrors>;

    /// Converts a `ProAxis` handle to its `ProGeomitem` handle.
    ///
    /// # Arguments
    ///
    /// * `solid` - The owner of the `axis_handle`.
    ///
    /// # Errors
    ///
    /// * `PRO_TK_BAD_INPUTS` - One or more of the input arguments are invalid.
    fn to_geomitem(&self, solid: &creo_sys::ProSolid) -> Result<creo_sys::ProGeomitem, ProErrors>;
}

impl ProAxisInterface for creo_sys::ProAxis {
    fn data_get(&self) -> Result<ProGeomitemdata, ProErrors> {
        unsafe {
            let mut axis_data = std::mem::zeroed();
            let status = creo_sys::ProAxisDataGet(*self, &mut axis_data);
            match status {
                creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(ProGeomitemdata::new(axis_data)),
                _ => Err(status.into()),
            }
        }
    }

    fn to_geomitem(&self, solid: &creo_sys::ProSolid) -> Result<creo_sys::ProGeomitem, ProErrors> {
        unsafe {
            let mut geomitem: creo_sys::ProGeomitem = std::mem::zeroed();
            let status = creo_sys::ProAxisToGeomitem(*solid, *self, &mut geomitem);
            match status {
                creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(geomitem),
                _ => Err(status.into()),
            }
        }
    }
}

use creo_sys;

use crate::pro_toolkit_errors::ProErrors;

/// Obtains the name of the element id
///
/// # Arguments
///
/// * `elem_id` - The element id.
///
/// # Errors
///
/// * PRO_TK_NO_ERROR - The name of the element id.
/// * PRO_TK_BAD_INPUTS - One or more input arguments was invalid.
/// * PRO_TK_E_NOT_FOUND - The given element id was not found.
pub fn pro_elem_id_string_get(elem_id: creo_sys::ProElemId) -> Result<String, ProErrors> {
    let mut elem_id_str: [std::ffi::c_char; 81] = [0; 81];
    unsafe {
        let status = creo_sys::ProElemIdStringGet(elem_id, elem_id_str.as_mut_ptr());
        if status != creo_sys::ProErrors_PRO_TK_NO_ERROR {
            return Err(status.into());
        }
        let elem_id_str = std::ffi::CStr::from_ptr(elem_id_str.as_ptr())
            .to_str()
            .unwrap()
            .to_string();
        Ok(elem_id_str)
    }
}

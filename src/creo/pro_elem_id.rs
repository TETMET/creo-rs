use crate::creo::creo::ffi;

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
pub fn pro_elem_id_string_get(elem_id: ffi::ProElemId) -> Result<String, i32> {
    let mut elem_id_str: [std::ffi::c_char; 81] = [0; 81];
    unsafe {
        let status = ffi::ProElemIdStringGet(elem_id, elem_id_str.as_mut_ptr());
        if status != ffi::ProErrors_PRO_TK_NO_ERROR {
            return Err(status);
        }
        let elem_id_str = std::ffi::CStr::from_ptr(elem_id_str.as_ptr())
            .to_str()
            .unwrap()
            .to_string();
        Ok(elem_id_str)
    }
}

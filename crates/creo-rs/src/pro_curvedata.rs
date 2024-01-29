use creo_sys;

use crate::pro_toolkit_errors::ProErrors;

/// Retrieves information from the line data structure.
///
/// # Output
///
/// * `end1` - The beginning of the line
/// * `end2` - The end of the line
///
/// # Errors
///
/// * `PRO_TK_INVALID_TYPE` - The specified data is not a line data structure.
/// * `PRO_TK_BAD_INPUTS` - The input argument is invalid.
pub fn pro_linedata_get(
    p_curve: *mut creo_sys::ProCurvedata,
) -> Result<(creo_sys::Pro3dPnt, creo_sys::Pro3dPnt), ProErrors> {
    unsafe {
        let mut start_point: [f64; 3] = std::mem::zeroed();
        let mut end_point: [f64; 3] = std::mem::zeroed();
        let status =
            creo_sys::ProLinedataGet(p_curve, start_point.as_mut_ptr(), end_point.as_mut_ptr());
        match status {
            creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok((start_point, end_point)),
            _ => Err(status.into()),
        }
    }
}

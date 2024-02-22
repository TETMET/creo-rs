use creo_sys;

use std::{ffi::OsString, os::windows::ffi::OsStringExt};

use crate::pro_toolkit_errors::ProErrors;

/// Gets the absolute path name of the current working directory for Creo Parametric.
///
/// # Errors
///
/// * `PRO_TK_E_NOT_FOUND` - The function was unable to find current working directory.
pub fn pro_directory_current_get() -> Result<String, ProErrors> {
    unsafe {
        let mut dir_path = [0; creo_sys::PRO_PATH_SIZE as usize];
        let status = creo_sys::ProDirectoryCurrentGet(dir_path.as_mut_ptr());
        match status {
            creo_sys::ProErrors_PRO_TK_NO_ERROR => {
                Ok(OsString::from_wide(&dir_path).into_string().unwrap())
            }
            _ => Err(status.into()),
        }
    }
}

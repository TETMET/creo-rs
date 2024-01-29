use creo_sys;

use crate::{pro_size_const::ProMdlName, pro_toolkit_errors::ProErrors};

/// Retrieves a window and attaches the object to it.
/// If the window is already open with the object attached,
/// the function returns the window identifier. If there is an
/// empty window up, the function attaches the object to it.
/// Otherwise, the function creates a new window and attaches
/// the object to it.
///
/// NOTE:
///
/// This function should be used only when the application
/// must display more than one object simultaneously. Under normal
/// circumstances, use ProMdlDisplay() to show an object in the
/// current or base window.
/// This function should be used on model present in the session.
///
/// **Note:** In Creo 3 this function does not support names longer than
/// 80 characters. It will return PRO_TK_BAD_INPUTS for longer names.
/// The support for ProMdlName will be added in a future release.
///
/// # Arguments
///
/// * `object_name` - The name of the object to be retrieved
/// * `object_type` - The type of the object (PRO_PART, PRO_ASSEMBLY, PRO_CATIA_PART,
///                   PRO_SW_ASSEM and so on)
///
/// # Returns
///
/// * `Result<i32, ProError>` - The window identifier in which the object was retrieved
///
/// # Errors
///
/// * `PRO_TK_NO_ERROR` - The function successfully retrieved the window and attached the object to it.
/// * `PRO_TK_INVALID_TYPE` - You specified an invalid object type.
/// * `PRO_TK_BAD_INPUTS` - Either you specified an invalid object name, or the window identifier is NULL.
/// * `PRO_TK_E_DEADLOCK` - The Creo Parametric user interface is not prepared to handle this request
///                         (because it is blocked by another process such as an OLE container activation).
/// * `PRO_TK_E_NOT_FOUND` - The application is trying to open an embedded model
///                          (that is, a model representing a single body of a multi-body foreign object)
///                          in a separate window. This operation is not supported.
pub fn pro_objectwindow_mdlname_create(
    object_name: &str,
    object_type: creo_sys::ProType,
) -> Result<i32, ProErrors> {
    unsafe {
        let mut window_id = 0;
        let mut object_name_w = ProMdlName::new(object_name);
        let status = creo_sys::ProObjectwindowMdlnameCreate(
            object_name_w.name_w.as_mut_ptr(),
            object_type,
            &mut window_id,
        );
        match status {
            creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(window_id),
            _ => Err(status.into()),
        }
    }
}

/// Activates the specified window.
///
/// # Arguments
///
/// * `window` - The handle to the Creo Parametric window
///
/// # Errors
///
/// * `PRO_TK_BAD_INPUTS` - The input argument is invalid.
/// * `PRO_TK_BAD_CONTEXT` - The function was called in RPC input (non-graphic) mode.
/// * `PRO_TK_GENERAL_ERROR` - The function failed.
///
/// # See Also
///
/// * `ProUIDialogAppActionSet()`
/// * `ProUIDialogAppActionRemove()`
pub fn pro_window_activate(window: i32) -> Result<(), ProErrors> {
    unsafe {
        let status = creo_sys::ProWindowActivate(window);
        match status {
            creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(()),
            _ => Err(status.into()),
        }
    }
}

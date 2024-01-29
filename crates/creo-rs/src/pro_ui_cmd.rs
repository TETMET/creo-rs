use creo_sys;

use std::ffi::CString;

use crate::pro_toolkit_errors::ProErrors;

/// Adds a new action to Creo Parametric. This action can be later associated with a push button command in the Creo Parametric Ribbon UI.
///
/// # Arguments
///
/// * `action_name` - The name of the action (must be unique).
/// * `action_cb` - The callback function.
/// * `priority` - Defines the priority of the action with respect to other actions that can be invoked during the execution of the action callback function.
/// * `access_func` - The function that defines the accessibility of the action.
/// * `allow_in_non_active_window` - Defines whether this action can be executed in a nonactive Creo Parametric window.
/// * `allow_in_accessory_window` - Defines whether this action can be executed in an accessory Creo Parametric window.
///
/// # Errors
///
/// * `PRO_TK_GENERAL_ERROR` - The function failed to add the action.
/// * `PRO_TK_E_FOUND` - An action with this name already exists.
pub fn pro_cmd_action_add(
    action_name: &str,
    action_cb: creo_sys::uiCmdCmdActFn,
    priority: i32,
    access_func: creo_sys::uiCmdAccessFn,
    allow_in_non_active_window: bool,
    allow_in_accessory_window: bool,
) -> Result<creo_sys::uiCmdCmdId, ProErrors> {
    let action_name_cstr = CString::new(action_name).unwrap();
    let mut action_id: creo_sys::uiCmdCmdId = std::ptr::null_mut();
    unsafe {
        let status = creo_sys::ProCmdActionAdd(
            action_name_cstr.as_ptr() as *mut i8,
            action_cb,
            priority,
            access_func,
            allow_in_non_active_window as i32,
            allow_in_accessory_window as i32,
            &mut action_id,
        );
        match status {
            creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(action_id),
            _ => Err(status.into()),
        }
    }
}

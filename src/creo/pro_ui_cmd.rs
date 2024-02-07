use crate::creo::creo::ffi;
use std::ffi::CString;

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
/// * `PRO_TK_NO_ERROR` - The function successfully added the action.
/// * `PRO_TK_GENERAL_ERROR` - The function failed to add the action.
/// * `PRO_TK_E_FOUND` - An action with this name already exists.
pub fn pro_cmd_action_add(
    action_name: &str,
    action_cb: ffi::uiCmdCmdActFn,
    priority: i32,
    access_func: ffi::uiCmdAccessFn,
    allow_in_non_active_window: bool,
    allow_in_accessory_window: bool,
) -> Result<ffi::uiCmdCmdId, i32> {
    let action_name_cstr = CString::new(action_name).unwrap();
    let mut action_id: ffi::uiCmdCmdId = std::ptr::null_mut();
    unsafe {
        let status = ffi::ProCmdActionAdd(
            action_name_cstr.as_ptr() as *mut i8,
            action_cb,
            priority,
            access_func,
            allow_in_non_active_window as i32,
            allow_in_accessory_window as i32,
            &mut action_id,
        );
        if status != ffi::ProErrors_PRO_TK_NO_ERROR {
            Err(status)
        } else {
            Ok(action_id)
        }
    }
}

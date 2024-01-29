use creo_sys;

use std::{
    ffi::{CString, OsString},
    os::windows::ffi::OsStrExt,
};

/// DEPRECATED: Since Creo 1
/// SUCCESSORS: CONTEXTUAL
/// Note: This API is deprecated. Use Customize UI dialog to
/// create menus in Creo Parametric Ribbon UI.
///
/// Adds a new menu to the menu bar of Creo Parametric.  This menu can
/// be used later to add another menus, push buttons, check
/// buttons, and radio groups.
///
/// This function affects the top-level Creo Parametric menu bar only.
///
/// This function is executed only once during a Creo Parametric
/// session for each menu. Subsequent calls to this function
/// for a previously loaded menu are ignored.
///
/// # Arguments
///
/// * `menu_name` - The name of the menu (must be unique)
/// * `untranslated_menu_label` - The label of the menu
/// * `neighbor` - The neighbor menu in the menu bar. Note:
///          If you are adding the first menu to the menu bar of
///          Creo Parametric, set the `neighbor` argument to NULL.
/// * `add_after_neighbor` - Defines whether to add the menu
///          to the left or right of the neighbor menu;
///          PRO_B_TRUE specifies to the right.
/// * `filename` - Name of the message file containing the label
///
/// # Errors
///
/// * `PRO_TK_MSG_NOT_FOUND` - The specified message was not found in the
///          message file.
/// * `PRO_TK_E_FOUND` - a menubar entity with this name already exists.
/// * `PRO_TK_GENERAL_ERROR` - The function failed to add the menu.
pub fn pro_menubar_menu_add(
    menu_name: &str,
    untranslated_menu_label: &str,
    neighbor: Option<String>,
    add_after_neighbor: bool,
    filename: &str,
) -> Result<(), i32> {
    let menu_name_cstr = CString::new(menu_name).unwrap();
    let untranslated_menu_label_cstr = CString::new(untranslated_menu_label).unwrap();
    let neighbor_cstr = neighbor.map(|s| CString::new(s).unwrap());
    let neighbor_cstr_ptr = neighbor_cstr.map_or(std::ptr::null(), |s| s.as_ptr());
    let add_after_neighbor = if add_after_neighbor {
        creo_sys::ProBooleans_PRO_B_TRUE
    } else {
        creo_sys::ProBooleans_PRO_B_FALSE
    };
    let mut wfilename = OsString::from(filename).encode_wide().collect::<Vec<_>>();
    wfilename.push(0);
    unsafe {
        let status = creo_sys::ProMenubarMenuAdd(
            menu_name_cstr.as_ptr() as *mut i8,
            untranslated_menu_label_cstr.as_ptr() as *mut i8,
            neighbor_cstr_ptr as *mut i8,
            add_after_neighbor,
            wfilename.as_mut_ptr(),
        );
        match status {
            creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(()),
            _ => Err(status),
        }
    }
}

/// DEPRECATED: Since Creo 1
/// SUCCESSORS: CONTEXTUAL
/// Purpose: **Note:** This API is deprecated. Use Customize UI dialog to
/// create menus in Creo Parametric Ribbon UI.
///
/// Adds a new push button to the menu of Creo Parametric.
///
/// This function affects the top-level Creo Parametric menu bar only.
///
/// This function is executed only once during a Creo Parametric
/// session for each push button. Subsequent calls to this function
/// for a previously loaded push button are ignored.
///
/// **Note:**
///
/// To find the name of an existing item to use as a neighbor,
/// select that item in Creo Parametric and look at your trail file.
///
/// Input Arguments:
/// - `parent_menu`: The name of the parent menu
/// - `push_button_name`: The name of the push button (must be unique)
/// - `push_button_label`: The label of the push button
/// - `one_line_help`: The one-line help for the push button
/// - `neighbor`: The neighbor item in the parent menu.
///   **Note:** If you are adding the first item to a new menu
///   (created using either `ProMenubarMenuAdd()` or
///   `ProMenubarmenuMenuAdd()`), set the `neighbor` argument to `None`.
/// - `add_after_neighbor`: Defines whether to add the button before or
///   after the neighbor item; `true` specifies after.
/// - `action_id`: The identifier of the action attached to the push button
/// - `filename`: The name of the message file that contains the
///   label and help string
///
/// # Errors
///
/// - `PRO_TK_MSG_NOT_FOUND`: The specified message was not found in the
///   message file.
/// - `PRO_TK_E_FOUND`: A menubar entity with this name already exists.
/// - `PRO_TK_GENERAL_ERROR`: The function failed to add a push button.
pub fn pro_menubarmenu_pushbutton_add(
    parent_menu: &str,
    push_button_name: &str,
    push_button_label: &str,
    one_line_help: &str,
    neighbor: Option<String>,
    add_after_neighbor: bool,
    action_id: creo_sys::uiCmdCmdId,
    filename: &str,
) -> Result<(), i32> {
    let parent_menu_cstr = CString::new(parent_menu).unwrap();
    let push_button_name_cstr = CString::new(push_button_name).unwrap();
    let push_button_label_cstr = CString::new(push_button_label).unwrap();
    let one_line_help_cstr = CString::new(one_line_help).unwrap();
    let neighbor_cstr = neighbor.map(|s| CString::new(s).unwrap());
    let neighbor_cstr_ptr = neighbor_cstr.map_or(std::ptr::null(), |s| s.as_ptr());
    let mut wfilename = OsString::from(filename).encode_wide().collect::<Vec<_>>();
    wfilename.push(0);
    unsafe {
        let status = creo_sys::ProMenubarmenuPushbuttonAdd(
            parent_menu_cstr.as_ptr() as *mut i8,
            push_button_name_cstr.as_ptr() as *mut i8,
            push_button_label_cstr.as_ptr() as *mut i8,
            one_line_help_cstr.as_ptr() as *mut i8,
            neighbor_cstr_ptr as *mut i8,
            add_after_neighbor as i32,
            action_id,
            wfilename.as_mut_ptr(),
        );
        match status {
            creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(()),
            _ => Err(status),
        }
    }
}

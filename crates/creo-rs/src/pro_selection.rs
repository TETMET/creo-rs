use creo_sys;

use std::ops;

use crate::pro_toolkit_errors::ProErrors;

pub trait ProSelectionInterface {
    /// Gets the model item from a selection object.
    ///
    /// # Errors
    ///
    /// * `PRO_TK_NOT_EXIST` - The model item doesn't exist.
    /// * `PRO_TK_BAD_INPUTS` - The supplied inputs were incorrect.
    fn modelitem_get(&self) -> Result<creo_sys::pro_model_item, ProErrors>;
}

impl ProSelectionInterface for creo_sys::ProSelection {
    fn modelitem_get(&self) -> Result<creo_sys::pro_model_item, ProErrors> {
        unsafe {
            let mut modelitem = std::mem::zeroed();
            let status = creo_sys::ProSelectionModelitemGet(*self, &mut modelitem);
            match status {
                creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(modelitem),
                _ => Err(status.into()),
            }
        }
    }
}

/// A collection of `ProSelection` structures.
pub struct ProSelections {
    /// The number of selections in the collection.
    pub n_sel: i32,
    /// A raw pointer to an [`creo_sys::ProArray`] of [`creo_sys::ProSelection`] structures.
    pro_selection_ptr: *mut creo_sys::ProSelection,
}

impl ProSelections {
    /// Creates a new [`ProSelections`] object from a raw [`creo_sys::ProSelection`] pointer and size
    pub fn new(selection: *mut creo_sys::ProSelection, n_sel: i32) -> ProSelections {
        ProSelections {
            n_sel,
            pro_selection_ptr: selection,
        }
    }
}

impl ops::Index<usize> for ProSelections {
    type Output = creo_sys::ProSelection;

    /// Indexes into the [`ProSelections`] object.
    /// Returns a reference to the [`creo_sys::ProSelection`] structure at the given index.
    fn index(&self, index: usize) -> &Self::Output {
        if index >= self.n_sel as usize {
            panic!("index out of bounds");
        }
        unsafe { &*self.pro_selection_ptr.offset(index as isize) }
    }
}

/// Selects an object in a Creo Parametric window.
///
/// NOTE: If the user ends the selection without actually making any,
/// ProSelect will return PRO_TK_NO_ERROR with (*p_n_sels) as zero and
/// p_sel_array as NULL.
///
/// # Arguments
///
/// * `option` - The selection filter. See the Creo Parametric TOOLKIT User's Guide for more information.
/// * `max_count` - The maximum number of selections allowed.
/// * `p_in_sel` - A `ProArray` of pointers to `ProSelection` structures (from a previous call to selection) used to initialize the array of selections. This can be NULL.
/// * `sel_func` - A pointer to a structure of filter functions. This can be NULL.
/// * `sel_env` - Use `ProSelectionEnvAlloc()` to allocate this argument. Can be NULL.
/// * `appl_act_data` - NULL in Release 18 and higher.
///
/// # Errors
///
/// * `PRO_TK_USER_ABORT` - The user chose `Quit`.
/// * `PRO_TK_PICK_ABOVE` - Selection was interrupted by a user's pick on any button from above menus.
/// * `PRO_TK_E_DEADLOCK` - The Creo Parametric user interface is not prepared to handle this request (because it is blocked by another process such as an OLE container activation).
///
/// # See Also
///
/// * `ProSelectionEnvAlloc()`
pub fn pro_select(
    option: &str,
    max_count: i32,
    p_in_sel: Option<ProSelections>,
    _sel_func: Option<creo_sys::ProSelFunctions>, // TODO: NOT IMPLEMENTED YET
    _sel_env: Option<creo_sys::ProSelectionEnv>,  // TODO: NOT IMPLEMENTED YET
    appl_act_data: Option<creo_sys::ProSelAppAction>,
) -> Result<ProSelections, i32> {
    unsafe {
        let option = std::ffi::CString::new(option).unwrap();
        let p_in_sel_ptr = p_in_sel
            .map(|s| s.pro_selection_ptr)
            .unwrap_or(std::ptr::null_mut());
        let appl_act_data_ptr = appl_act_data.unwrap_or(std::ptr::null_mut());
        let mut p_sel_array = std::ptr::null_mut();
        let mut p_n_sels = 0;
        let status = creo_sys::ProSelect(
            option.as_ptr() as *mut i8,
            max_count,
            p_in_sel_ptr,
            std::ptr::null_mut(),
            std::ptr::null_mut(),
            appl_act_data_ptr,
            &mut p_sel_array,
            &mut p_n_sels,
        );
        match status {
            creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(ProSelections::new(p_sel_array, p_n_sels)),
            _ => Err(status),
        }
    }
}

/// A wrapper around the [`creo_sys::ProSelection`] structure to manage memory allocation and deallocation.
/// Creo manages the ProSelection in case of a call to [`pro_select`], but when we need to pre-allocate
/// a [`creo_sys::ProSelection`], we need to manage the memory ourselves.
pub struct ProSelection {
    pub pro_selection_ptr: creo_sys::ProSelection,
}

impl ProSelection {
    pub fn new(
        p_cmp_path: Option<*mut creo_sys::ProAsmcomppath>,
        p_mdl_itm: Option<*mut creo_sys::ProModelitem>,
    ) -> ProSelection {
        unsafe {
            let p_cmp_path_ptr = p_cmp_path.unwrap_or(std::ptr::null_mut());
            let p_mdl_itm_ptr = p_mdl_itm.unwrap_or(std::ptr::null_mut());
            let mut pro_selection_ptr = std::mem::zeroed();
            creo_sys::ProSelectionAlloc(p_cmp_path_ptr, p_mdl_itm_ptr, &mut pro_selection_ptr);
            ProSelection { pro_selection_ptr }
        }
    }
}

impl ProSelectionInterface for ProSelection {
    fn modelitem_get(&self) -> Result<creo_sys::pro_model_item, ProErrors> {
        self.pro_selection_ptr.modelitem_get()
    }
}

impl Drop for ProSelection {
    fn drop(&mut self) {
        unsafe {
            creo_sys::ProSelectionFree(&mut self.pro_selection_ptr);
        }
    }
}

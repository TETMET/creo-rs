use creo_sys;

use crate::pro_toolkit_errors::ProErrors;

pub trait ProMdlInterface {
    /// Converts a `ProMdl` to a `ProModelitem`.
    ///
    /// # Arguments
    ///
    /// * `mdl` - The handle to a valid model.
    ///
    /// # Errors
    ///
    /// * `PRO_TK_BAD_INPUTS` - An argument is NULL.
    /// * `PRO_TK_INVALID_PTR` - The handle is invalid.
    fn to_modelitem(&self) -> Result<creo_sys::pro_model_item, ProErrors>;

    /// Displays the specified model in the current view.
    ///
    /// # Note
    /// - If no model is displayed, the specified model is placed in the base window.
    /// - If the specified model has been assigned a new window, the model will be displayed in the new window.
    ///
    /// If `model` is `None`, the function displays the object in the current window. The valid types of model that can be displayed are as follows:
    /// - `PRO_MDL_ASSEMBLY`
    /// - `PRO_MDL_PART`
    /// - `PRO_MDL_2DSECTION`
    /// - `PRO_MDL_DRAWING`
    /// - `PRO_MDL_LAYOUT`
    /// - `PRO_MDL_DWGFORM`
    /// - `PRO_MDL_MFG`
    /// - `PRO_MDL_REPORT`
    /// - `PRO_MDL_MARKUP`
    /// - `PRO_MDL_DIAGRAM`
    ///
    /// # Note
    /// Calling this function will update the display, but may not activate the UI. To achieve it, the model window should be activated (for instance, by the user).
    ///
    /// # Errors
    ///
    /// - `PRO_TK_E_NOT_FOUND` if the `model` is `None` and there is no current object.
    /// - `PRO_TK_INVALID_PTR` if the specified model is not in memory.
    /// - `PRO_TK_GENERAL_ERROR` if there was a general error and the function failed.
    /// - `PRO_TK_INVALID_TYPE` if you specified an invalid model type.
    fn display(&self) -> Result<(), ProErrors>;
}

impl ProMdlInterface for creo_sys::ProMdl {
    fn to_modelitem(&self) -> Result<creo_sys::pro_model_item, ProErrors> {
        unsafe {
            let mut modelitem = std::mem::zeroed();
            let status = creo_sys::ProMdlToModelitem(*self, &mut modelitem);
            match status {
                creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(modelitem),
                _ => Err(status.into()),
            }
        }
    }

    fn display(&self) -> Result<(), ProErrors> {
        unsafe {
            let status = creo_sys::ProMdlDisplay(*self);
            match status {
                creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(()),
                _ => Err(status.into()),
            }
        }
    }
}

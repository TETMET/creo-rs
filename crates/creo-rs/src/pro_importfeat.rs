use creo_sys;

use crate::{pro_intf_data::DataSource, pro_size_const::ProPath, pro_toolkit_errors::ProErrors};

/// Retrieves the current parameters of the specified import feature.
///
/// # Arguments
///
/// * `p_feat_handle` - The handle for the import feature.
///
/// # Output
///
/// * `p_import_feat_data` - The current parameters.
///
/// # Errors
///
/// * `PRO_TK_GENERAL_ERROR` - A problem occurred during retrieval.
/// * `PRO_TK_BAD_INPUTS` - The input argument is invalid.
pub fn pro_importfeat_data_get(
    p_feat_handle: &mut creo_sys::ProFeature,
) -> Result<creo_sys::ProImportfeatData, ProErrors> {
    unsafe {
        let mut p_import_feat_data = std::mem::zeroed();
        let status = creo_sys::ProImportfeatDataGet(p_feat_handle, &mut p_import_feat_data);
        match status {
            creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(p_import_feat_data),
            _ => Err(status.into()),
        }
    }
}

/// Creates a new import feature in the solid.
///
/// # Arguments
///
/// * `p_solid` - The pointer to the solid (part). Assembly case is not supported.
/// * `data_source` - The source of data from which to create the import feature.
/// * `p_csys` - The pointer to a reference coordinate system. If this is None, the function uses the default coordinate system.
/// * `p_attributes` - The attributes for creation of the new import feature. If this pointer is None, the function uses the default attributes.
///
/// # Errors
///
/// * `PRO_TK_BAD_INPUTS` - One or more of the input arguments are invalid.
/// * `PRO_TK_NO_LICENSE` - License check failure.
/// * `PRO_TK_INVALID_FILE` - The model file doesn't have correct format.
/// * `PRO_TK_UNSUPPORTED` - Creo Parametric does not support import of the indicated file.
pub fn pro_importfeat_create(
    p_solid: creo_sys::ProSolid,
    data_source: &DataSource,
    p_csys: Option<creo_sys::ProCsys>,
    p_attributes: Option<*mut creo_sys::ProImportfeatAttr>,
) -> Result<creo_sys::ProFeature, ProErrors> {
    unsafe {
        let mut data_source_ptr: creo_sys::ProIntfDataSource = data_source.to_ffi();
        let p_csys_ptr: creo_sys::ProCsys = p_csys.unwrap_or(std::mem::zeroed());
        let p_attributes_ptr = p_attributes.unwrap_or(std::mem::zeroed());
        let mut p_feat_handle = std::mem::zeroed();
        let status = creo_sys::ProImportfeatCreate(
            p_solid,
            &mut data_source_ptr as *mut creo_sys::ProIntfDataSource,
            p_csys_ptr,
            p_attributes_ptr,
            &mut p_feat_handle,
        );
        match status {
            creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(p_feat_handle),
            _ => Err(status.into()),
        }
    }
}

/// Creates a new import feature in the solid using profile.
///
/// Note:
/// Once a profile is set, it remains valid for the entire session unless it is reset with another profile.
///
/// Input Arguments:
/// * `p_solid` - The pointer to the solid (part). Assembly case is not supported.
/// * `data_source` - The source of data from which to create the import feature.
/// * `p_csys` - The pointer to a reference coordinate system. If this is NULL, the function uses the default coordinate system.
/// * `profile` - Profile path. Can be NULL, in which case it works identically with ProImportfeatCreate.
/// * `p_attributes` - The attributes for creation of the new import feature. If this pointer is NULL, the function uses the default attributes.
///
/// Creates a new import feature in the solid using profile.
///
/// # Errors
///
/// * `PRO_TK_BAD_INPUTS` - One or more of the input arguments are invalid.
/// * `PRO_TK_UNSUPPORTED` - Creo Parametric does not support import of the indicated file.
/// * `PRO_TK_NOT_EXIST` - No profile found.
/// * `PRO_TK_INVALID_FILE` - Profile is not readable.
/// * `PRO_TK_NO_LICENSE` - Missing the required license to perform this import feature operation.
pub fn pro_import_feature_with_profile_create(
    p_solid: creo_sys::ProSolid,
    data_source: &DataSource,
    p_csys: Option<creo_sys::ProCsys>,
    profile: &ProPath,
    p_attributes: Option<*mut creo_sys::ProImportfeatAttr>,
) -> Result<creo_sys::ProFeature, ProErrors> {
    unsafe {
        let mut data_source_ptr = data_source.to_ffi();
        let p_csys_ptr: creo_sys::ProCsys = p_csys.unwrap_or(std::mem::zeroed());
        let p_attributes_ptr = p_attributes.unwrap_or(std::mem::zeroed());
        let mut p_feat_handle = std::mem::zeroed();
        let status = creo_sys::ProImportfeatureWithProfileCreate(
            p_solid,
            &mut data_source_ptr as *mut _,
            p_csys_ptr,
            profile.path_w.as_ptr() as *mut _,
            p_attributes_ptr,
            &mut p_feat_handle,
        );
        match status {
            creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(p_feat_handle),
            _ => Err(status.into()),
        }
    }
}

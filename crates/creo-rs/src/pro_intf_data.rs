use creo_sys;

use crate::pro_size_const::ProPath;
use std::pin::Pin;

/// The type of data source for the interface
#[non_exhaustive]
#[derive(Debug)]
pub enum DataSource {
    File(DataSourceFile),
    // Memory not implemented yet
}

impl DataSource {
    /// Converts the `DataSource` to the FFI representation
    pub fn to_ffi(&self) -> creo_sys::pro_intf_data_source {
        match self {
            DataSource::File(data_source_file) => {
                let filename_ptr = data_source_file.path.path_w.as_ptr();
                creo_sys::pro_intf_data_source {
                    intf_type: data_source_file.intf_type,
                    choice: creo_sys::pro_intf_data_source__bindgen_ty_1 {
                        // Sound because we have allocated memory for the filename and it is not shared
                        filename: filename_ptr as *mut _,
                    },
                }
            }
        }
    }
}

#[derive(Debug)]
pub struct DataSourceFile {
    pub intf_type: creo_sys::pro_intf_type,
    pub path: Pin<Box<ProPath>>,
}

impl DataSourceFile {
    pub fn new(intf_type: creo_sys::pro_intf_type, path: ProPath) -> Self {
        DataSourceFile {
            intf_type,
            path: Box::pin(path),
        }
    }
}

/// Determines the type of `ProIntfDataSource` to be used, depending on the type of interface.
/// This is an auxiliary function that helps you determine what kind of data source the function `ProIntfDataSourceInit()`
/// expects you to pass in the argument `p_source`.
///
/// # Arguments
///
/// * `intf_type` - The type of interface
pub fn pro_intf_type_to_intf_data_source_type(
    intf_type: creo_sys::ProIntfType,
) -> creo_sys::ProIntfDataSourceType {
    unsafe { creo_sys::ProIntfTypeToIntfDataSourceType(intf_type as i32) }
}

/// Exports a Creo Parametric model using default export profile.
///
/// # Arguments
///
/// * `solid` - The solid model used for export.
/// * `file_type` - The type of output file to produce.
/// * `path` - The output file name to produce. Optionally, this includes
///             the path for the file location. The filename should not
///             include the file extension, this will be added automatically by Creo Parametric.
///
/// # Errors
///
/// * `PRO_TK_BAD_INPUTS` - The input arguments are invalid.
/// * `PRO_TK_NOT_IMPLEMENTED` - `file_type` interface is currently not supported.
/// * `PRO_TK_INVALID_TYPE` - Invalid `file_type`.
/// * `PRO_TK_BAD_CONTEXT` - The function is not available in Design Test Mode.
/// * `PRO_TK_NO_CHANGE` - The function did not export the source model as the up-to-date image in HDIC context already exists
///                        for the specified Creo Parametric source model.
/// * `PRO_TK_NO_LICENSE` - Missing the required license to perform this export operation.
pub fn pro_intf_3dfile_write_with_default_profile(
    solid: creo_sys::ProSolid,
    file_type: creo_sys::ProIntf3DExportType,
    path: &str,
) -> Result<(), creo_sys::ProErrors> {
    unsafe {
        let mut path = ProPath::new(path);
        let status = creo_sys::ProIntf3DFileWriteWithDefaultProfile(
            solid,
            file_type,
            path.path_w.as_mut_ptr(),
        );
        match status {
            creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(()),
            _ => Err(status),
        }
    }
}

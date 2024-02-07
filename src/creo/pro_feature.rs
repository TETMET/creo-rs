use crate::creo::creo::ffi;

/// Wrapper type for the [`ffi::ProFeatureCreateOptions`] type.
/// Used to automatically allocate and free the memory for the feature options.
pub struct ProFeatureCreateOptions {
    pub pro_feature_options_ptr: ffi::ProArray,
}

impl ProFeatureCreateOptions {
    pub fn new(options: &Vec<ffi::pro_feature_create_options>) -> ProFeatureCreateOptions {
        unsafe {
            let mut pro_feature_options_ptr: ffi::ProArray = std::mem::zeroed();
            ffi::ProArrayAlloc(
                options.len() as i32,
                std::mem::size_of::<ffi::pro_feature_create_options>() as i32,
                1,
                &mut pro_feature_options_ptr,
            );
            for (i, option) in options.iter().enumerate() {
                ffi::ProArrayObjectAdd(
                    &mut pro_feature_options_ptr,
                    i as i32,
                    1,
                    option as *const i32 as *mut std::ffi::c_void,
                );
            }
            ProFeatureCreateOptions {
                pro_feature_options_ptr,
            }
        }
    }
}

impl Drop for ProFeatureCreateOptions {
    fn drop(&mut self) {
        unsafe {
            ffi::ProArrayFree(&mut self.pro_feature_options_ptr);
        }
    }
}

pub trait ProFeatureInterface {
    fn type_get(&self) -> Result<ffi::ProFeattype, ffi::ProError>;
    fn solid_get(&self) -> Result<ffi::ProSolid, ffi::ProError>;
}

impl ProFeatureInterface for ffi::ProFeature {
    /// Retrieves the type of the specified feature.
    ///
    /// # Arguments
    ///
    /// * `p_feat_handle` - The feature handle.
    ///
    /// # Errors
    ///
    /// `ProError::PRO_TK_BAD_INPUTS` if the input argument is invalid.
    ///
    /// # See Also
    ///
    /// ProFeatType.h
    fn type_get(&self) -> Result<ffi::ProFeattype, ffi::ProError> {
        unsafe {
            let mut p_type = std::mem::zeroed();
            let mut p_feat = *self;
            let status = ffi::ProFeatureTypeGet(&mut p_feat, &mut p_type);
            if status != ffi::ProErrors_PRO_TK_NO_ERROR {
                return Err(status);
            }
            Ok(p_type)
        }
    }

    /// Retrieves the solid owner of the feature.
    fn solid_get(&self) -> Result<ffi::ProSolid, ffi::ProError> {
        unsafe {
            let mut solid = std::mem::zeroed();
            let mut p_feat = *self;
            let status = ffi::ProFeatureSolidGet(&mut p_feat, &mut solid);
            if status != ffi::ProErrors_PRO_TK_NO_ERROR {
                return Err(status);
            }
            Ok(solid)
        }
    }
}

/// Creates a feature from the element tree.
///
/// # Note
///
/// Refer to the Creo Parametric Toolkit Release Notes for more information
/// on restrictions of using this function with Multi-CAD models.
///
/// # Arguments
///
/// * `model` - A selection object representing the part or assembly
///             on which the feature is being created. The selection
///             object must be constructed using a valid assembly
///             component path to the top level assembly if:
///     - the feature contains external references to
///       geometry located outside of the solid model where it
///       will be constructed.
///     - or, the solid model in which the feature will be
///       constructed is a member of the active Creo Parametric
///       assembly.
///
/// * `elemtree` - The element tree.
/// * `options` - A ProArray array of user options.
/// * `flags` - The regeneration control flags. Pass as bitmask containing
///             one or more of the bit flags PRO_REGEN* defined in ProSolid.h
///
/// # Errors
///
/// * `PRO_TK_NO_ERROR` - The function successfully created the feature.
/// * `PRO_TK_BAD_INPUTS` - One or more of the input arguments are invalid.
/// * `PRO_TK_GENERAL_ERROR` - The feature was not created. See p_errors for
///                            the list of problems.
/// * `PRO_TK_UNSUPPORTED` - If creating embedded component feature in
///                          different owner asm or sub-asm.
pub fn pro_feature_withoptions_create(
    model: ffi::ProSelection,
    elemtree: ffi::ProElement,
    options: *mut ffi::ProFeatureCreateOptions,
    flags: i32,
) -> Result<ffi::ProFeature, ffi::ProError> {
    unsafe {
        let mut feature = std::mem::zeroed();
        let mut p_errors = std::mem::zeroed();
        let status = ffi::ProFeatureWithoptionsCreate(
            model,
            elemtree,
            options,
            flags,
            &mut feature,
            &mut p_errors,
        );
        if status != ffi::ProErrors_PRO_TK_NO_ERROR {
            return Err(status);
        }
        Ok(feature)
    }
}

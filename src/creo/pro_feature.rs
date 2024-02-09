use crate::{creo::creo::ffi, ProElement};

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
    /// Creates a copy of the feature element tree. Preferred over
    /// `ProFeatureElemtreeCreate()` since it provides options to resolve
    /// the paths of external references of the feature.
    ///
    /// # Arguments
    /// * `path`: The path used to resolve references from the element tree. Can
    ///   be `None`, but trees which lack the proper external references may
    ///   not be usable to redefine the feature with external references.
    /// * `opts`: The options used for the extraction.
    ///   Pass `PRO_FEAT_EXTRACT_NO_OPTS` for now.
    ///
    /// # Errors:
    /// * `PRO_TK_NO_ERROR`: The function successfully created the feature element
    ///   tree.
    /// * `PRO_TK_BAD_INPUTS`: The input argument is invalid.
    /// * `PRO_TK_INVALID_TYPE`: Extraction of the element tree is not supported
    ///   for this feature type.
    fn elemtree_extract(
        &self,
        path: Option<*mut ffi::ProAsmcomppath>,
        opts: ffi::ProFeatureElemtreeExtractOptions,
    ) -> Result<ProElement, ffi::ProError>;

    /// Visits all the geometry items created by the specified feature.
    ///
    /// The valid item types are as follows:
    /// - `PRO_TYPE_UNUSED` -- Visit all geometry.
    /// - `PRO_SURFACE` -- Visit surfaces only.
    /// - `PRO_EDGE` -- Visit edges only.
    /// - `PRO_QUILT` -- Visit quilts only.
    /// - `PRO_CURVE` -- Visit curves only.
    /// - `PRO_POINT` -- Visit points only.
    /// - `PRO_AXIS` -- Visit axes only.
    /// - `PRO_CSYS` -- Visit coordinate systems only.
    /// - `PRO_ANNOTATION_ELEM` -- Visit annotation elements only.
    ///
    /// # Arguments
    ///
    /// * `item_type` - The type of items to visit.
    /// * `action` - The visiting function. If it returns anything
    ///              other than `PRO_TK_NO_ERROR`, visiting stops.
    /// * `filter` - The filter function. If `None`, all items
    ///              are visited using the action function.
    ///              `ProFeatureGeomitemVisit()` will stop visiting if
    ///              `PRO_TK_GENERAL_ERROR` is returned by the filter.
    /// * `app_data` - The application data passed to the filter
    ///                and visiting functions.
    ///
    /// # Errors
    ///
    /// * `PRO_TK_NO_ERROR` - The function successfully visited all the items.
    /// * `PRO_TK_BAD_INPUTS` - One or more arguments was invalid.
    /// * `Other` - The action function returned a value other than
    ///             `PRO_TK_NO_ERROR` and visiting stopped.
    ///
    /// # See Also
    ///
    /// `ProFeatureGeomitemVisit()`
    fn geomitem_visit(
        &self,
        item_type: ffi::ProType,
        action: ffi::ProGeomitemAction,
        filter: ffi::ProGeomitemFilter,
        app_data: ffi::ProAppData,
    ) -> Result<(), ffi::ProError>;

    /// Collect all Geometry items created by the specified feature
    fn collect_geomitems(
        &self,
        item_type: ffi::ProType,
        filter: ffi::ProGeomitemFilter,
    ) -> Result<Box<Vec<ffi::ProGeomitem>>, ffi::ProError>;

    /// Retrieves the solid owner of the feature
    fn solid_get(&self) -> Result<ffi::ProSolid, ffi::ProError>;

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
    fn type_get(&self) -> Result<ffi::ProFeattype, ffi::ProError>;
}

/// Visit a ProGeomitem and collect the entity in `app_data`.
/// `app_data` is a Box to a Vec<T> where T is the entity type.
unsafe extern "C" fn visit_collect_geomitem(
    p_handle: *mut ffi::ProGeomitem,
    status: ffi::ProError,
    app_data: ffi::ProAppData,
) -> ffi::ProError {
    // We leak the box since the memory is handled by the caller
    let vec: &mut Vec<ffi::ProGeomitem> =
        Box::leak(Box::from_raw(app_data as *mut Vec<ffi::ProGeomitem>));
    if status == ffi::ProErrors_PRO_TK_NO_ERROR {
        vec.push(*p_handle);
    }
    status
}

impl ProFeatureInterface for ffi::ProFeature {
    fn elemtree_extract(
        &self,
        path: Option<*mut ffi::ProAsmcomppath>,
        opts: ffi::ProFeatureElemtreeExtractOptions,
    ) -> Result<ProElement, ffi::ProError> {
        unsafe {
            let mut elemtree = std::mem::zeroed();
            let path_ptr = match path {
                Some(p) => p,
                None => std::ptr::null_mut(),
            };
            let mut p_feat = *self;
            let status = ffi::ProFeatureElemtreeExtract(&mut p_feat, path_ptr, opts, &mut elemtree);
            if status != ffi::ProErrors_PRO_TK_NO_ERROR {
                return Err(status);
            }
            Ok(ProElement::from_ptr(elemtree, true))
        }
    }

    fn geomitem_visit(
        &self,
        item_type: ffi::ProType,
        action: ffi::ProGeomitemAction,
        filter: ffi::ProGeomitemFilter,
        app_data: ffi::ProAppData,
    ) -> Result<(), ffi::ProError> {
        unsafe {
            // Ugly hack, but the generated FFI bindings have a *mut bindings for a reference
            let refer = self as *const ffi::ProFeature as *mut ffi::ProFeature;
            let status = ffi::ProFeatureGeomitemVisit(refer, item_type, action, filter, app_data);
            if status != ffi::ProErrors_PRO_TK_NO_ERROR {
                return Err(status);
            }
            Ok(())
        }
    }

    fn collect_geomitems(
        &self,
        item_type: ffi::ProType,
        filter: ffi::ProGeomitemFilter,
    ) -> Result<Box<Vec<ffi::ProGeomitem>>, ffi::ProError> {
        let geomitems: Vec<ffi::ProGeomitem> = Vec::new();
        let app_data_ptr = Box::into_raw(Box::new(geomitems));
        self.geomitem_visit(
            item_type,
            Some(visit_collect_geomitem),
            filter,
            app_data_ptr as ffi::ProAppData,
        )?;
        unsafe { Ok(Box::from_raw(app_data_ptr)) }
    }

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

use creo_sys;

use crate::{pro_element::ProElement, pro_toolkit_errors::ProErrors};

/// Wrapper type for the [`creo_sys::ProFeatureCreateOptions`] type.
/// Used to automatically allocate and free the memory for the feature options.
pub struct ProFeatureCreateOptions {
    pub pro_feature_options_ptr: creo_sys::ProArray,
}

impl ProFeatureCreateOptions {
    pub fn new(options: &Vec<creo_sys::pro_feature_create_options>) -> ProFeatureCreateOptions {
        unsafe {
            let mut pro_feature_options_ptr: creo_sys::ProArray = std::mem::zeroed();
            creo_sys::ProArrayAlloc(
                options.len() as i32,
                std::mem::size_of::<creo_sys::pro_feature_create_options>() as i32,
                1,
                &mut pro_feature_options_ptr,
            );
            for (i, option) in options.iter().enumerate() {
                creo_sys::ProArrayObjectAdd(
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
            creo_sys::ProArrayFree(&mut self.pro_feature_options_ptr);
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
    ///
    /// * `PRO_TK_BAD_INPUTS`: The input argument is invalid.
    /// * `PRO_TK_INVALID_TYPE`: Extraction of the element tree is not supported
    ///   for this feature type.
    fn elemtree_extract(
        &self,
        path: Option<*mut creo_sys::ProAsmcomppath>,
        opts: creo_sys::ProFeatureElemtreeExtractOptions,
    ) -> Result<ProElement, ProErrors>;

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
    /// * `PRO_TK_BAD_INPUTS` - One or more arguments was invalid.
    /// * `Other` - The action function returned a value other than
    ///             `PRO_TK_NO_ERROR` and visiting stopped.
    ///
    /// # See Also
    ///
    /// `ProFeatureGeomitemVisit()`
    fn geomitem_visit(
        &self,
        item_type: creo_sys::ProType,
        action: creo_sys::ProGeomitemAction,
        filter: creo_sys::ProGeomitemFilter,
        app_data: creo_sys::ProAppData,
    ) -> Result<(), ProErrors>;

    /// Collect all Geometry items created by the specified feature
    fn collect_geomitems(
        &self,
        item_type: creo_sys::ProType,
        filter: creo_sys::ProGeomitemFilter,
    ) -> Result<Box<Vec<creo_sys::ProGeomitem>>, ProErrors>;

    /// Retrieves the solid owner of the feature
    fn solid_get(&self) -> Result<creo_sys::ProSolid, ProErrors>;

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
    fn type_get(&self) -> Result<creo_sys::ProFeattype, ProErrors>;
}

/// Visit a ProGeomitem and collect the entity in `app_data`.
/// `app_data` is a Box to a Vec<T> where T is the entity type.
unsafe extern "C" fn visit_collect_geomitem(
    p_handle: *mut creo_sys::ProGeomitem,
    status: creo_sys::ProErrors,
    app_data: creo_sys::ProAppData,
) -> creo_sys::ProErrors {
    // We leak the box since the memory is handled by the caller
    let vec: &mut Vec<creo_sys::ProGeomitem> =
        Box::leak(Box::from_raw(app_data as *mut Vec<creo_sys::ProGeomitem>));
    if status == creo_sys::ProErrors_PRO_TK_NO_ERROR {
        vec.push(*p_handle);
    }
    status
}

impl ProFeatureInterface for creo_sys::ProFeature {
    fn elemtree_extract(
        &self,
        path: Option<*mut creo_sys::ProAsmcomppath>,
        opts: creo_sys::ProFeatureElemtreeExtractOptions,
    ) -> Result<ProElement, ProErrors> {
        unsafe {
            let mut elemtree = std::mem::zeroed();
            let path_ptr = match path {
                Some(p) => p,
                None => std::ptr::null_mut(),
            };
            let mut p_feat = *self;
            let status =
                creo_sys::ProFeatureElemtreeExtract(&mut p_feat, path_ptr, opts, &mut elemtree);
            match status {
                creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(ProElement::from_ptr(elemtree, true)),
                _ => Err(status.into()),
            }
        }
    }

    fn geomitem_visit(
        &self,
        item_type: creo_sys::ProType,
        action: creo_sys::ProGeomitemAction,
        filter: creo_sys::ProGeomitemFilter,
        app_data: creo_sys::ProAppData,
    ) -> Result<(), ProErrors> {
        unsafe {
            let refer = self as *const creo_sys::ProFeature as *mut creo_sys::ProFeature;
            let status =
                creo_sys::ProFeatureGeomitemVisit(refer, item_type, action, filter, app_data);
            match status {
                creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(()),
                _ => Err(status.into()),
            }
        }
    }

    fn collect_geomitems(
        &self,
        item_type: creo_sys::ProType,
        filter: creo_sys::ProGeomitemFilter,
    ) -> Result<Box<Vec<creo_sys::ProGeomitem>>, ProErrors> {
        let geomitems: Vec<creo_sys::ProGeomitem> = Vec::new();
        let app_data_ptr = Box::into_raw(Box::new(geomitems));
        self.geomitem_visit(
            item_type,
            Some(visit_collect_geomitem),
            filter,
            app_data_ptr as creo_sys::ProAppData,
        )?;
        unsafe { Ok(Box::from_raw(app_data_ptr)) }
    }

    fn solid_get(&self) -> Result<creo_sys::ProSolid, ProErrors> {
        unsafe {
            let mut solid = std::mem::zeroed();
            let mut p_feat = *self;
            let status = creo_sys::ProFeatureSolidGet(&mut p_feat, &mut solid);
            if status != creo_sys::ProErrors_PRO_TK_NO_ERROR {
                return Err(status.into());
            }
            Ok(solid)
        }
    }

    fn type_get(&self) -> Result<creo_sys::ProFeattype, ProErrors> {
        unsafe {
            let mut p_type = std::mem::zeroed();
            let mut p_feat = *self;
            let status = creo_sys::ProFeatureTypeGet(&mut p_feat, &mut p_type);
            if status != creo_sys::ProErrors_PRO_TK_NO_ERROR {
                return Err(status.into());
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
/// * `PRO_TK_BAD_INPUTS` - One or more of the input arguments are invalid.
/// * `PRO_TK_GENERAL_ERROR` - The feature was not created. See p_errors for
///                            the list of problems.
/// * `PRO_TK_UNSUPPORTED` - If creating embedded component feature in
///                          different owner asm or sub-asm.
pub fn pro_feature_withoptions_create(
    model: creo_sys::ProSelection,
    elemtree: creo_sys::ProElement,
    options: *mut creo_sys::ProFeatureCreateOptions,
    flags: i32,
) -> Result<creo_sys::ProFeature, ProErrors> {
    unsafe {
        let mut feature = std::mem::zeroed();
        let mut p_errors = std::mem::zeroed();
        let status = creo_sys::ProFeatureWithoptionsCreate(
            model,
            elemtree,
            options,
            flags,
            &mut feature,
            &mut p_errors,
        );
        match status {
            creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(feature),
            _ => Err(status.into()),
        }
    }
}

use creo_sys;

use crate::pro_mdl::ProMdlInterface;
use crate::pro_size_const::ProMdlName;
use crate::pro_toolkit_errors::ProErrors;

pub trait ProSolidInterface {
    /// Visits all the axes in the ProSolid object
    ///
    /// # Arguments
    ///
    /// * `p_handle` - The solid handle.
    /// * `visit_action` - The visiting function. If it returns anything other than `PRO_TK_NO_ERROR`, visiting stops.
    /// * `filter_action` - The filter function. If `None`, all axes are visited using the action function.
    /// * `app_data` - The application data passed to the filter and visiting functions.
    ///
    /// # Errors
    ///
    /// * `PRO_TK_E_NOT_FOUND` - Either no axes exist, or they were all filtered out by the filter function.
    /// * `Other` - Any other value is the value returned by the action function (visiting stopped).
    fn axis_visit(
        &self,
        visit_action: creo_sys::ProAxisVisitAction,
        filter_action: creo_sys::ProAxisFilterAction,
        app_data: creo_sys::ProAppData,
    ) -> Result<(), ProErrors>;

    /// Visits all the axis in the ProSolid object and collects them
    fn collect_axis(&self) -> Result<Box<Vec<creo_sys::ProAxis>>, ProErrors>;

    fn collect_feat(&self) -> Result<Box<Vec<creo_sys::ProFeature>>, ProErrors>;

    /// Collects all surfaces in the ProSolid object
    fn collect_surfaces(&self) -> Result<Box<Vec<creo_sys::ProSurface>>, ProErrors>;

    fn feat_visit(
        &self,
        visit_action: creo_sys::ProFeatureVisitAction,
        filter_action: creo_sys::ProFeatureFilterAction,
        app_data: creo_sys::ProAppData,
    ) -> Result<(), ProErrors>;

    /// DEPRECATED: Since Creo 7.0
    /// SUCCESSORS: ProSolidBodySurfaceVisit
    /// Purpose: Visits all the surfaces in the specified solid.
    /// Note: This API will not work on solid having more than one body.
    /// Use ProSolidBodySurfaceVisit() for multi-body parts.
    ///
    /// Input Arguments:
    /// - p_handle: The solid handle.
    /// - visit_action: The visiting function. If it returns anything other
    ///   than PRO_TK_NO_ERROR, visiting stops.
    /// - filter_action: The filter function. If NULL, all surfaces are
    ///   visited using the action function.
    /// - app_data: The application data passed to the filter and
    ///   visiting functions.
    ///
    /// # Errors
    ///
    /// - PRO_TK_NO_ERROR: The function successfully visited all the surfaces.
    /// - PRO_TK_E_NOT_FOUND: Either no surfaces exist, or they were all filtered
    ///   out by the filter function.
    /// - PRO_TK_MULTIBODY_UNSUPPORTED: Model has more than 1 body.
    /// - Other: Any other value is the value returned by the action function
    ///   (visiting stopped).
    fn surface_visit(
        &self,
        visit_action: creo_sys::ProSurfaceVisitAction,
        filter_action: creo_sys::ProSurfaceFilterAction,
        app_data: creo_sys::ProAppData,
    ) -> Result<(), ProErrors>;
}

/// Visit an entity and collect the entity in `app_data`.
/// `app_data` is a Box to a Vec<T> where T is the entity type.
unsafe extern "C" fn visit_collect_entity<T>(
    entity: T,
    status: creo_sys::ProError,
    app_data: creo_sys::ProAppData,
) -> creo_sys::ProError {
    // We leak the box since the memory is handled by the caller
    let vec: &mut Vec<T> = Box::leak(Box::from_raw(app_data as *mut Vec<T>));
    if status == creo_sys::ProErrors_PRO_TK_NO_ERROR {
        vec.push(entity);
    }
    status
}

unsafe extern "C" fn visit_collect_feat(
    feat: *mut creo_sys::ProFeature,
    status: creo_sys::ProError,
    app_data: creo_sys::ProAppData,
) -> creo_sys::ProError {
    // We leak the box since the memory is handled by the caller
    let vec: &mut Vec<creo_sys::ProFeature> =
        Box::leak(Box::from_raw(app_data as *mut Vec<creo_sys::ProFeature>));
    if status == creo_sys::ProErrors_PRO_TK_NO_ERROR {
        vec.push(*feat);
    }
    status
}

impl ProSolidInterface for creo_sys::ProSolid {
    fn axis_visit(
        &self,
        visit_action: creo_sys::ProAxisVisitAction,
        filter_action: creo_sys::ProAxisFilterAction,
        app_data: creo_sys::ProAppData,
    ) -> Result<(), ProErrors> {
        unsafe {
            let status = creo_sys::ProSolidAxisVisit(*self, visit_action, filter_action, app_data);
            match status {
                creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(()),
                _ => Err(status.into()),
            }
        }
    }

    fn collect_axis(&self) -> Result<Box<Vec<creo_sys::ProAxis>>, ProErrors> {
        let app_data_ptr = Box::into_raw(Box::new(Vec::new()));
        self.axis_visit(
            Some(visit_collect_entity),
            None,
            app_data_ptr as creo_sys::ProAppData,
        )?;
        unsafe {
            // Transfer ownership to the caller
            let axis = Box::from_raw(app_data_ptr);
            Ok(axis)
        }
    }

    fn collect_surfaces(&self) -> Result<Box<Vec<creo_sys::ProSurface>>, ProErrors> {
        let app_data_ptr = Box::into_raw(Box::new(Vec::new()));
        self.surface_visit(
            Some(visit_collect_entity),
            None,
            app_data_ptr as creo_sys::ProAppData,
        )?;
        unsafe {
            // Transfer ownership to the caller
            let axis = Box::from_raw(app_data_ptr);
            Ok(axis)
        }
    }

    fn collect_feat(&self) -> Result<Box<Vec<creo_sys::ProFeature>>, ProErrors> {
        let app_data_ptr = Box::into_raw(Box::new(Vec::new()));
        self.feat_visit(
            Some(visit_collect_feat),
            None,
            app_data_ptr as creo_sys::ProAppData,
        )?;
        unsafe {
            // Transfer ownership to the caller
            let feat = Box::from_raw(app_data_ptr);
            Ok(feat)
        }
    }

    fn feat_visit(
        &self,
        visit_action: creo_sys::ProFeatureVisitAction,
        filter_action: creo_sys::ProFeatureFilterAction,
        app_data: creo_sys::ProAppData,
    ) -> Result<(), ProErrors> {
        unsafe {
            let status = creo_sys::ProSolidFeatVisit(*self, visit_action, filter_action, app_data);
            match status {
                creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(()),
                _ => Err(status.into()),
            }
        }
    }

    fn surface_visit(
        &self,
        visit_action: creo_sys::ProSurfaceVisitAction,
        filter_action: creo_sys::ProSurfaceFilterAction,
        app_data: creo_sys::ProAppData,
    ) -> Result<(), ProErrors> {
        unsafe {
            let status =
                creo_sys::ProSolidSurfaceVisit(*self, visit_action, filter_action, app_data);
            match status {
                creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(()),
                _ => Err(status.into()),
            }
        }
    }
}

// TODO: We need to implement a Macro or a use a lib to automatically generate this from the ProMdl definition
impl ProMdlInterface for creo_sys::ProSolid {
    fn to_modelitem(&self) -> Result<creo_sys::pro_model_item, ProErrors> {
        unsafe {
            let mut modelitem = std::mem::zeroed();
            let status = creo_sys::ProMdlToModelitem(*self as creo_sys::ProMdl, &mut modelitem);
            match status {
                creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(modelitem),
                _ => Err(status.into()),
            }
        }
    }

    fn display(&self) -> Result<(), ProErrors> {
        unsafe {
            let status = creo_sys::ProMdlDisplay(*self as _);
            match status {
                creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(()),
                _ => Err(status.into()),
            }
        }
    }
}

/// Creates a new Creo Parametric object of the specified type and initializes the `ProSolid` handle.
///
/// # Arguments
///
/// * `name` - The name of the new object.
/// * `type` - The solid type (`PRO_PART` or `PRO_ASSEMBLY`).
///
/// # Returns
///
/// * `PRO_TK_BAD_INPUTS` - One or more of the input arguments are invalid.
/// * `PRO_TK_E_FOUND` - An object of the specified name and type already exists.
/// * `PRO_TK_GENERAL_ERROR` - The object could not be created.
/// * `PRO_TK_LINE_TOO_LONG` - Name length of the new object is more than 31 chars.
pub fn pro_solid_mdlname_create(
    name: &str,
    _type: creo_sys::ProMdlfileType,
) -> Result<creo_sys::ProSolid, ProErrors> {
    unsafe {
        let mut name = ProMdlName::new(name);
        let mut solid_handle = std::mem::zeroed();
        let status =
            creo_sys::ProSolidMdlnameCreate(name.name_w.as_mut_ptr(), _type, &mut solid_handle);
        match status {
            creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(solid_handle),
            _ => Err(status.into()),
        }
    }
}

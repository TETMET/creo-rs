use crate::creo::creo::ffi;

use super::pro_mdl::ProMdlInterface;

pub trait ProSolidInterface {
    fn axis_visit(
        &self,
        visit_action: ffi::ProAxisVisitAction,
        filter_action: ffi::ProAxisFilterAction,
        app_data: ffi::ProAppData,
    ) -> Result<(), ffi::ProError>;
    fn collect_axis(&self) -> Result<Box<Vec<ffi::ProAxis>>, ffi::ProError>;
    fn surface_visit(
        &self,
        visit_action: ffi::ProSurfaceVisitAction,
        filter_action: ffi::ProSurfaceFilterAction,
        app_data: ffi::ProAppData,
    ) -> Result<(), ffi::ProError>;
    fn collect_surfaces(&self) -> Result<Box<Vec<ffi::ProSurface>>, ffi::ProError>;
}

/// Visit an entity in the ProSolid object and collect the entity in `app_data`.
/// `app_data` is a Box to a Vec<T> where T is the entity type.
unsafe extern "C" fn visit_collect_entity<T>(
    axis: T,
    status: ffi::ProError,
    app_data: ffi::ProAppData,
) -> ffi::ProError {
    // We leak the box since the memory is handled by the caller
    let vec: &mut Vec<T> = Box::leak(Box::from_raw(app_data as *mut Vec<T>));
    if status == ffi::ProErrors_PRO_TK_NO_ERROR {
        vec.push(axis);
    }
    status
}

impl ProSolidInterface for ffi::ProSolid {
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
    /// * `PRO_TK_NO_ERROR` - The function successfully visited all the axes.
    /// * `PRO_TK_E_NOT_FOUND` - Either no axes exist, or they were all filtered out by the filter function.
    /// * `Other` - Any other value is the value returned by the action function (visiting stopped).
    fn axis_visit(
        &self,
        visit_action: ffi::ProAxisVisitAction,
        filter_action: ffi::ProAxisFilterAction,
        app_data: ffi::ProAppData,
    ) -> Result<(), ffi::ProError> {
        unsafe {
            let status = ffi::ProSolidAxisVisit(*self, visit_action, filter_action, app_data);
            if status != ffi::ProErrors_PRO_TK_NO_ERROR {
                return Err(status);
            }
            Ok(())
        }
    }

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
        visit_action: ffi::ProSurfaceVisitAction,
        filter_action: ffi::ProSurfaceFilterAction,
        app_data: ffi::ProAppData,
    ) -> Result<(), ffi::ProError> {
        unsafe {
            let status = ffi::ProSolidSurfaceVisit(*self, visit_action, filter_action, app_data);
            if status != ffi::ProErrors_PRO_TK_NO_ERROR {
                return Err(status);
            }
            Ok(())
        }
    }

    /// Collects all the axes in the ProSolid object
    fn collect_axis(&self) -> Result<Box<Vec<ffi::ProAxis>>, ffi::ProError> {
        let app_data_ptr: *mut Vec<ffi::ProAxis> = Box::into_raw(Box::new(Vec::new()));
        self.axis_visit(
            Some(visit_collect_entity),
            None,
            app_data_ptr as ffi::ProAppData,
        )?;
        unsafe {
            // Transfer ownership to the caller
            let axis = Box::from_raw(app_data_ptr);
            Ok(axis)
        }
    }

    /// Collects all surfaces in the ProSolid object
    fn collect_surfaces(&self) -> Result<Box<Vec<ffi::ProSurface>>, ffi::ProError> {
        let app_data_ptr: *mut Vec<ffi::ProSurface> = Box::into_raw(Box::new(Vec::new()));
        self.surface_visit(
            Some(visit_collect_entity),
            None,
            app_data_ptr as ffi::ProAppData,
        )?;
        unsafe {
            // Transfer ownership to the caller
            let axis = Box::from_raw(app_data_ptr);
            Ok(axis)
        }
    }
}

impl ProMdlInterface for ffi::ProSolid {
    fn to_modelitem(&self) -> Result<ffi::pro_model_item, ffi::ProErrors> {
        unsafe {
            let mut modelitem = std::mem::zeroed();
            let status = ffi::ProMdlToModelitem(*self as ffi::ProMdl, &mut modelitem);
            if status != ffi::ProErrors_PRO_TK_NO_ERROR {
                return Err(status);
            }
            Ok(modelitem)
        }
    }
}

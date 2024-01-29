use creo_sys;

pub struct ProGeomitemdata {
    /// The pointer to the ProGeomitemdata struct.
    pub pro_geomitemdata_ptr: *mut creo_sys::ProGeomitemdata,

    /// Indicates whether the pointer is managed by this struct and should be freed when the struct is dropped.
    managed: bool,
}

impl ProGeomitemdata {
    /// Creates a new ProGeomitemdata struct.
    /// Will be cleaned when the struct is dropped.
    pub fn new(pro_geomitemdata_ptr: *mut creo_sys::ProGeomitemdata) -> ProGeomitemdata {
        ProGeomitemdata {
            pro_geomitemdata_ptr,
            managed: true,
        }
    }
}

impl Drop for ProGeomitemdata {
    fn drop(&mut self) {
        if self.managed {
            unsafe {
                creo_sys::ProGeomitemdataFree(&mut self.pro_geomitemdata_ptr);
            }
        }
    }
}

pub trait ProGeomitemInterface {
    /// Retrieves the geometric representation data of the specified
    /// geometry item.
    ///
    /// The following types of Geomitems are supported:
    /// - PRO_EDGE
    /// - PRO_EDGE_START
    /// - PRO_EDGE_END
    /// - PRO_CURVE
    /// - PRO_CRV_START
    /// - PRO_CRV_END
    /// - PRO_AXIS
    /// - PRO_SURFACE
    /// - PRO_CSYS
    ///
    /// # Errors
    ///
    /// - `PRO_TK_BAD_INPUTS`: The input argument is invalid.
    fn data_get(&self) -> Result<ProGeomitemdata, creo_sys::ProErrors>;

    /// Converts a point `ProGeomitem` handle to a `ProPoint` handle.
    ///
    /// # Errors
    ///
    /// * `PRO_TK_BAD_INPUTS` - The input geometry item handle is invalid, or does not refer to a point.
    fn to_point(&self) -> Result<creo_sys::ProPoint, creo_sys::ProErrors>;
}

impl ProGeomitemInterface for creo_sys::ProGeomitem {
    fn data_get(&self) -> Result<ProGeomitemdata, creo_sys::ProErrors> {
        unsafe {
            let mut pro_geomitemdata = std::mem::zeroed();
            let status =
                creo_sys::ProGeomitemdataGet(self as *const _ as *mut _, &mut pro_geomitemdata);
            match status {
                creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(ProGeomitemdata::new(pro_geomitemdata)),
                _ => Err(status),
            }
        }
    }

    fn to_point(&self) -> Result<creo_sys::ProPoint, creo_sys::ProErrors> {
        unsafe {
            let mut point = std::mem::zeroed();
            let status = creo_sys::ProGeomitemToPoint(self as *const _ as *mut _, &mut point);
            match status {
                creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(point),
                _ => Err(status),
            }
        }
    }
}

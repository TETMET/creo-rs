use crate::creo::creo::ffi;

pub trait ProMdlInterface {
    fn to_modelitem(&self) -> Result<ffi::pro_model_item, ffi::ProErrors>;
}

impl ProMdlInterface for ffi::ProMdl {
    fn to_modelitem(&self) -> Result<ffi::pro_model_item, ffi::ProErrors> {
        unsafe {
            let mut modelitem = std::mem::zeroed();
            let status = ffi::ProMdlToModelitem(*self, &mut modelitem);
            if status != ffi::ProErrors_PRO_TK_NO_ERROR {
                return Err(status);
            }
            Ok(modelitem)
        }
    }
}

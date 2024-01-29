use creo_sys;

#[allow(non_camel_case_types)]
#[derive(Debug, PartialEq)]
pub enum ProErrors {
    PRO_TK_NO_ERROR,
    PRO_TK_GENERAL_ERROR,
    PRO_TK_BAD_INPUTS,
    PRO_TK_USER_ABORT,
    PRO_TK_E_NOT_FOUND,
    PRO_TK_E_FOUND,
    PRO_TK_LINE_TOO_LONG,
    PRO_TK_CONTINUE,
    PRO_TK_BAD_CONTEXT,
    PRO_TK_NOT_IMPLEMENTED,
    PRO_TK_OUT_OF_MEMORY,
    PRO_TK_COMM_ERROR,
    PRO_TK_NO_CHANGE,
    PRO_TK_SUPP_PARENTS,
    PRO_TK_PICK_ABOVE,
    PRO_TK_INVALID_DIR,
    PRO_TK_INVALID_FILE,
    PRO_TK_CANT_WRITE,
    PRO_TK_INVALID_TYPE,
    PRO_TK_INVALID_PTR,
    PRO_TK_UNAV_SEC,
    PRO_TK_INVALID_MATRIX,
    PRO_TK_INVALID_NAME,
    PRO_TK_NOT_EXIST,
    PRO_TK_CANT_OPEN,
    PRO_TK_ABORT,
    PRO_TK_NOT_VALID,
    PRO_TK_INVALID_ITEM,
    PRO_TK_MSG_NOT_FOUND,
    PRO_TK_MSG_NO_TRANS,
    PRO_TK_MSG_FMT_ERROR,
    PRO_TK_MSG_USER_QUIT,
    PRO_TK_MSG_TOO_LONG,
    PRO_TK_CANT_ACCESS,
    PRO_TK_OBSOLETE_FUNC,
    PRO_TK_NO_COORD_SYSTEM,
    PRO_TK_E_AMBIGUOUS,
    PRO_TK_E_DEADLOCK,
    PRO_TK_E_BUSY,
    PRO_TK_E_IN_USE,
    PRO_TK_NO_LICENSE,
    PRO_TK_BSPL_UNSUITABLE_DEGREE,
    PRO_TK_BSPL_NON_STD_END_KNOTS,
    PRO_TK_BSPL_MULTI_INNER_KNOTS,
    PRO_TK_BAD_SRF_CRV,
    PRO_TK_EMPTY,
    PRO_TK_BAD_DIM_ATTACH,
    PRO_TK_NOT_DISPLAYED,
    PRO_TK_CANT_MODIFY,
    PRO_TK_CHECKOUT_CONFLICT,
    PRO_TK_CRE_VIEW_BAD_SHEET,
    PRO_TK_CRE_VIEW_BAD_MODEL,
    PRO_TK_CRE_VIEW_BAD_PARENT,
    PRO_TK_CRE_VIEW_BAD_TYPE,
    PRO_TK_CRE_VIEW_BAD_EXPLODE,
    PRO_TK_UNATTACHED_FEATS,
    PRO_TK_REGEN_AGAIN,
    PRO_TK_DWGCREATE_ERRORS,
    PRO_TK_UNSUPPORTED,
    PRO_TK_NO_PERMISSION,
    PRO_TK_AUTHENTICATION_FAILURE,
    PRO_TK_OUTDATED,
    PRO_TK_INCOMPLETE,
    PRO_TK_CHECK_OMITTED,
    PRO_TK_MAX_LIMIT_REACHED,
    PRO_TK_OUT_OF_RANGE,
    PRO_TK_CHECK_LAST_ERROR,
    PRO_TK_NO_PLM_LICENSE,
    PRO_TK_INCOMPLETE_TESS,
    PRO_TK_MULTIBODY_UNSUPPORTED,
    PRO_TK_BROWSER_UNAVAILABLE,
    PRO_TK_DLL_LOAD_ERROR,
    PRO_TK_APP_CREO_BARRED,
    PRO_TK_APP_TOO_OLD,
    PRO_TK_APP_BAD_DATAPATH,
    PRO_TK_APP_BAD_ENCODING,
    PRO_TK_APP_NO_LICENSE,
    PRO_TK_APP_XS_CALLBACKS,
    PRO_TK_APP_STARTUP_FAIL,
    PRO_TK_APP_INIT_FAIL,
    PRO_TK_APP_VERSION_MISMATCH,
    PRO_TK_APP_COMM_FAILURE,
    PRO_TK_APP_NEW_VERSION,
    PRO_TK_APP_UNLOCK,
    PRO_TK_APP_JLINK_NOT_ALLOWED,
    UnknownError,
}

impl From<creo_sys::ProErrors> for ProErrors {
    fn from(error_code: creo_sys::ProErrors) -> Self {
        match error_code {
            creo_sys::ProErrors_PRO_TK_NO_ERROR => ProErrors::PRO_TK_NO_ERROR,
            creo_sys::ProErrors_PRO_TK_GENERAL_ERROR => ProErrors::PRO_TK_GENERAL_ERROR,
            creo_sys::ProErrors_PRO_TK_BAD_INPUTS => ProErrors::PRO_TK_BAD_INPUTS,
            creo_sys::ProErrors_PRO_TK_USER_ABORT => ProErrors::PRO_TK_USER_ABORT,
            creo_sys::ProErrors_PRO_TK_E_NOT_FOUND => ProErrors::PRO_TK_E_NOT_FOUND,
            creo_sys::ProErrors_PRO_TK_E_FOUND => ProErrors::PRO_TK_E_FOUND,
            creo_sys::ProErrors_PRO_TK_LINE_TOO_LONG => ProErrors::PRO_TK_LINE_TOO_LONG,
            creo_sys::ProErrors_PRO_TK_CONTINUE => ProErrors::PRO_TK_CONTINUE,
            creo_sys::ProErrors_PRO_TK_BAD_CONTEXT => ProErrors::PRO_TK_BAD_CONTEXT,
            creo_sys::ProErrors_PRO_TK_NOT_IMPLEMENTED => ProErrors::PRO_TK_NOT_IMPLEMENTED,
            creo_sys::ProErrors_PRO_TK_OUT_OF_MEMORY => ProErrors::PRO_TK_OUT_OF_MEMORY,
            creo_sys::ProErrors_PRO_TK_COMM_ERROR => ProErrors::PRO_TK_COMM_ERROR,
            creo_sys::ProErrors_PRO_TK_NO_CHANGE => ProErrors::PRO_TK_NO_CHANGE,
            creo_sys::ProErrors_PRO_TK_SUPP_PARENTS => ProErrors::PRO_TK_SUPP_PARENTS,
            creo_sys::ProErrors_PRO_TK_PICK_ABOVE => ProErrors::PRO_TK_PICK_ABOVE,
            creo_sys::ProErrors_PRO_TK_INVALID_DIR => ProErrors::PRO_TK_INVALID_DIR,
            creo_sys::ProErrors_PRO_TK_INVALID_FILE => ProErrors::PRO_TK_INVALID_FILE,
            creo_sys::ProErrors_PRO_TK_CANT_WRITE => ProErrors::PRO_TK_CANT_WRITE,
            creo_sys::ProErrors_PRO_TK_INVALID_TYPE => ProErrors::PRO_TK_INVALID_TYPE,
            creo_sys::ProErrors_PRO_TK_INVALID_PTR => ProErrors::PRO_TK_INVALID_PTR,
            creo_sys::ProErrors_PRO_TK_UNAV_SEC => ProErrors::PRO_TK_UNAV_SEC,
            creo_sys::ProErrors_PRO_TK_INVALID_MATRIX => ProErrors::PRO_TK_INVALID_MATRIX,
            creo_sys::ProErrors_PRO_TK_INVALID_NAME => ProErrors::PRO_TK_INVALID_NAME,
            creo_sys::ProErrors_PRO_TK_NOT_EXIST => ProErrors::PRO_TK_NOT_EXIST,
            creo_sys::ProErrors_PRO_TK_CANT_OPEN => ProErrors::PRO_TK_CANT_OPEN,
            creo_sys::ProErrors_PRO_TK_ABORT => ProErrors::PRO_TK_ABORT,
            creo_sys::ProErrors_PRO_TK_NOT_VALID => ProErrors::PRO_TK_NOT_VALID,
            creo_sys::ProErrors_PRO_TK_INVALID_ITEM => ProErrors::PRO_TK_INVALID_ITEM,
            creo_sys::ProErrors_PRO_TK_MSG_NOT_FOUND => ProErrors::PRO_TK_MSG_NOT_FOUND,
            creo_sys::ProErrors_PRO_TK_MSG_NO_TRANS => ProErrors::PRO_TK_MSG_NO_TRANS,
            creo_sys::ProErrors_PRO_TK_MSG_FMT_ERROR => ProErrors::PRO_TK_MSG_FMT_ERROR,
            creo_sys::ProErrors_PRO_TK_MSG_USER_QUIT => ProErrors::PRO_TK_MSG_USER_QUIT,
            creo_sys::ProErrors_PRO_TK_MSG_TOO_LONG => ProErrors::PRO_TK_MSG_TOO_LONG,
            creo_sys::ProErrors_PRO_TK_CANT_ACCESS => ProErrors::PRO_TK_CANT_ACCESS,
            creo_sys::ProErrors_PRO_TK_OBSOLETE_FUNC => ProErrors::PRO_TK_OBSOLETE_FUNC,
            creo_sys::ProErrors_PRO_TK_NO_COORD_SYSTEM => ProErrors::PRO_TK_NO_COORD_SYSTEM,
            creo_sys::ProErrors_PRO_TK_E_AMBIGUOUS => ProErrors::PRO_TK_E_AMBIGUOUS,
            creo_sys::ProErrors_PRO_TK_E_DEADLOCK => ProErrors::PRO_TK_E_DEADLOCK,
            creo_sys::ProErrors_PRO_TK_E_BUSY => ProErrors::PRO_TK_E_BUSY,
            creo_sys::ProErrors_PRO_TK_E_IN_USE => ProErrors::PRO_TK_E_IN_USE,
            creo_sys::ProErrors_PRO_TK_NO_LICENSE => ProErrors::PRO_TK_NO_LICENSE,
            creo_sys::ProErrors_PRO_TK_BSPL_UNSUITABLE_DEGREE => {
                ProErrors::PRO_TK_BSPL_UNSUITABLE_DEGREE
            }
            creo_sys::ProErrors_PRO_TK_BSPL_NON_STD_END_KNOTS => {
                ProErrors::PRO_TK_BSPL_NON_STD_END_KNOTS
            }
            creo_sys::ProErrors_PRO_TK_BSPL_MULTI_INNER_KNOTS => {
                ProErrors::PRO_TK_BSPL_MULTI_INNER_KNOTS
            }
            creo_sys::ProErrors_PRO_TK_BAD_SRF_CRV => ProErrors::PRO_TK_BAD_SRF_CRV,
            creo_sys::ProErrors_PRO_TK_EMPTY => ProErrors::PRO_TK_EMPTY,
            creo_sys::ProErrors_PRO_TK_BAD_DIM_ATTACH => ProErrors::PRO_TK_BAD_DIM_ATTACH,
            creo_sys::ProErrors_PRO_TK_NOT_DISPLAYED => ProErrors::PRO_TK_NOT_DISPLAYED,
            creo_sys::ProErrors_PRO_TK_CANT_MODIFY => ProErrors::PRO_TK_CANT_MODIFY,
            creo_sys::ProErrors_PRO_TK_CHECKOUT_CONFLICT => ProErrors::PRO_TK_CHECKOUT_CONFLICT,
            creo_sys::ProErrors_PRO_TK_CRE_VIEW_BAD_SHEET => ProErrors::PRO_TK_CRE_VIEW_BAD_SHEET,
            creo_sys::ProErrors_PRO_TK_CRE_VIEW_BAD_MODEL => ProErrors::PRO_TK_CRE_VIEW_BAD_MODEL,
            creo_sys::ProErrors_PRO_TK_CRE_VIEW_BAD_PARENT => ProErrors::PRO_TK_CRE_VIEW_BAD_PARENT,
            creo_sys::ProErrors_PRO_TK_CRE_VIEW_BAD_TYPE => ProErrors::PRO_TK_CRE_VIEW_BAD_TYPE,
            creo_sys::ProErrors_PRO_TK_CRE_VIEW_BAD_EXPLODE => {
                ProErrors::PRO_TK_CRE_VIEW_BAD_EXPLODE
            }
            creo_sys::ProErrors_PRO_TK_UNATTACHED_FEATS => ProErrors::PRO_TK_UNATTACHED_FEATS,
            creo_sys::ProErrors_PRO_TK_REGEN_AGAIN => ProErrors::PRO_TK_REGEN_AGAIN,
            creo_sys::ProErrors_PRO_TK_DWGCREATE_ERRORS => ProErrors::PRO_TK_DWGCREATE_ERRORS,
            creo_sys::ProErrors_PRO_TK_UNSUPPORTED => ProErrors::PRO_TK_UNSUPPORTED,
            creo_sys::ProErrors_PRO_TK_NO_PERMISSION => ProErrors::PRO_TK_NO_PERMISSION,
            creo_sys::ProErrors_PRO_TK_AUTHENTICATION_FAILURE => {
                ProErrors::PRO_TK_AUTHENTICATION_FAILURE
            }
            creo_sys::ProErrors_PRO_TK_OUTDATED => ProErrors::PRO_TK_OUTDATED,
            creo_sys::ProErrors_PRO_TK_INCOMPLETE => ProErrors::PRO_TK_INCOMPLETE,
            creo_sys::ProErrors_PRO_TK_CHECK_OMITTED => ProErrors::PRO_TK_CHECK_OMITTED,
            creo_sys::ProErrors_PRO_TK_MAX_LIMIT_REACHED => ProErrors::PRO_TK_MAX_LIMIT_REACHED,
            creo_sys::ProErrors_PRO_TK_OUT_OF_RANGE => ProErrors::PRO_TK_OUT_OF_RANGE,
            creo_sys::ProErrors_PRO_TK_CHECK_LAST_ERROR => ProErrors::PRO_TK_CHECK_LAST_ERROR,
            creo_sys::ProErrors_PRO_TK_NO_PLM_LICENSE => ProErrors::PRO_TK_NO_PLM_LICENSE,
            creo_sys::ProErrors_PRO_TK_INCOMPLETE_TESS => ProErrors::PRO_TK_INCOMPLETE_TESS,
            creo_sys::ProErrors_PRO_TK_MULTIBODY_UNSUPPORTED => {
                ProErrors::PRO_TK_MULTIBODY_UNSUPPORTED
            }
            creo_sys::ProErrors_PRO_TK_BROWSER_UNAVAILABLE => ProErrors::PRO_TK_BROWSER_UNAVAILABLE,
            creo_sys::ProErrors_PRO_TK_DLL_LOAD_ERROR => ProErrors::PRO_TK_DLL_LOAD_ERROR,
            creo_sys::ProErrors_PRO_TK_APP_CREO_BARRED => ProErrors::PRO_TK_APP_CREO_BARRED,
            creo_sys::ProErrors_PRO_TK_APP_TOO_OLD => ProErrors::PRO_TK_APP_TOO_OLD,
            creo_sys::ProErrors_PRO_TK_APP_BAD_DATAPATH => ProErrors::PRO_TK_APP_BAD_DATAPATH,
            creo_sys::ProErrors_PRO_TK_APP_BAD_ENCODING => ProErrors::PRO_TK_APP_BAD_ENCODING,
            creo_sys::ProErrors_PRO_TK_APP_NO_LICENSE => ProErrors::PRO_TK_APP_NO_LICENSE,
            creo_sys::ProErrors_PRO_TK_APP_XS_CALLBACKS => ProErrors::PRO_TK_APP_XS_CALLBACKS,
            creo_sys::ProErrors_PRO_TK_APP_STARTUP_FAIL => ProErrors::PRO_TK_APP_STARTUP_FAIL,
            creo_sys::ProErrors_PRO_TK_APP_INIT_FAIL => ProErrors::PRO_TK_APP_INIT_FAIL,
            creo_sys::ProErrors_PRO_TK_APP_VERSION_MISMATCH => {
                ProErrors::PRO_TK_APP_VERSION_MISMATCH
            }
            creo_sys::ProErrors_PRO_TK_APP_COMM_FAILURE => ProErrors::PRO_TK_APP_COMM_FAILURE,
            creo_sys::ProErrors_PRO_TK_APP_NEW_VERSION => ProErrors::PRO_TK_APP_NEW_VERSION,
            creo_sys::ProErrors_PRO_TK_APP_UNLOCK => ProErrors::PRO_TK_APP_UNLOCK,
            creo_sys::ProErrors_PRO_TK_APP_JLINK_NOT_ALLOWED => {
                ProErrors::PRO_TK_APP_JLINK_NOT_ALLOWED
            }
            _ => ProErrors::UnknownError,
        }
    }
}

impl Into<creo_sys::ProErrors> for ProErrors {
    fn into(self) -> creo_sys::ProErrors {
        match self {
            ProErrors::PRO_TK_NO_ERROR => creo_sys::ProErrors_PRO_TK_NO_ERROR,
            ProErrors::PRO_TK_GENERAL_ERROR => creo_sys::ProErrors_PRO_TK_GENERAL_ERROR,
            ProErrors::PRO_TK_BAD_INPUTS => creo_sys::ProErrors_PRO_TK_BAD_INPUTS,
            ProErrors::PRO_TK_USER_ABORT => creo_sys::ProErrors_PRO_TK_USER_ABORT,
            ProErrors::PRO_TK_E_NOT_FOUND => creo_sys::ProErrors_PRO_TK_E_NOT_FOUND,
            ProErrors::PRO_TK_E_FOUND => creo_sys::ProErrors_PRO_TK_E_FOUND,
            ProErrors::PRO_TK_LINE_TOO_LONG => creo_sys::ProErrors_PRO_TK_LINE_TOO_LONG,
            ProErrors::PRO_TK_CONTINUE => creo_sys::ProErrors_PRO_TK_CONTINUE,
            ProErrors::PRO_TK_BAD_CONTEXT => creo_sys::ProErrors_PRO_TK_BAD_CONTEXT,
            ProErrors::PRO_TK_NOT_IMPLEMENTED => creo_sys::ProErrors_PRO_TK_NOT_IMPLEMENTED,
            ProErrors::PRO_TK_OUT_OF_MEMORY => creo_sys::ProErrors_PRO_TK_OUT_OF_MEMORY,
            ProErrors::PRO_TK_COMM_ERROR => creo_sys::ProErrors_PRO_TK_COMM_ERROR,
            ProErrors::PRO_TK_NO_CHANGE => creo_sys::ProErrors_PRO_TK_NO_CHANGE,
            ProErrors::PRO_TK_SUPP_PARENTS => creo_sys::ProErrors_PRO_TK_SUPP_PARENTS,
            ProErrors::PRO_TK_PICK_ABOVE => creo_sys::ProErrors_PRO_TK_PICK_ABOVE,
            ProErrors::PRO_TK_INVALID_DIR => creo_sys::ProErrors_PRO_TK_INVALID_DIR,
            ProErrors::PRO_TK_INVALID_FILE => creo_sys::ProErrors_PRO_TK_INVALID_FILE,
            ProErrors::PRO_TK_CANT_WRITE => creo_sys::ProErrors_PRO_TK_CANT_WRITE,
            ProErrors::PRO_TK_INVALID_TYPE => creo_sys::ProErrors_PRO_TK_INVALID_TYPE,
            ProErrors::PRO_TK_INVALID_PTR => creo_sys::ProErrors_PRO_TK_INVALID_PTR,
            ProErrors::PRO_TK_UNAV_SEC => creo_sys::ProErrors_PRO_TK_UNAV_SEC,
            ProErrors::PRO_TK_INVALID_MATRIX => creo_sys::ProErrors_PRO_TK_INVALID_MATRIX,
            ProErrors::PRO_TK_INVALID_NAME => creo_sys::ProErrors_PRO_TK_INVALID_NAME,
            ProErrors::PRO_TK_NOT_EXIST => creo_sys::ProErrors_PRO_TK_NOT_EXIST,
            ProErrors::PRO_TK_CANT_OPEN => creo_sys::ProErrors_PRO_TK_CANT_OPEN,
            ProErrors::PRO_TK_ABORT => creo_sys::ProErrors_PRO_TK_ABORT,
            ProErrors::PRO_TK_NOT_VALID => creo_sys::ProErrors_PRO_TK_NOT_VALID,
            ProErrors::PRO_TK_INVALID_ITEM => creo_sys::ProErrors_PRO_TK_INVALID_ITEM,
            ProErrors::PRO_TK_MSG_NOT_FOUND => creo_sys::ProErrors_PRO_TK_MSG_NOT_FOUND,
            ProErrors::PRO_TK_MSG_NO_TRANS => creo_sys::ProErrors_PRO_TK_MSG_NO_TRANS,
            ProErrors::PRO_TK_MSG_FMT_ERROR => creo_sys::ProErrors_PRO_TK_MSG_FMT_ERROR,
            ProErrors::PRO_TK_MSG_USER_QUIT => creo_sys::ProErrors_PRO_TK_MSG_USER_QUIT,
            ProErrors::PRO_TK_MSG_TOO_LONG => creo_sys::ProErrors_PRO_TK_MSG_TOO_LONG,
            ProErrors::PRO_TK_CANT_ACCESS => creo_sys::ProErrors_PRO_TK_CANT_ACCESS,
            ProErrors::PRO_TK_OBSOLETE_FUNC => creo_sys::ProErrors_PRO_TK_OBSOLETE_FUNC,
            ProErrors::PRO_TK_NO_COORD_SYSTEM => creo_sys::ProErrors_PRO_TK_NO_COORD_SYSTEM,
            ProErrors::PRO_TK_E_AMBIGUOUS => creo_sys::ProErrors_PRO_TK_E_AMBIGUOUS,
            ProErrors::PRO_TK_E_DEADLOCK => creo_sys::ProErrors_PRO_TK_E_DEADLOCK,
            ProErrors::PRO_TK_E_BUSY => creo_sys::ProErrors_PRO_TK_E_BUSY,
            ProErrors::PRO_TK_E_IN_USE => creo_sys::ProErrors_PRO_TK_E_IN_USE,
            ProErrors::PRO_TK_NO_LICENSE => creo_sys::ProErrors_PRO_TK_NO_LICENSE,
            ProErrors::PRO_TK_BSPL_UNSUITABLE_DEGREE => {
                creo_sys::ProErrors_PRO_TK_BSPL_UNSUITABLE_DEGREE
            }
            ProErrors::PRO_TK_BSPL_NON_STD_END_KNOTS => {
                creo_sys::ProErrors_PRO_TK_BSPL_NON_STD_END_KNOTS
            }
            ProErrors::PRO_TK_BSPL_MULTI_INNER_KNOTS => {
                creo_sys::ProErrors_PRO_TK_BSPL_MULTI_INNER_KNOTS
            }
            ProErrors::PRO_TK_BAD_SRF_CRV => creo_sys::ProErrors_PRO_TK_BAD_SRF_CRV,
            ProErrors::PRO_TK_EMPTY => creo_sys::ProErrors_PRO_TK_EMPTY,
            ProErrors::PRO_TK_BAD_DIM_ATTACH => creo_sys::ProErrors_PRO_TK_BAD_DIM_ATTACH,
            ProErrors::PRO_TK_NOT_DISPLAYED => creo_sys::ProErrors_PRO_TK_NOT_DISPLAYED,
            ProErrors::PRO_TK_CANT_MODIFY => creo_sys::ProErrors_PRO_TK_CANT_MODIFY,
            ProErrors::PRO_TK_CHECKOUT_CONFLICT => creo_sys::ProErrors_PRO_TK_CHECKOUT_CONFLICT,
            ProErrors::PRO_TK_CRE_VIEW_BAD_SHEET => creo_sys::ProErrors_PRO_TK_CRE_VIEW_BAD_SHEET,
            ProErrors::PRO_TK_CRE_VIEW_BAD_MODEL => creo_sys::ProErrors_PRO_TK_CRE_VIEW_BAD_MODEL,
            ProErrors::PRO_TK_CRE_VIEW_BAD_PARENT => creo_sys::ProErrors_PRO_TK_CRE_VIEW_BAD_PARENT,
            ProErrors::PRO_TK_CRE_VIEW_BAD_TYPE => creo_sys::ProErrors_PRO_TK_CRE_VIEW_BAD_TYPE,
            ProErrors::PRO_TK_CRE_VIEW_BAD_EXPLODE => {
                creo_sys::ProErrors_PRO_TK_CRE_VIEW_BAD_EXPLODE
            }
            ProErrors::PRO_TK_UNATTACHED_FEATS => creo_sys::ProErrors_PRO_TK_UNATTACHED_FEATS,
            ProErrors::PRO_TK_REGEN_AGAIN => creo_sys::ProErrors_PRO_TK_REGEN_AGAIN,
            ProErrors::PRO_TK_DWGCREATE_ERRORS => creo_sys::ProErrors_PRO_TK_DWGCREATE_ERRORS,
            ProErrors::PRO_TK_UNSUPPORTED => creo_sys::ProErrors_PRO_TK_UNSUPPORTED,
            ProErrors::PRO_TK_NO_PERMISSION => creo_sys::ProErrors_PRO_TK_NO_PERMISSION,
            ProErrors::PRO_TK_AUTHENTICATION_FAILURE => {
                creo_sys::ProErrors_PRO_TK_AUTHENTICATION_FAILURE
            }
            ProErrors::PRO_TK_OUTDATED => creo_sys::ProErrors_PRO_TK_OUTDATED,
            ProErrors::PRO_TK_INCOMPLETE => creo_sys::ProErrors_PRO_TK_INCOMPLETE,
            ProErrors::PRO_TK_CHECK_OMITTED => creo_sys::ProErrors_PRO_TK_CHECK_OMITTED,
            ProErrors::PRO_TK_MAX_LIMIT_REACHED => creo_sys::ProErrors_PRO_TK_MAX_LIMIT_REACHED,
            ProErrors::PRO_TK_OUT_OF_RANGE => creo_sys::ProErrors_PRO_TK_OUT_OF_RANGE,
            ProErrors::PRO_TK_CHECK_LAST_ERROR => creo_sys::ProErrors_PRO_TK_CHECK_LAST_ERROR,
            ProErrors::PRO_TK_NO_PLM_LICENSE => creo_sys::ProErrors_PRO_TK_NO_PLM_LICENSE,
            ProErrors::PRO_TK_INCOMPLETE_TESS => creo_sys::ProErrors_PRO_TK_INCOMPLETE_TESS,
            ProErrors::PRO_TK_MULTIBODY_UNSUPPORTED => {
                creo_sys::ProErrors_PRO_TK_MULTIBODY_UNSUPPORTED
            }
            ProErrors::PRO_TK_BROWSER_UNAVAILABLE => creo_sys::ProErrors_PRO_TK_BROWSER_UNAVAILABLE,
            ProErrors::PRO_TK_DLL_LOAD_ERROR => creo_sys::ProErrors_PRO_TK_DLL_LOAD_ERROR,
            ProErrors::PRO_TK_APP_CREO_BARRED => creo_sys::ProErrors_PRO_TK_APP_CREO_BARRED,
            ProErrors::PRO_TK_APP_TOO_OLD => creo_sys::ProErrors_PRO_TK_APP_TOO_OLD,
            ProErrors::PRO_TK_APP_BAD_DATAPATH => creo_sys::ProErrors_PRO_TK_APP_BAD_DATAPATH,
            ProErrors::PRO_TK_APP_BAD_ENCODING => creo_sys::ProErrors_PRO_TK_APP_BAD_ENCODING,
            ProErrors::PRO_TK_APP_NO_LICENSE => creo_sys::ProErrors_PRO_TK_APP_NO_LICENSE,
            ProErrors::PRO_TK_APP_XS_CALLBACKS => creo_sys::ProErrors_PRO_TK_APP_XS_CALLBACKS,
            ProErrors::PRO_TK_APP_STARTUP_FAIL => creo_sys::ProErrors_PRO_TK_APP_STARTUP_FAIL,
            ProErrors::PRO_TK_APP_INIT_FAIL => creo_sys::ProErrors_PRO_TK_APP_INIT_FAIL,
            ProErrors::PRO_TK_APP_VERSION_MISMATCH => {
                creo_sys::ProErrors_PRO_TK_APP_VERSION_MISMATCH
            }
            ProErrors::PRO_TK_APP_COMM_FAILURE => creo_sys::ProErrors_PRO_TK_APP_COMM_FAILURE,
            ProErrors::PRO_TK_APP_NEW_VERSION => creo_sys::ProErrors_PRO_TK_APP_NEW_VERSION,
            ProErrors::PRO_TK_APP_UNLOCK => creo_sys::ProErrors_PRO_TK_APP_UNLOCK,
            ProErrors::PRO_TK_APP_JLINK_NOT_ALLOWED => {
                creo_sys::ProErrors_PRO_TK_APP_JLINK_NOT_ALLOWED
            }
            ProErrors::UnknownError => -1,
        }
    }
}

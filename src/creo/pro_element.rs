use std::{ffi::OsString, os::windows::ffi::OsStrExt};

use crate::{creo::creo::ffi, ProReference};

pub struct ProElement {
    pub pro_element_ptr: ffi::ProElement,
    root: bool,
}

impl ProElement {
    /// Creates a new ProElement and manages the memory.
    /// The element is freed when it goes out of scope.
    pub fn new(name_id: u32) -> ProElement {
        unsafe {
            let mut pro_element_ptr: ffi::ProElement = std::mem::zeroed();
            ffi::ProElementAlloc(name_id as i32, &mut pro_element_ptr);
            ProElement {
                pro_element_ptr,
                root: true,
            }
        }
    }

    /// Assigns the number of decimals places to be used for a double value in the feature.
    /// This value will be used in the part dimension related to this element.
    ///
    /// # Notes
    /// - Creo Parametric has a limit of 0-9 to nine decimal places for feature dimensions.
    /// - This function should be used before `ProElementDoubleSet` to ensure that the value is assigned with the correct intended number of decimal places.
    /// - Relations referring to the related dimension will not be affected by the decimal places setting.
    /// - This will have no effect on the related dimension if it is created as fractional.
    ///
    /// # Arguments
    /// - `decimals` - The number of decimal places to be used.
    ///
    /// # Errors
    /// - `PRO_TK_NO_ERROR` - The function succeeded.
    /// - `PRO_TK_BAD_INPUTS` - One or more input arguments was invalid.
    /// - `PRO_TK_INVALID_TYPE` - The element is not a double element.
    /// - `PRO_TK_CANT_WRITE` - The element is relation driven.
    ///
    /// # See Also
    /// - `ProElementDoubleGet()`
    /// - `ProElementDoubleSet()`
    /// - `ProElementDecimalsGet()`
    pub fn decimals_set(&self, decimals: i32) -> Result<(), ffi::ProError> {
        unsafe {
            let status = ffi::ProElementDecimalsSet(self.pro_element_ptr, decimals);
            if status != ffi::ProErrors_PRO_TK_NO_ERROR {
                return Err(status);
            }
        }
        Ok(())
    }

    /// Sets the double value for the element.
    ///
    /// # Note
    /// Before using this function, use `decimals_set()` to assign the number of decimal places
    /// to be used for the double value.
    ///
    /// # Arguments
    /// - `value` - The double value.
    ///
    /// # Errors
    /// - `PRO_TK_NO_ERROR` - The function succeeded.
    /// - `PRO_TK_BAD_INPUTS` - One or more arguments is invalid.
    /// - `PRO_TK_INVALID_TYPE` - The element does not allow this type of value.
    pub fn double_set(&self, value: f64) -> Result<(), ffi::ProError> {
        unsafe {
            let status = ffi::ProElementDoubleSet(self.pro_element_ptr, value);
            if status != ffi::ProErrors_PRO_TK_NO_ERROR {
                return Err(status);
            }
        }
        Ok(())
    }

    /// Retrieves the specified element identifier.
    ///
    /// # Arguments
    /// - `elem` - The `ProElement` object.
    ///
    /// # Errors
    /// - `PRO_TK_NO_ERROR` - The function succeeded.
    /// - `PRO_TK_BAD_INPUTS` - The input argument is invalid.
    pub fn id_get(&self) -> Result<ffi::ProElemId, ffi::ProError> {
        unsafe {
            let mut id: ffi::ProElemId = std::mem::zeroed();
            let status = ffi::ProElementIdGet(self.pro_element_ptr,  &mut id);
            if status != ffi::ProErrors_PRO_TK_NO_ERROR {
                return Err(status);
            }
            Ok(id)
        }
    }

    /// Adds the element directly on the element tree.
    ///
    /// # Errors
    ///
    /// * `PRO_TK_NO_ERROR` - The element was successfully added.
    /// * `PRO_TK_BAD_INPUTS` - The input argument is invalid.
    pub fn elementree_add(&self, element: &mut ProElement) -> Result<(), ffi::ProError> {
        unsafe {
            let status = ffi::ProElemtreeElementAdd(
                self.pro_element_ptr,
                std::ptr::null_mut(), // TODO: implement elem_path
                element.pro_element_ptr,
            );
            if status != ffi::ProErrors_PRO_TK_NO_ERROR {
                return Err(status);
            }
            element.root = false;
        }
        Ok(())
    }

    /// Sets the integer value for the element.
    ///
    /// # Arguments
    ///
    /// * `element` - The element.
    /// * `value` - The integer value.
    ///
    /// # Returns
    ///
    /// * `PRO_TK_NO_ERROR` - The function succeeded.
    /// * `PRO_TK_BAD_INPUTS` - One or more arguments is invalid.
    /// * `PRO_TK_INVALID_TYPE` - The element does not allow this type of value.
    pub fn integer_set(&self, value: i32) -> Result<(), ffi::ProError> {
        unsafe {
            let status = ffi::ProElementIntegerSet(self.pro_element_ptr, value);
            if status != ffi::ProErrors_PRO_TK_NO_ERROR {
                return Err(status);
            }
        }
        Ok(())
    }

    /// Sets the reference value for the element.
    ///
    /// # Arguments
    ///
    /// * `reference` - The reference value.
    ///
    /// # Errors
    ///
    /// * `PRO_TK_NO_ERROR` - The function succeeded.
    /// * `PRO_TK_BAD_INPUTS` - One or more arguments is invalid.
    /// * `PRO_TK_INVALID_TYPE` - The element does not allow this type of value.
    pub fn reference_set(&self, reference: ProReference) -> Result<(), ffi::ProError> {
        unsafe {
            let status =
                ffi::ProElementReferenceSet(self.pro_element_ptr, reference.pro_reference_ptr);
            if status != ffi::ProErrors_PRO_TK_NO_ERROR {
                return Err(status);
            }
        }
        Ok(())
    }

    /// Sets the string value for the element.
    ///
    /// # Arguments
    ///
    /// * `value` - The wide-string value.
    ///
    /// # Errors
    ///
    /// * `PRO_TK_NO_ERROR` - The function succeeded.
    /// * `PRO_TK_BAD_INPUTS` - One or more arguments is invalid.
    /// * `PRO_TK_INVALID_TYPE` - The element does not allow this type of value.
    /// * `PRO_TK_CANT_WRITE` - The element is relation driven.
    pub fn w_string_set(&self, value: &str) -> Result<(), ffi::ProError> {
        unsafe {
            let mut wvalue = OsString::from(value).encode_wide().collect::<Vec<_>>();
            wvalue.push(0);
            let status = ffi::ProElementWstringSet(self.pro_element_ptr, wvalue.as_mut_ptr());
            if status != ffi::ProErrors_PRO_TK_NO_ERROR {
                return Err(status);
            }
            Ok(())
        }
    }
}

/// Executing ProElementFree will free the memory for all children
/// Therefore we keep track of the root node and only free that one when it is out of scope
impl Drop for ProElement {
    fn drop(&mut self) {
        unsafe {
            if self.root {
                ffi::ProElementFree(&mut self.pro_element_ptr);
            }
        }
    }
}

pub struct ProElementBuilder<'a> {
    name_id: u32,
    decimals: Option<i32>,
    double: Option<f64>,
    elements: Vec<&'a mut ProElement>,
    integer: Option<i32>,
    reference: Option<ProReference>,
    w_string: Option<String>,
}

impl<'a> ProElementBuilder<'a> {
    pub fn new(name_id: u32) -> ProElementBuilder<'a> {
        ProElementBuilder {
            name_id,
            decimals: None,
            double: None,
            elements: Vec::new(),
            integer: None,
            reference: None,
            w_string: None,
        }
    }

    pub fn with_decimals(mut self, decimals: i32) -> ProElementBuilder<'a> {
        self.decimals = Some(decimals);
        self
    }

    pub fn with_double(mut self, double: f64) -> ProElementBuilder<'a> {
        self.double = Some(double);
        self
    }

    pub fn with_element(mut self, element: &'a mut ProElement) -> ProElementBuilder {
        self.elements.push(element);
        self
    }

    pub fn with_integer(mut self, integer: i32) -> ProElementBuilder<'a> {
        self.integer = Some(integer);
        self
    }

    pub fn with_reference(mut self, reference: ProReference) -> ProElementBuilder<'a> {
        self.reference = Some(reference);
        self
    }

    pub fn with_w_string(mut self, w_string: &str) -> ProElementBuilder<'a> {
        self.w_string = Some(w_string.to_string());
        self
    }

    pub fn build(self) -> Result<ProElement, ffi::ProError> {
        let element = ProElement::new(self.name_id);
        if let Some(decimals) = self.decimals {
            element.decimals_set(decimals)?;
        }
        if let Some(double) = self.double {
            element.double_set(double)?;
        }
        for mut child in self.elements {
            element.elementree_add(&mut child)?;
        }
        if let Some(integer) = self.integer {
            element.integer_set(integer)?;
        }
        if let Some(reference) = self.reference {
            element.reference_set(reference)?;
        }
        if let Some(w_string) = self.w_string {
            element.w_string_set(&w_string)?;
        }
        Ok(element)
    }
}

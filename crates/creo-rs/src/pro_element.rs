use crate::{pro_reference::ProReference, pro_toolkit_errors::ProErrors};
use creo_sys;
use std::{
    ffi::OsString,
    os::windows::ffi::{OsStrExt, OsStringExt},
};

pub trait ProElementInterface {
    /// Retrieves the children of the element specified by the `elempath` argument.
    ///
    /// # Arguments
    /// * `elempath` - The path to the element.
    ///
    /// # Errors
    ///
    /// * `PRO_TK_BAD_INPUTS` - One or more of the input arguments are invalid.
    fn children_get(
        &self,
        elempath: Option<creo_sys::ProElempath>,
    ) -> Result<Vec<creo_sys::ProElement>, ProErrors>;

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
    /// * `PRO_TK_NO_ERROR` - The function succeeded.
    /// * `PRO_TK_BAD_INPUTS` - One or more input arguments was invalid.
    /// * `PRO_TK_INVALID_TYPE` - The element is not a double element.
    /// * `PRO_TK_CANT_WRITE` - The element is relation driven.
    fn decimals_set(&self, decimals: i32) -> Result<(), ProErrors>;

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
    /// * `PRO_TK_NO_ERROR` - The function succeeded.
    /// * `PRO_TK_BAD_INPUTS` - One or more arguments is invalid.
    /// * `PRO_TK_INVALID_TYPE` - The element does not allow this type of value.
    fn double_set(&self, value: f64) -> Result<(), ProErrors>;

    /// Adds the element directly on the element tree.
    ///
    /// # Errors
    ///
    /// * `PRO_TK_NO_ERROR` - The element was successfully added.
    /// * `PRO_TK_BAD_INPUTS` - The input argument is invalid.
    fn elementree_add(&self, element: &mut ProElement) -> Result<(), ProErrors>;

    /// Recursively visits the subelements of the specified element in the element tree.
    /// Note that the element must be an array, and the element path cannot contain an index to visit subelements for each index.
    ///
    /// The visit function is called if the filter function is `PRO_TK_NO_FUNCTION`, or if it returns an error code other than `PRO_TK_NO_ERROR`.
    /// Traversal continues until all the elements have been visited, or if the visit function returns an error code other than `PRO_TK_NO_ERROR`,
    /// or filter function returns an error code other than `PRO_TK_NO_ERROR` and `PRO_TK_CONTINUE`.
    ///
    /// # Arguments
    ///
    /// * `elempath` - The path to the element.
    /// * `filterfunc` - The filter function. If `None`, all elements are visited using the action function.
    /// * `visitfunc` - The visit function.
    /// * `appdata` - The user data passed to the visit and filter functions.
    ///
    /// # Errors
    ///
    /// * `PRO_TK_BAD_INPUTS` - One or more of the arguments are invalid.
    /// * `PRO_TK_E_NOT_FOUND` - Visit function was never called.
    /// * Other - Any other value is the error code returned by the visit or filter function (visiting stopped).
    fn elemtree_elements_visit(
        &self,
        elempath: creo_sys::ProElempath,
        filterfunc: creo_sys::ProElemtreeVisitFilter,
        visitfunc: creo_sys::ProElemtreeVisitAction,
        appdata: creo_sys::ProAppData,
    ) -> Result<(), ProErrors>;

    /// Collect all the elements in the element tree.
    fn collect_elements(
        &self,
        elempath: Option<creo_sys::ProElempath>,
        filterfunc: creo_sys::ProElemtreeVisitFilter,
    ) -> Result<Box<Vec<creo_sys::ProElement>>, ProErrors>;

    /// Retrieves the specified element identifier.
    ///
    /// # Arguments
    /// - `elem` - The `ProElement` object.
    ///
    /// # Errors
    ///
    /// * `PRO_TK_BAD_INPUTS` - The input argument is invalid.
    fn id_get(&self) -> Result<creo_sys::ProElemId, ProErrors>;

    /// Sets the integer value for the element.
    ///
    /// # Arguments
    ///
    /// * `element` - The element.
    /// * `value` - The integer value.
    ///
    /// # Errors
    ///
    /// * `PRO_TK_NO_ERROR` - The function succeeded.
    /// * `PRO_TK_BAD_INPUTS` - One or more arguments is invalid.
    /// * `PRO_TK_INVALID_TYPE` - The element does not allow this type of value.
    fn integer_set(&self, value: i32) -> Result<(), ProErrors>;

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
    fn reference_set(&self, reference: ProReference) -> Result<(), ProErrors>;

    /// Returns a single reference value for the element.
    ///
    /// # Errors
    ///
    /// * `PRO_TK_NO_ERROR` - The function succeeded.
    /// * `PRO_TK_BAD_INPUTS` - One or more arguments is invalid.
    /// * `PRO_TK_EMPTY` - The element does not contain reference.
    /// * `PRO_TK_INVALID_TYPE` - The element does not contain a value that can be represented as a single reference element.
    fn reference_get(&self) -> Result<creo_sys::ProReference, ProErrors>;

    /// Returns the nominal value type for the element.
    ///
    /// # Errors
    ///
    /// * `PRO_TK_NO_ERROR` - The function succeeded.
    /// * `PRO_TK_BAD_INPUTS` - One or more arguments is invalid.
    /// * `PRO_TK_EMPTY` - The element does not have a value yet.
    /// * `PRO_TK_INVALID_TYPE` - The element is compound or array, use `ProElementIsCompound` or `ProElementIsArray`.
    fn valuetype_get(&self) -> Result<creo_sys::ProValueDataType, ProErrors>;

    /// Returns a string value representation for the element.
    ///
    /// # Note
    ///
    /// When called on an element driven by relations, the function will return
    /// the first appropriate relations string involving this element value.
    /// The relation string should be used only to identify that the element is
    /// driven by relations, and should not be used to calculate the element
    /// value. This is because the element value may be driven by more than
    /// one relation simultaneously in the feature's relation set.
    /// Refer to ProRelSet.h for more details about relations and relation sets.
    ///
    /// # Arguments
    ///
    /// * `options` - Options for how the string should be obtained. Can be `None`.
    ///
    /// # Errors
    ///
    /// * `Ok(value)` - The wide-string value. Free this string using `ProWstringFree()`.
    /// * `Err(PRO_TK_NO_ERROR)` - The function succeeded.
    /// * `Err(PRO_TK_BAD_INPUTS)` - One or more arguments is invalid.
    /// * `Err(PRO_TK_INVALID_TYPE)` - The element does not contain a value that can be represented as this type.
    fn w_string_get(&self) -> Result<String, ProErrors>;

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
    fn w_string_set(&self, value: &str) -> Result<(), ProErrors>;
}

/// Visit a creo_sys::ProElement and collect the entity in `app_data`.
/// Also traverse all children of the element.
unsafe extern "C" fn visit_collect_element(
    _elem_tree: creo_sys::ProElement,
    elem: creo_sys::ProElement,
    _elem_path: creo_sys::ProElempath,
    appdata: creo_sys::ProAppData,
) -> creo_sys::ProError {
    // We leak the box since the memory is handled by the caller
    let vec: &mut Vec<creo_sys::ProElement> =
        Box::leak(Box::from_raw(appdata as *mut Vec<creo_sys::ProElement>));
    vec.push(elem);
    creo_sys::ProErrors_PRO_TK_NO_ERROR
}

impl ProElementInterface for creo_sys::ProElement {
    fn children_get(
        &self,
        elempath: Option<creo_sys::ProElempath>,
    ) -> Result<Vec<creo_sys::ProElement>, ProErrors> {
        let mut children: Vec<creo_sys::ProElement> = Vec::new();
        unsafe {
            // We allocate the ProArray and store the children,
            // once fetched we can convert the ProArray to a Vec<ProElement> and free the ProArray
            let mut pro_array_children_ptr: creo_sys::ProArray = std::mem::zeroed();
            creo_sys::ProArrayAlloc(
                1,
                std::mem::size_of::<creo_sys::ProElement>() as i32,
                1,
                &mut pro_array_children_ptr,
            );
            let status = creo_sys::ProElementChildrenGet(
                *self,
                elempath.unwrap_or(std::ptr::null_mut()),
                pro_array_children_ptr as *mut *mut creo_sys::ProElement,
            );
            if status != creo_sys::ProErrors_PRO_TK_NO_ERROR {
                return Err(status.into());
            }
            let mut n_elems = 0;
            creo_sys::ProArraySizeGet(pro_array_children_ptr, &mut n_elems);
            // Now copy all creo_sys::ProElements so that it is now owned by Rust
            for i in 0..n_elems {
                let elem = pro_array_children_ptr.offset(i as isize);
                children.push(elem as creo_sys::ProElement);
            }
            // Free the ProArray
            creo_sys::ProArrayFree(pro_array_children_ptr as *mut creo_sys::ProArray);
            Ok(children)
        }
    }

    fn decimals_set(&self, decimals: i32) -> Result<(), ProErrors> {
        unsafe {
            let status = creo_sys::ProElementDecimalsSet(*self, decimals);
            match status {
                creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(()),
                _ => Err(status.into()),
            }
        }
    }

    fn double_set(&self, value: f64) -> Result<(), ProErrors> {
        unsafe {
            let status = creo_sys::ProElementDoubleSet(*self, value);
            match status {
                creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(()),
                _ => Err(status.into()),
            }
        }
    }

    fn id_get(&self) -> Result<creo_sys::ProElemId, ProErrors> {
        unsafe {
            let mut id: creo_sys::ProElemId = std::mem::zeroed();
            let status = creo_sys::ProElementIdGet(*self, &mut id);
            if status != creo_sys::ProErrors_PRO_TK_NO_ERROR {
                return Err(status.into());
            }
            Ok(id)
        }
    }

    fn elementree_add(&self, element: &mut ProElement) -> Result<(), ProErrors> {
        unsafe {
            let status = creo_sys::ProElemtreeElementAdd(
                *self,
                std::ptr::null_mut(), // TODO: implement elem_path
                element.pro_element_ptr,
            );
            if status != creo_sys::ProErrors_PRO_TK_NO_ERROR {
                return Err(status.into());
            }
            element.root = false;
        }
        Ok(())
    }

    fn elemtree_elements_visit(
        &self,
        elempath: creo_sys::ProElempath,
        filterfunc: creo_sys::ProElemtreeVisitFilter,
        visitfunc: creo_sys::ProElemtreeVisitAction,
        appdata: creo_sys::ProAppData,
    ) -> Result<(), ProErrors> {
        unsafe {
            let status =
                creo_sys::ProElemtreeElementVisit(*self, elempath, filterfunc, visitfunc, appdata);
            match status {
                creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(()),
                _ => Err(status.into()),
            }
        }
    }

    fn collect_elements(
        &self,
        elempath: Option<creo_sys::ProElempath>,
        filterfunc: creo_sys::ProElemtreeVisitFilter,
    ) -> Result<Box<Vec<creo_sys::ProElement>>, ProErrors> {
        let elements: Vec<creo_sys::ProElement> = Vec::new();
        unsafe {
            let app_data_ptr = Box::into_raw(Box::new(elements));
            let elempath_ptr = elempath.unwrap_or(std::ptr::null_mut());
            self.elemtree_elements_visit(
                elempath_ptr,
                filterfunc,
                Some(visit_collect_element),
                app_data_ptr as creo_sys::ProAppData,
            )?;
            // Transfer ownership to the caller
            let elements = Box::from_raw(app_data_ptr);
            Ok(elements)
        }
    }

    fn integer_set(&self, value: i32) -> Result<(), ProErrors> {
        unsafe {
            let status = creo_sys::ProElementIntegerSet(*self, value);
            match status {
                creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(()),
                _ => Err(status.into()),
            }
        }
    }

    fn reference_set(&self, reference: ProReference) -> Result<(), ProErrors> {
        unsafe {
            let status = creo_sys::ProElementReferenceSet(*self, reference.pro_reference_ptr);
            if status != creo_sys::ProErrors_PRO_TK_NO_ERROR {
                return Err(status.into());
            }
        }
        Ok(())
    }

    fn reference_get(&self) -> Result<creo_sys::ProReference, ProErrors> {
        unsafe {
            let mut reference: creo_sys::ProReference = std::mem::zeroed();
            let status =
                creo_sys::ProElementReferenceGet(*self, std::ptr::null_mut(), &mut reference);
            match status {
                creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(reference),
                _ => Err(status.into()),
            }
        }
    }

    fn valuetype_get(&self) -> Result<creo_sys::ProValueDataType, ProErrors> {
        unsafe {
            let mut valuetype: creo_sys::ProValueDataType = std::mem::zeroed();
            let status = creo_sys::ProElementValuetypeGet(*self, &mut valuetype);
            match status {
                creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(valuetype),
                _ => Err(status.into()),
            }
        }
    }

    fn w_string_get(&self) -> Result<String, ProErrors> {
        unsafe {
            let mut wvalue: *mut u16 = std::ptr::null_mut();
            let status = creo_sys::ProElementWstringGet(*self, std::ptr::null_mut(), &mut wvalue);
            if status != creo_sys::ProErrors_PRO_TK_NO_ERROR {
                return Err(status.into());
            }
            // Find the NULL terminator in wvalue
            let length = (0..)
                .find(|&i| wvalue.offset(i).is_null() || *wvalue.offset(i) == 0)
                .unwrap();
            let wstr = OsString::from_wide(std::slice::from_raw_parts(wvalue, length as usize));
            creo_sys::ProWstringFree(wvalue);
            Ok(wstr.to_string_lossy().into_owned())
        }
    }

    fn w_string_set(&self, value: &str) -> Result<(), ProErrors> {
        unsafe {
            let mut wvalue = OsString::from(value).encode_wide().collect::<Vec<_>>();
            wvalue.push(0);
            let status = creo_sys::ProElementWstringSet(*self, wvalue.as_mut_ptr());
            match status {
                creo_sys::ProErrors_PRO_TK_NO_ERROR => Ok(()),
                _ => Err(status.into()),
            }
        }
    }
}

pub struct ProElement {
    /// Pointer to the ProElement object
    pub pro_element_ptr: creo_sys::ProElement,

    /// Indicates if this element is the root and should therefore be freed
    root: bool,
}

impl ProElement {
    /// Creates a new ProElement and manages the memory.
    /// The element is freed when it goes out of scope.
    pub fn new(name_id: u32) -> ProElement {
        unsafe {
            let mut pro_element_ptr: creo_sys::ProElement = std::mem::zeroed();
            creo_sys::ProElementAlloc(name_id as i32, &mut pro_element_ptr);
            ProElement {
                pro_element_ptr,
                root: true,
            }
        }
    }

    /// Creates a managed ProElement from a creo_sys::ProElement pointer.
    /// The element is automatically freed if `root` is set to true.
    pub fn from_ptr(pro_element_ptr: creo_sys::ProElement, root: bool) -> ProElement {
        ProElement {
            pro_element_ptr,
            root,
        }
    }
}

impl ProElementInterface for ProElement {
    fn children_get(
        &self,
        elempath: Option<creo_sys::ProElempath>,
    ) -> Result<Vec<creo_sys::ProElement>, ProErrors> {
        self.pro_element_ptr.children_get(elempath)
    }

    fn decimals_set(&self, decimals: i32) -> Result<(), ProErrors> {
        self.pro_element_ptr.decimals_set(decimals)
    }

    fn double_set(&self, value: f64) -> Result<(), ProErrors> {
        self.pro_element_ptr.double_set(value)
    }

    fn id_get(&self) -> Result<creo_sys::ProElemId, ProErrors> {
        self.pro_element_ptr.id_get()
    }

    fn elementree_add(&self, element: &mut ProElement) -> Result<(), ProErrors> {
        self.pro_element_ptr.elementree_add(element)
    }

    fn elemtree_elements_visit(
        &self,
        elempath: creo_sys::ProElempath,
        filterfunc: creo_sys::ProElemtreeVisitFilter,
        visitfunc: creo_sys::ProElemtreeVisitAction,
        appdata: creo_sys::ProAppData,
    ) -> Result<(), ProErrors> {
        self.pro_element_ptr
            .elemtree_elements_visit(elempath, filterfunc, visitfunc, appdata)
    }

    fn collect_elements(
        &self,
        elempath: Option<creo_sys::ProElempath>,
        filterfunc: creo_sys::ProElemtreeVisitFilter,
    ) -> Result<Box<Vec<creo_sys::ProElement>>, ProErrors> {
        self.pro_element_ptr.collect_elements(elempath, filterfunc)
    }

    fn integer_set(&self, value: i32) -> Result<(), ProErrors> {
        self.pro_element_ptr.integer_set(value)
    }

    fn reference_set(&self, reference: ProReference) -> Result<(), ProErrors> {
        self.pro_element_ptr.reference_set(reference)
    }

    fn reference_get(&self) -> Result<creo_sys::ProReference, ProErrors> {
        self.pro_element_ptr.reference_get()
    }

    fn valuetype_get(&self) -> Result<creo_sys::ProValueDataType, ProErrors> {
        todo!()
    }

    fn w_string_get(&self) -> Result<String, ProErrors> {
        todo!()
    }

    fn w_string_set(&self, value: &str) -> Result<(), ProErrors> {
        self.pro_element_ptr.w_string_set(value)
    }
}

/// Executing ProElementFree will free the memory for all children
/// Therefore we keep track of the root node and only free that one when it is out of scope
impl Drop for ProElement {
    fn drop(&mut self) {
        unsafe {
            if self.root {
                creo_sys::ProElementFree(&mut self.pro_element_ptr);
            }
        }
    }
}

pub struct ProElementBuilder {
    name_id: u32,
    decimals: Option<i32>,
    double: Option<f64>,
    elements: Vec<ProElement>,
    integer: Option<i32>,
    reference: Option<ProReference>,
    w_string: Option<String>,
}

impl ProElementBuilder {
    pub fn new(name_id: u32) -> ProElementBuilder {
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

    pub fn with_decimals(mut self, decimals: i32) -> ProElementBuilder {
        self.decimals = Some(decimals);
        self
    }

    pub fn with_double(mut self, double: f64) -> ProElementBuilder {
        self.double = Some(double);
        self
    }

    pub fn with_element(mut self, element: ProElement) -> ProElementBuilder {
        self.elements.push(element);
        self
    }

    pub fn with_integer(mut self, integer: i32) -> ProElementBuilder {
        self.integer = Some(integer);
        self
    }

    pub fn with_reference(mut self, reference: ProReference) -> ProElementBuilder {
        self.reference = Some(reference);
        self
    }

    pub fn with_w_string(mut self, w_string: &str) -> ProElementBuilder {
        self.w_string = Some(w_string.to_string());
        self
    }

    pub fn build(self) -> Result<ProElement, ProErrors> {
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

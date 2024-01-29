use creo_sys;
use std::ffi::CString;

use crate::pro_toolkit_errors::ProErrors;

/// Called from an asynchronous program which will be started by a Creo Parametric session.
/// This function should be called from the user-supplied main() as early as possible, to establish
/// the connection between the user application and the Creo Parametric session which launched it.
///
/// # Arguments
///
/// * `argc` - You cannot modify this argument in any way.
/// * `argv` - You cannot modify this argument in any way.
///
/// # Errors
///
/// * `Other` - The connection failed.
pub fn pro_asynchronous_main(argc: i32, argv: Vec<String>) -> ProErrors {
    let cstr_argv: Vec<_> = argv
        .iter()
        .map(|arg| CString::new(arg.as_str()).unwrap())
        .collect();
    let mut p_argv: Vec<_> = cstr_argv
        .iter() // do NOT into_iter()
        .map(|arg| arg.as_ptr())
        .collect();
    p_argv.push(std::ptr::null());
    unsafe { creo_sys::ProAsynchronousMain(argc, p_argv.as_ptr() as *mut *mut i8).into() }
}

/// Enables you to write your own `main()` without using asynchronous mode.
///
/// The rules and restrictions for a user-supplied `main()` are as follows:
/// - You can make other, non-Creo Parametric TOOLKIT calls before `ProToolkitMain()`.
/// - This process is spawned by Creo Parametric and is therefore a child process. The process
///   operates like the standard synchronous mode.
/// - The process terminates when you exit Creo Parametric.
///
/// # Arguments
///
/// * `argc` - You cannot modify this argument in any way.
/// * `argv` - You cannot modify this argument in any way.
pub fn pro_toolkit_main(argc: i32, argv: Vec<String>) {
    let cstr_argv: Vec<_> = argv
        .iter()
        .map(|arg| CString::new(arg.as_str()).unwrap())
        .collect();
    let mut p_argv: Vec<_> = cstr_argv
        .iter() // do NOT into_iter()
        .map(|arg| arg.as_ptr())
        .collect();
    p_argv.push(std::ptr::null());
    unsafe { creo_sys::ProToolkitMain(argc, p_argv.as_ptr() as *mut *mut i8) }
}

/// Causes the Creo Parametric TOOLKIT program to spawn and connect to a new
/// Creo Parametric session. It is intended for use in simple and
/// full asynchronous modes.
///
/// # Arguments
///
/// * `proe_path` - The path and file name of the Creo Parametric
///            executable, or a script that runs it.
/// * `prodev_text_path`` -  The path under which the Creo Parametric TOOLKIT message
///            and menu files are held. This is used in full
///            asynchronous mode only. Otherwise, pass a null
///            string.
///
/// # Errors
///
/// * `PRO_TK_GENERAL_ERROR` - There was a general error.
/// * `PRO_TK_NO_LICENSE` - Creo Parametric could not get a license to run.
/// * `PRO_TK_APP_NO_LICENSE` - Creo Parametric was run without the licenses needed to run this application.
/// * `PRO_TK_INVALID_DIR` - prodev_text_path is not a valid directory.
/// * `other` - Error status returned by the application's
///                user_initialize().
///
/// # Special cases
///
/// * if the application is running in native mode
///    instead of Unicode, PRO_TK_BAD_INPUTS may indicate
///    that prodev_text_path could not be converted to or
///    from Unicode.
/// * PRO_TK_APP_CREO_BARRED indicates that
///    `proe_path` pointed to a Creo application
///    that does not support Creo Parametric TOOLKIT.
pub fn pro_engineer_start(proe_path: &str, prodev_text_path: &str) -> ProErrors {
    let cstr_proe_path = CString::new(proe_path).unwrap();
    let cstr_prodev_text_path = CString::new(prodev_text_path).unwrap();
    unsafe {
        creo_sys::ProEngineerStart(
            cstr_proe_path.as_ptr() as *mut i8,
            cstr_prodev_text_path.as_ptr() as *mut i8,
        )
        .into()
    }
}

/// Initiates an event loop which will call [`pro_event_process`]
/// periodically(1sec) to handle events returned from Creo Parametric.  
/// Applicable only for full asynchronous applications which are not
/// supposed to carry out any tasks while waiting for Creo Parametric events.
///
/// IMPORTANT NOTE: This function will not return until interrupted by
/// ProAsynchronousEventLoopInterrupt() or an error returns from
/// ProEngineerStatusGet().  No separate application processing is
/// possible until that time, except within Creo Parametric event callbacks.           
///
/// # Return
///
/// * `PRO_TK_USER_ABORT` - The function was interrupted by
///                         ProAsynchronousEventLoopInterrupt().
/// * Any other value - ProEngineerStatusGet() returned this error.
pub fn pro_asynchronous_event_loop() -> ProErrors {
    unsafe { creo_sys::ProAsynchronousEventLoop().into() }
}

/// Enables your process to respond to calls (functions within
/// your process) from Creo Parametric. You must call this function
/// within your control loop to let you readily process any
/// Creo Parametric calls.
pub fn pro_event_process() {
    unsafe { creo_sys::ProEventProcess() }
}

/// Terminates a Creo Parametric session. In synchronous mode,
/// the function also terminates any Creo Parametric TOOLKIT process
/// started by the current Creo Parametric session.
///
/// # Return
///
/// * PRO_TK_COMM_ERROR - The communications to Creo Parametric failed.
pub fn pro_engineer_end() -> ProErrors {
    unsafe { creo_sys::ProEngineerEnd().into() }
}

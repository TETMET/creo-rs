//! Example of a Creo Parametric plugin written in Rust

use creo::pro_feature::*;
use creo::pro_menu_bar;
use creo::pro_selection::*;
use creo::pro_toolkit_errors::*;
use creo::pro_ui_cmd;
use log::{error, info, warn, LevelFilter};
use log4rs::config::Appender;
use log4rs::{
    append::file::FileAppender,
    config::{Config, Root},
    encode::pattern::PatternEncoder,
};

const TETMET_MSG: &str = "example_msg.txt";

fn initialize_log() -> Result<log4rs::Handle, log::SetLoggerError> {
    // For now we just write to the public dir
    let log_path = dirs_next::public_dir().unwrap().join("tetmet.log");
    let logfile = FileAppender::builder()
        // Pattern: https://docs.rs/log4rs/*/log4rs/encode/pattern/index.html
        .encoder(Box::new(PatternEncoder::new("{l} - {m}\n")))
        .build(log_path)
        .unwrap();
    // Append all logs to the same file
    let config = Config::builder()
        .appender(Appender::builder().build("logfile", Box::new(logfile)))
        .build(
            Root::builder()
                .appender("logfile")
                .build(LevelFilter::Trace),
        )
        .unwrap();
    log4rs::init_config(config)
}

fn initialize_menu() -> Result<(), creo::pro_toolkit_errors::ProErrors> {
    pro_menu_bar::pro_menubar_menu_add("TETMET", "TetmetLabel", None, true, TETMET_MSG)?;
    let cmd_id = pro_ui_cmd::pro_cmd_action_add(
        "SketchAction",
        Some(action_cb),
        5, // uiProe2ndImmediate
        Some(access_func),
        true,
        true,
    )?;
    pro_menu_bar::pro_menubarmenu_pushbutton_add(
        "TETMET",
        "SketchButton",
        "SketchLabel",
        "SketchHelpLabel",
        None,
        true,
        cmd_id,
        TETMET_MSG,
    )?;
    Ok(())
}

fn sketch_solid() -> Result<(), ProErrors> {
    let selection = pro_select("feature", 1, None, None, None, None)?;
    if selection.n_sel != 1 {
        warn!("expected 1 selection, got {}", selection.n_sel);
        return Err(ProErrors::PRO_TK_GENERAL_ERROR);
    }
    let feature: creo::ffi::ProFeature = selection[0].modelitem_get()?;
    let solid: creo::ffi::ProSolid = feature.solid_get()?;
    info!("Selected solid: {:?}", solid);
    Ok(())
}

/// Action callback when the menu item is clicked
unsafe extern "C" fn action_cb(
    _: creo::ffi::uiCmdCmdId,
    _: *mut creo::ffi::uiCmdValue,
    _: *mut std::os::raw::c_void,
) -> std::os::raw::c_int {
    if let Err(err) = sketch_solid() {
        error!("error sketching solid: {:?}", err);
        return err.into();
    }
    creo::ffi::ProErrors_PRO_TK_NO_ERROR
}

/// Access function for the menu item
unsafe extern "C" fn access_func(_: creo::ffi::uiCmdAccessMode) -> creo::ffi::uiCmdAccessState {
    creo::ffi::uiCmdAccessState_ACCESS_AVAILABLE
}

/// Initializes the plugin by creating the log file and setting up the menu
#[no_mangle]
pub extern "C" fn user_initialize() -> creo::ffi::ProError {
    if let Err(err) = initialize_log() {
        error!("error initializing logs: {}", err);
        return creo::ffi::ProErrors_PRO_TK_GENERAL_ERROR;
    }
    if let Err(err) = initialize_menu() {
        error!("error initializing menu: {:?}", err);
        return err.into();
    }
    creo::ffi::ProErrors_PRO_TK_NO_ERROR
}

/// User exits
#[no_mangle]
pub extern "C" fn user_terminate() -> creo::ffi::ProErrors {
    info!("Bye!");
    creo::ffi::ProErrors_PRO_TK_NO_ERROR
}

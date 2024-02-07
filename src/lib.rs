//! Rust plugin for lattice design in Creo Parametric

mod creo;
mod lattice;

use creo::pro_axis::*;
use creo::pro_element::*;
use creo::pro_feature::*;
use creo::pro_mdl::*;
use creo::pro_reference::*;
use creo::pro_selection::*;
use creo::pro_solid::*;
use creo::pro_surface::*;
use creo::{creo::ffi, pro_core, pro_menu_bar, pro_ui_cmd};
use lattice::LatticeCell;
use log::{error, info, warn, LevelFilter};
use log4rs::{
    append::file::FileAppender,
    config::{Appender, Config, Root},
    encode::pattern::PatternEncoder,
};

const ASLM_MSG: &str = "aslm_msg.txt";

/// Initializes the log file
fn initialize_log() -> Result<log4rs::Handle, log::SetLoggerError> {
    let log_path = dirs_next::document_dir().unwrap().join("aslm.log");
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

fn initialize_menu() -> Result<(), ffi::ProError> {
    pro_menu_bar::pro_menubar_menu_add("ASLM", "AslmLabel", None, true, ASLM_MSG)?;
    let cmd_id = pro_ui_cmd::pro_cmd_action_add(
        "SketchAction",
        Some(action_cb),
        0,
        Some(access_func),
        true,
        true,
    )?;
    pro_menu_bar::pro_menubarmenu_pushbutton_add(
        "ASLM",
        "SketchButton",
        "SketchLabel",
        "SketchHelpLabel",
        None,
        true,
        cmd_id,
        ASLM_MSG,
    )?;
    Ok(())
}

/// Add a DatumPoint on an axis at the specified distance
/// Direction is now assumed but should be fetched from the geometry
fn datum_point(
    solid: &ffi::ProSolid,
    surface: &ffi::ProSurface,
    axis: &ffi::ProAxis,
    name: &str,
    distance: f64,
) -> Result<ProElement, ffi::ProError> {
    let mut axis_geomitem = axis.to_geomitem(solid)?;
    let mut surface_geomitem = surface.to_geomitem(solid)?;
    let axis_reference: ProReference = ProReference::new();
    axis_reference.set(None, Some(&mut axis_geomitem))?;
    let surface_reference: ProReference = ProReference::new();
    surface_reference.set(None, Some(&mut surface_geomitem))?;
    ProElementBuilder::new(ffi::PRO_E_DPOINT_POINT)
        .with_element(
            &mut ProElementBuilder::new(ffi::PRO_E_DPOINT_POINT_NAME)
                .with_w_string(&format!("POINT_{:}",name))
                .build()?,
        )
        .with_element(
            // First constraint is the axis
            &mut ProElementBuilder::new(ffi::PRO_E_DPOINT_PLA_CONSTRAINTS)
                .with_element(
                    &mut ProElementBuilder::new(ffi::PRO_E_DPOINT_PLA_CONSTRAINT)
                        .with_element(
                            &mut ProElementBuilder::new(ffi::PRO_E_DPOINT_PLA_CONSTR_REF)
                                .with_reference(axis_reference)
                                .build()?,
                        )
                        .with_element(
                            &mut ProElementBuilder::new(ffi::PRO_E_DPOINT_PLA_CONSTR_TYPE)
                                .with_integer(ffi::pro_dtmpnt_constr_type_PRO_DTMPNT_CONSTR_TYPE_ON)
                                .build()?,
                        )
                        .build()?,
                )
                .build()?,
        )
        .with_element(
            // Second constraint is dimension to the surface
            &mut ProElementBuilder::new(ffi::PRO_E_DPOINT_PLA_CONSTRAINTS)
                .with_element(
                    &mut ProElementBuilder::new(ffi::PRO_E_DPOINT_DIM_CONSTRAINT)
                        .with_element(
                            &mut ProElementBuilder::new(ffi::PRO_E_DPOINT_DIM_CONSTR_TYPE)
                                .with_integer(
                                    ffi::pro_dtmpnt_constr_type_PRO_DTMPNT_CONSTR_TYPE_OFFSET,
                                )
                                .build()?,
                        )
                        .with_element(
                            &mut ProElementBuilder::new(ffi::PRO_E_DPOINT_DIM_CONSTR_REF)
                                .with_reference(surface_reference)
                                .build()?,
                        )
                        .with_element(
                            &mut ProElementBuilder::new(ffi::PRO_E_DPOINT_DIM_CONSTR_VAL)
                                .with_decimals(4)
                                .with_double(-distance)
                                .build()?,
                        )
                        .build()?,
                )
                .build()?,
        )
        .build()
}

/// Fill the solid with a BCC lattice
/// This makes a couple of big assumptions and must be refactored to fit our needs
/// - WE ASSUME THERE ARE 5 AXIS
/// - The solid has one sketch
/// - The sketch has only 5 points
/// - The points are in the order of left top, right top, right bottom, left bottom, and center
/// - The generated lattice is just one cell of a BCC
/// To improve, we must tesselate a 2D sketch first, and then build up the 3D lattice
fn fill_bcc(
    solid: &ffi::ProSolid,
    surfaces: &Vec<ffi::ProSurface>,
    axis: &Vec<ffi::ProAxis>,
) -> Result<LatticeCell, ffi::ProError> {
    // Bottom
    let mut p0 = datum_point(solid, &surfaces[0], &axis[0], "P0", 0.0)?;
    let mut p1 = datum_point(solid, &surfaces[0], &axis[1], "P1", 0.0)?;
    let mut p2 = datum_point(solid, &surfaces[0], &axis[2], "P2", 0.0)?;
    let mut p3 = datum_point(solid, &surfaces[0], &axis[3], "P3", 0.0, )?;
    // Top
    let mut p4 = datum_point(solid, &surfaces[0], &axis[0], "P4", 40.0)?;
    let mut p5 = datum_point(solid, &surfaces[0], &axis[1], "P5", 40.0)?;
    let mut p6 = datum_point(solid, &surfaces[0], &axis[2], "P6", 40.0, )?;
    let mut p7 = datum_point(solid, &surfaces[0], &axis[3], "P7", 40.0)?;
    // Middle
    let mut p8 = datum_point(solid, &surfaces[0], &axis[4], "P8", 20.0)?;
    let cell = LatticeCell {
        points: vec![
            p0.id_get()?,
            p1.id_get()?,
            p2.id_get()?,
            p3.id_get()?,
            p4.id_get()?,
            p5.id_get()?,
            p6.id_get()?,
            p7.id_get()?,
            p8.id_get()?,
        ]
    };
    let mut points_array = ProElementBuilder::new(ffi::PRO_E_DPOINT_POINTS_ARRAY)
        .with_element(&mut p0)
        .with_element(&mut p1)
        .with_element(&mut p2)
        .with_element(&mut p3)
        .with_element(&mut p4)
        .with_element(&mut p5)
        .with_element(&mut p6)
        .with_element(&mut p7)
        .with_element(&mut p8)
        .build()?;
    let pro_e_feature_tree = ProElementBuilder::new(ffi::PRO_E_FEATURE_TREE)
        .with_element(
            &mut ProElementBuilder::new(ffi::PRO_E_FEATURE_TYPE)
                .with_integer(ffi::PRO_FEAT_DATUM_POINT as i32)
                .build()?,
        )
        .with_element(
            &mut ProElementBuilder::new(ffi::PRO_E_DPOINT_TYPE)
                .with_integer(ffi::Pro_DPoint_Type_PRO_DPOINT_TYPE_GENERAL)
                .build()?,
        )
        .with_element(
            &mut ProElementBuilder::new(ffi::PRO_E_STD_FEATURE_NAME)
                .with_w_string("BCC_LATICE")
                .build()?,
        )
        .with_element(&mut points_array)
        .build()?;
    let mut model_item: ffi::ProModelitem = solid.to_modelitem()?;
    let selection = ProSelection::new(None, Some(&mut model_item));
    let feature_options = ProFeatureCreateOptions::new(&vec![
        ffi::pro_feature_create_options_PRO_FEAT_CR_INCOMPLETE_FEAT,
    ]);
    pro_feature_withoptions_create(
        selection.pro_selection_ptr,
        pro_e_feature_tree.pro_element_ptr,
        feature_options.pro_feature_options_ptr as *mut ffi::ProFeatureCreateOptions,
        ffi::PRO_REGEN_NO_FLAGS as i32,
    )?;
    // TODO: Export a vector
    Ok(cell)
}

/// Transform a solid into a lattice structure and keep track of the nodes
fn sketch_solid() -> Result<(), ffi::ProError> {
    // Prompt the user to select a solid
    let selection = pro_select("feature", 1, None, None, None, None)?;
    if selection.n_sel != 1 {
        warn!("expected 1 selection, got {}", selection.n_sel);
        return Err(ffi::ProErrors_PRO_TK_GENERAL_ERROR);
    }
    let feature: ffi::ProFeature = selection[0].modelitem_get()?;
    let solid: ffi::ProSolid = feature.solid_get()?;
    let axis = solid.collect_axis()?;
    let surfaces = solid.collect_surfaces()?;
    if axis.len() < 5 {
        warn!("expected 5 axis, got {}", axis.len());
        return Err(ffi::ProErrors_PRO_TK_GENERAL_ERROR);
    }
    let cell = fill_bcc(&solid, &surfaces, &axis)?;
    for i in 0..cell.points.len() {
        info!("point {}: {}", i, cell.points[i]);
    }
    Ok(())
}

/// Action callback when the menu item is clicked
unsafe extern "C" fn action_cb(
    _: ffi::uiCmdCmdId,
    _: *mut ffi::uiCmdValue,
    _: *mut std::os::raw::c_void,
) -> std::os::raw::c_int {
    info!("Sketch button clicked");
    if let Err(err) = sketch_solid() {
        error!("error sketching solid: {}", err);
        return err;
    }
    ffi::ProErrors_PRO_TK_NO_ERROR
}

/// Access function for the menu item
unsafe extern "C" fn access_func(_: ffi::uiCmdAccessMode) -> ffi::uiCmdAccessState {
    ffi::uiCmdAccessState_ACCESS_AVAILABLE
}

/// Initializes the plugin by creating the log file and setting up the menu
#[no_mangle]
pub extern "C" fn user_initialize() -> ffi::ProError {
    if let Err(err) = initialize_log() {
        error!("error initializing logs: {}", err);
        return ffi::ProErrors_PRO_TK_GENERAL_ERROR;
    }
    if let Err(err) = initialize_menu() {
        error!("error initializing menu: {}", err);
        return err;
    }
    ffi::ProErrors_PRO_TK_NO_ERROR
}

/// User exits
#[no_mangle]
pub extern "C" fn user_terminate() -> i32 {
    return 0;
}

pub extern "C" fn main() {
    let args: Vec<String> = std::env::args().collect();
    let argc = args.len() as i32;
    pro_core::pro_toolkit_main(argc, args);
}

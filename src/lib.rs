//! Rust plugin for lattice design in Creo Parametric

mod creo;
mod lattice;

use crate::creo::pro_elem_id::*;
use creo::pro_axis::*;
use creo::pro_element::*;
use creo::pro_feature::*;
use creo::pro_mdl::*;
use creo::pro_reference::*;
use creo::pro_selection::*;
use creo::pro_solid::*;
use creo::pro_surface::*;
use creo::{creo::ffi, pro_core, pro_menu_bar, pro_ui_cmd};
use lattice::Lattice;
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
        5, // uiProe2ndImmediate
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
    id: &u32,
    offset: f64,
) -> Result<ProElement, ffi::ProError> {
    let mut axis_geomitem = axis.to_geomitem(solid)?;
    let mut surface_geomitem = surface.to_geomitem(solid)?;
    let axis_reference: ProReference = ProReference::new();
    axis_reference.set(None, Some(&mut axis_geomitem))?;
    let surface_reference: ProReference = ProReference::new();
    surface_reference.set(None, Some(&mut surface_geomitem))?;
    ProElementBuilder::new(ffi::PRO_E_DPOINT_POINT)
        .with_element(
            ProElementBuilder::new(ffi::PRO_E_DPOINT_POINT_NAME)
                .with_w_string(&id.to_string())
                .build()?,
        )
        .with_element(
            // First constraint is the axis
            ProElementBuilder::new(ffi::PRO_E_DPOINT_PLA_CONSTRAINTS)
                .with_element(
                    ProElementBuilder::new(ffi::PRO_E_DPOINT_PLA_CONSTRAINT)
                        .with_element(
                            ProElementBuilder::new(ffi::PRO_E_DPOINT_PLA_CONSTR_REF)
                                .with_reference(axis_reference)
                                .build()?,
                        )
                        .with_element(
                            ProElementBuilder::new(ffi::PRO_E_DPOINT_PLA_CONSTR_TYPE)
                                .with_integer(ffi::pro_dtmpnt_constr_type_PRO_DTMPNT_CONSTR_TYPE_ON)
                                .build()?,
                        )
                        .build()?,
                )
                .build()?,
        )
        .with_element(
            // Second constraint is dimension to the surface
            ProElementBuilder::new(ffi::PRO_E_DPOINT_DIM_CONSTRAINTS)
                .with_element(
                    ProElementBuilder::new(ffi::PRO_E_DPOINT_DIM_CONSTRAINT)
                        .with_element(
                            ProElementBuilder::new(ffi::PRO_E_DPOINT_DIM_CONSTR_TYPE)
                                .with_integer(
                                    ffi::pro_dtmpnt_constr_type_PRO_DTMPNT_CONSTR_TYPE_OFFSET,
                                )
                                .build()?,
                        )
                        .with_element(
                            ProElementBuilder::new(ffi::PRO_E_DPOINT_DIM_CONSTR_REF)
                                .with_reference(surface_reference)
                                .build()?,
                        )
                        .with_element(
                            ProElementBuilder::new(ffi::PRO_E_DPOINT_DIM_CONSTR_VAL)
                                .with_decimals(4)
                                .with_double(-offset) // TODO: Implement direction
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
/// - Only 5 axis for now (!)
/// - This only creates the unit cell for now
/// - The solid has one sketch
/// - The points are in the order of left top, right top, right bottom, left bottom, and center
fn fill_bcc(
    solid: &ffi::ProSolid,
    surfaces: &Vec<ffi::ProSurface>,
    axis: &Vec<ffi::ProAxis>,
    offset: f64,
    lattice: &mut Lattice,
) -> Result<(), ffi::ProError> {
    let mut points: [u32; 9] = [0; 9];
    for i in 0..9 {
        points[i] = lattice.add_node(None);
    }
    lattice.add_edge(points[0], points[1]);
    lattice.add_edge(points[1], points[2]);
    lattice.add_edge(points[2], points[3]);
    lattice.add_edge(points[3], points[0]);
    lattice.add_edge(points[4], points[5]);
    lattice.add_edge(points[5], points[6]);
    lattice.add_edge(points[6], points[7]);
    lattice.add_edge(points[7], points[4]);
    lattice.add_edge(points[0], points[4]);
    lattice.add_edge(points[1], points[5]);
    lattice.add_edge(points[2], points[6]);
    lattice.add_edge(points[3], points[7]);
    lattice.add_edge(points[0], points[8]);
    lattice.add_edge(points[1], points[8]);
    lattice.add_edge(points[2], points[8]);
    lattice.add_edge(points[3], points[8]);
    lattice.add_edge(points[4], points[8]);
    lattice.add_edge(points[5], points[8]);
    lattice.add_edge(points[6], points[8]);
    lattice.add_edge(points[7], points[8]);
    let datum_points: ProElement = ProElementBuilder::new(ffi::PRO_E_DPOINT_POINTS_ARRAY)
        .with_element(datum_point(
            solid,
            &surfaces[0],
            &axis[0],
            &points[0],
            offset,
        )?)
        .with_element(datum_point(
            solid,
            &surfaces[0],
            &axis[1],
            &points[1],
            offset,
        )?)
        .with_element(datum_point(
            solid,
            &surfaces[0],
            &axis[2],
            &points[2],
            offset,
        )?)
        .with_element(datum_point(
            solid,
            &surfaces[0],
            &axis[3],
            &points[3],
            offset,
        )?)
        .with_element(datum_point(
            solid,
            &surfaces[0],
            &axis[0],
            &points[4],
            offset + 50.0,
        )?)
        .with_element(datum_point(
            solid,
            &surfaces[0],
            &axis[1],
            &points[5],
            offset + 50.0,
        )?)
        .with_element(datum_point(
            solid,
            &surfaces[0],
            &axis[2],
            &points[6],
            offset + 50.0,
        )?)
        .with_element(datum_point(
            solid,
            &surfaces[0],
            &axis[3],
            &points[7],
            offset + 50.0,
        )?)
        .with_element(datum_point(
            solid,
            &surfaces[0],
            &axis[4],
            &points[8],
            offset + 25.0,
        )?)
        .build()?;
    let pro_e_feature_tree = ProElementBuilder::new(ffi::PRO_E_FEATURE_TREE)
        .with_element(
            ProElementBuilder::new(ffi::PRO_E_FEATURE_TYPE)
                .with_integer(ffi::PRO_FEAT_DATUM_POINT as i32)
                .build()?,
        )
        .with_element(
            ProElementBuilder::new(ffi::PRO_E_DPOINT_TYPE)
                .with_integer(ffi::Pro_DPoint_Type_PRO_DPOINT_TYPE_GENERAL)
                .build()?,
        )
        .with_element(
            ProElementBuilder::new(ffi::PRO_E_STD_FEATURE_NAME)
                .with_w_string("BCC_LATTICE")
                .build()?,
        )
        .with_element(datum_points)
        .build()?;
    let mut model_item: ffi::ProModelitem = solid.to_modelitem()?;
    let selection = ProSelection::new(None, Some(&mut model_item));
    let feature_options = ProFeatureCreateOptions::new(&vec![
        ffi::pro_feature_create_options_PRO_FEAT_CR_DEFINE_MISS_ELEMS,
    ]);
    let feature = pro_feature_withoptions_create(
        selection.pro_selection_ptr,
        pro_e_feature_tree.pro_element_ptr,
        feature_options.pro_feature_options_ptr as *mut ffi::ProFeatureCreateOptions,
        ffi::PRO_REGEN_NO_FLAGS as i32,
    )?;
    info!("extracting points from feature {:?}", feature);
    let points = feature.collect_geomitems(ffi::pro_obj_types_PRO_POINT, None)?;
    info!("Collected {} points", points.len());
    Ok(())
}

// Print a ProElement
// TODO: Move this to ProElement and finish it to make it better, this is here for debugging
fn print_element(element: ProElement) -> Result<(), ffi::ProError> {
    let id = element.id_get()?;
    let elements = element.collect_elements(None, None)?;
    info!("Found {} elements for element {}", elements.len(), id);
    for element in elements.as_ref() {
        let element_id = element.id_get()?;
        let element_type = element.valuetype_get();
        if let Ok(element_type) = element_type {
            info!(
                "Element ID: {} and Value Type: {}",
                pro_elem_id_string_get(element_id)?,
                element_type
            );
            match element_type {
                ffi::pro_value_data_type_PRO_VALUE_TYPE_WSTRING => {
                    let name = element.w_string_get()?;
                    info!("Name: {}", name);
                }
                _ => {
                    //
                }
            }
        }
    }
    Ok(())
}

/// Transform a solid into a lattice structure and keep track of the nodes
fn sketch_solid() -> Result<(), ffi::ProError> {
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
    let mut lattice = Lattice::new();
    let offset = 0.0; // For now just 1 BCC at the bottom
    fill_bcc(&solid, &surfaces, &axis, offset, &mut lattice)?;
    Ok(())
}

/// Action callback when the menu item is clicked
unsafe extern "C" fn action_cb(
    _: ffi::uiCmdCmdId,
    _: *mut ffi::uiCmdValue,
    _: *mut std::os::raw::c_void,
) -> std::os::raw::c_int {
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

pub extern "C" fn main() -> i32 {
    let args: Vec<String> = std::env::args().collect();
    let argc = args.len() as i32;
    pro_core::pro_toolkit_main(argc, args);
    0
}

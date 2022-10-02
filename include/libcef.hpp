#if !defined(INC_H___0035FA3F_4688_4DF1_B09A_E28C2D872233__HEAD__)
#define INC_H___0035FA3F_4688_4DF1_B09A_E28C2D872233__HEAD__

#include <shellscalingapi.h>

#include "include/cef_base.h"
#include "include/cef_app.h"
#include "include/cef_v8.h"
#include "include/cef_dom.h"
#include "include/cef_task.h"
#include "include/cef_trace.h"
#include "include/cef_client.h"
#include "include/cef_frame.h"
#include "include/cef_parser.h"
#include "include/cef_cookie.h"
#include "include/cef_values.h"
#include "include/cef_server.h"
#include "include/cef_browser.h"
#include "include/cef_i18n_util.h"
#include "include/cef_crash_util.h"
#include "include/cef_pack_strings.h"
#include "include/cef_command_line.h"
#include "include/cef_command_ids.h"
#include "include/cef_request.h"
#include "include/cef_urlrequest.h"
#include "include/cef_render_handler.h"
#include "include/cef_request_context.h"
#include "include/cef_resource_handler.h"
#include "include/cef_response.h"
#include "include/cef_stream.h"
#include "include/cef_scheme.h"
#include "include/cef_file_util.h"
#include "include/cef_path_util.h"
#include "include/cef_ssl_status.h"
#include "include/cef_pack_strings.h"
#include "include/cef_media_router.h"
#include "include/cef_waitable_event.h"
#include "include/cef_x509_certificate.h"
#include "include/cef_extension_handler.h"
#include "include/cef_menu_model_delegate.h"
#include "include/cef_request_context_handler.h"
#include "include/cef_shared_process_message_builder.h"

#include "include/internal/cef_types.h"
#include "include/internal/cef_string.h"
#include "include/internal/cef_types_wrappers.h"

#include "include/base/cef_lock.h"
#include "include/base/cef_build.h"
#include "include/base/cef_macros.h"
#include "include/base/cef_logging.h"
#include "include/base/cef_callback.h"
#include "include/base/cef_weak_ptr.h"
#include "include/base/cef_ref_counted.h"
#include "include/base/cef_thread_checker.h"
#include "include/base/cef_platform_thread.h"
#include "include/base/cef_cxx17_backports.h"
#include "include/base/cef_callback_forward.h"

#include "include/views/cef_window.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_panel.h"
#include "include/views/cef_panel_delegate.h"
#include "include/views/cef_scroll_view.h"
#include "include/views/cef_window_delegate.h"
#include "include/views/cef_box_layout.h"
#include "include/views/cef_fill_layout.h"
#include "include/views/cef_layout.h"
#include "include/views/cef_button.h"
#include "include/views/cef_display.h"
#include "include/views/cef_box_layout.h"
#include "include/views/cef_button_delegate.h"
#include "include/views/cef_label_button.h"
#include "include/views/cef_textfield.h"
#include "include/views/cef_textfield_delegate.h"
#include "include/views/cef_menu_button.h"
#include "include/views/cef_overlay_controller.h"
#include "include/views/cef_menu_button_delegate.h"

#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_message_router.h"
#include "include/wrapper/cef_scoped_temp_dir.h"
#include "include/wrapper/cef_byte_read_handler.h"
#include "include/wrapper/cef_resource_manager.h"
#include "include/wrapper/cef_stream_resource_handler.h"

#ifdef WRAPPING_CEF_SHARED
#undef WRAPPING_CEF_SHARED
#define WRAPPING_CEF_SHARED 1
#else
#define WRAPPING_CEF_SHARED 1
#endif
#include "include/test/cef_test_helpers.h"


#pragma comment(lib,"d3d11")
#pragma comment(lib,"glu32")
#pragma comment(lib,"imm32")
#pragma comment(lib,"opengl32")

#pragma comment(lib,"libcef")
#pragma comment(lib,"libcef_dll_wrapper")
/// /*新生®（上海）**/
/// /*2022_09_23T04:33:37.1463922Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___0035FA3F_4688_4DF1_B09A_E28C2D872233__HEAD__
// Copyright (c) 2022 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
//
// ---------------------------------------------------------------------------
//
// This file was generated by the CEF translator tool. If making changes by
// hand only do so within the body of existing method and function
// implementations. See the translator.README.txt file in the tools directory
// for more information.
//
// $hash=03a39472314a6cb7cf93fb0196be57e9ae308c53$
//

#include "libcef_dll/cpptoc/views/panel_delegate_cpptoc.h"
#include "libcef_dll/cpptoc/views/window_delegate_cpptoc.h"
#include "libcef_dll/ctocpp/views/view_ctocpp.h"
#include "libcef_dll/shutdown_checker.h"

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

cef_size_t CEF_CALLBACK
panel_delegate_get_preferred_size(struct _cef_view_delegate_t* self,
                                  cef_view_t* view) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return CefSize();
  // Verify param: view; type: refptr_diff
  DCHECK(view);
  if (!view)
    return CefSize();

  // Execute
  cef_size_t _retval =
      CefPanelDelegateCppToC::Get(reinterpret_cast<cef_panel_delegate_t*>(self))
          ->GetPreferredSize(CefViewCToCpp::Wrap(view));

  // Return type: simple
  return _retval;
}

cef_size_t CEF_CALLBACK
panel_delegate_get_minimum_size(struct _cef_view_delegate_t* self,
                                cef_view_t* view) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return CefSize();
  // Verify param: view; type: refptr_diff
  DCHECK(view);
  if (!view)
    return CefSize();

  // Execute
  cef_size_t _retval =
      CefPanelDelegateCppToC::Get(reinterpret_cast<cef_panel_delegate_t*>(self))
          ->GetMinimumSize(CefViewCToCpp::Wrap(view));

  // Return type: simple
  return _retval;
}

cef_size_t CEF_CALLBACK
panel_delegate_get_maximum_size(struct _cef_view_delegate_t* self,
                                cef_view_t* view) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return CefSize();
  // Verify param: view; type: refptr_diff
  DCHECK(view);
  if (!view)
    return CefSize();

  // Execute
  cef_size_t _retval =
      CefPanelDelegateCppToC::Get(reinterpret_cast<cef_panel_delegate_t*>(self))
          ->GetMaximumSize(CefViewCToCpp::Wrap(view));

  // Return type: simple
  return _retval;
}

int CEF_CALLBACK
panel_delegate_get_height_for_width(struct _cef_view_delegate_t* self,
                                    cef_view_t* view,
                                    int width) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;
  // Verify param: view; type: refptr_diff
  DCHECK(view);
  if (!view)
    return 0;

  // Execute
  int _retval =
      CefPanelDelegateCppToC::Get(reinterpret_cast<cef_panel_delegate_t*>(self))
          ->GetHeightForWidth(CefViewCToCpp::Wrap(view), width);

  // Return type: simple
  return _retval;
}

void CEF_CALLBACK
panel_delegate_on_parent_view_changed(struct _cef_view_delegate_t* self,
                                      cef_view_t* view,
                                      int added,
                                      cef_view_t* parent) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;
  // Verify param: view; type: refptr_diff
  DCHECK(view);
  if (!view)
    return;
  // Verify param: parent; type: refptr_diff
  DCHECK(parent);
  if (!parent)
    return;

  // Execute
  CefPanelDelegateCppToC::Get(reinterpret_cast<cef_panel_delegate_t*>(self))
      ->OnParentViewChanged(CefViewCToCpp::Wrap(view), added ? true : false,
                            CefViewCToCpp::Wrap(parent));
}

void CEF_CALLBACK
panel_delegate_on_child_view_changed(struct _cef_view_delegate_t* self,
                                     cef_view_t* view,
                                     int added,
                                     cef_view_t* child) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;
  // Verify param: view; type: refptr_diff
  DCHECK(view);
  if (!view)
    return;
  // Verify param: child; type: refptr_diff
  DCHECK(child);
  if (!child)
    return;

  // Execute
  CefPanelDelegateCppToC::Get(reinterpret_cast<cef_panel_delegate_t*>(self))
      ->OnChildViewChanged(CefViewCToCpp::Wrap(view), added ? true : false,
                           CefViewCToCpp::Wrap(child));
}

void CEF_CALLBACK
panel_delegate_on_window_changed(struct _cef_view_delegate_t* self,
                                 cef_view_t* view,
                                 int added) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;
  // Verify param: view; type: refptr_diff
  DCHECK(view);
  if (!view)
    return;

  // Execute
  CefPanelDelegateCppToC::Get(reinterpret_cast<cef_panel_delegate_t*>(self))
      ->OnWindowChanged(CefViewCToCpp::Wrap(view), added ? true : false);
}

void CEF_CALLBACK
panel_delegate_on_layout_changed(struct _cef_view_delegate_t* self,
                                 cef_view_t* view,
                                 const cef_rect_t* new_bounds) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;
  // Verify param: view; type: refptr_diff
  DCHECK(view);
  if (!view)
    return;
  // Verify param: new_bounds; type: simple_byref_const
  DCHECK(new_bounds);
  if (!new_bounds)
    return;

  // Translate param: new_bounds; type: simple_byref_const
  CefRect new_boundsVal = new_bounds ? *new_bounds : CefRect();

  // Execute
  CefPanelDelegateCppToC::Get(reinterpret_cast<cef_panel_delegate_t*>(self))
      ->OnLayoutChanged(CefViewCToCpp::Wrap(view), new_boundsVal);
}

void CEF_CALLBACK panel_delegate_on_focus(struct _cef_view_delegate_t* self,
                                          cef_view_t* view) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;
  // Verify param: view; type: refptr_diff
  DCHECK(view);
  if (!view)
    return;

  // Execute
  CefPanelDelegateCppToC::Get(reinterpret_cast<cef_panel_delegate_t*>(self))
      ->OnFocus(CefViewCToCpp::Wrap(view));
}

void CEF_CALLBACK panel_delegate_on_blur(struct _cef_view_delegate_t* self,
                                         cef_view_t* view) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;
  // Verify param: view; type: refptr_diff
  DCHECK(view);
  if (!view)
    return;

  // Execute
  CefPanelDelegateCppToC::Get(reinterpret_cast<cef_panel_delegate_t*>(self))
      ->OnBlur(CefViewCToCpp::Wrap(view));
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

CefPanelDelegateCppToC::CefPanelDelegateCppToC() {
  GetStruct()->base.get_preferred_size = panel_delegate_get_preferred_size;
  GetStruct()->base.get_minimum_size = panel_delegate_get_minimum_size;
  GetStruct()->base.get_maximum_size = panel_delegate_get_maximum_size;
  GetStruct()->base.get_height_for_width = panel_delegate_get_height_for_width;
  GetStruct()->base.on_parent_view_changed =
      panel_delegate_on_parent_view_changed;
  GetStruct()->base.on_child_view_changed =
      panel_delegate_on_child_view_changed;
  GetStruct()->base.on_window_changed = panel_delegate_on_window_changed;
  GetStruct()->base.on_layout_changed = panel_delegate_on_layout_changed;
  GetStruct()->base.on_focus = panel_delegate_on_focus;
  GetStruct()->base.on_blur = panel_delegate_on_blur;
}

// DESTRUCTOR - Do not edit by hand.

CefPanelDelegateCppToC::~CefPanelDelegateCppToC() {
  shutdown_checker::AssertNotShutdown();
}

template <>
CefRefPtr<CefPanelDelegate> CefCppToCRefCounted<
    CefPanelDelegateCppToC,
    CefPanelDelegate,
    cef_panel_delegate_t>::UnwrapDerived(CefWrapperType type,
                                         cef_panel_delegate_t* s) {
  if (type == WT_WINDOW_DELEGATE) {
    return CefWindowDelegateCppToC::Unwrap(
        reinterpret_cast<cef_window_delegate_t*>(s));
  }
  NOTREACHED() << "Unexpected class type: " << type;
  return nullptr;
}

template <>
CefWrapperType CefCppToCRefCounted<CefPanelDelegateCppToC,
                                   CefPanelDelegate,
                                   cef_panel_delegate_t>::kWrapperType =
    WT_PANEL_DELEGATE;

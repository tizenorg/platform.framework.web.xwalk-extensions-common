// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "xwalk/xwalk_extension.h"

#include <assert.h>
#include <iostream>
#include <vector>

namespace xwalk {

XWalkExtension::XWalkExtension() :
    xw_extension_(0) {
}

XWalkExtension::~XWalkExtension() {}

bool XWalkExtension::InitializeInterfaces(XW_GetInterface get_interface) {
  core_interface_ = reinterpret_cast<const XW_CoreInterface*>(
      get_interface(XW_CORE_INTERFACE));
  if (!core_interface_) {
    std::cerr << "Can't initialize extension: error getting Core interface.\n";
    return false;
  }

  messaging_interface_ = reinterpret_cast<const XW_MessagingInterface2*>(
      get_interface(XW_MESSAGING_INTERFACE_2));
  if (!messaging_interface_) {
    std::cerr <<
        "Can't initialize extension: error getting Messaging interface 2.\n";
    return false;
  }

  sync_messaging_interface_ =
      reinterpret_cast<const XW_Internal_SyncMessagingInterface*>(
          get_interface(XW_INTERNAL_SYNC_MESSAGING_INTERFACE));
  if (!sync_messaging_interface_) {
    std::cerr <<
        "Can't initialize extension: error getting SyncMessaging interface.\n";
    return false;
  }

  entry_points_interface_ =
      reinterpret_cast<const XW_Internal_EntryPointsInterface*>(
          get_interface(XW_INTERNAL_ENTRY_POINTS_INTERFACE));
  if (!entry_points_interface_) {
    std::cerr << "NOTE: Entry points interface not available in this version "
              << "of Crosswalk, ignoring entry point data for extensions.\n";
  }

  runtime_interface_ = reinterpret_cast<const XW_Internal_RuntimeInterface*>(
      get_interface(XW_INTERNAL_RUNTIME_INTERFACE));
  if (!runtime_interface_) {
    std::cerr << "NOTE: runtime interface not available in this version "
              << "of Crosswalk, ignoring runtime variables for extensions.\n";
  }

  return true;
}

void XWalkExtension::InitializeInternal(
    const char* name, const char* jsapi, const char** entry_points) {
  core_interface_->SetExtensionName(xw_extension_, name);
  core_interface_->SetJavaScriptAPI(xw_extension_, jsapi);
  if (entry_points && entry_points_interface_) {
    entry_points_interface_->SetExtraJSEntryPoints(
        xw_extension_, entry_points);
  }
}

XWalkExtensionInstance* XWalkExtension::CreateInstance() {
  return NULL;
}

std::string XWalkExtension::GetRuntimeVariable(
    const char* var_name, unsigned len) {
  if (!runtime_interface_)
    return "";

  std::vector<char> res(len + 1, 0);
  runtime_interface_->GetRuntimeVariableString(
      xw_extension_, var_name, &res[0], len);
  return std::string(res.begin(), res.end());
}

// static
void XWalkExtension::OnShutdown(XW_Extension /*xw_extension*/) {
}

// static
void XWalkExtension::OnInstanceCreated(
    XWalkExtension* extension, XW_Instance xw_instance) {
  assert(!extension->core_interface_->GetInstanceData(xw_instance));
  XWalkExtensionInstance* instance = extension->CreateInstance();
  if (!instance)
    return;
  instance->xw_instance_ = xw_instance;
  instance->extension_ = extension;
  extension->core_interface_->SetInstanceData(xw_instance, instance);
  instance->Initialize();
}

// static
void XWalkExtension::OnInstanceDestroyed(
    XWalkExtension* extension, XW_Instance xw_instance) {
  XWalkExtensionInstance* instance =
      reinterpret_cast<XWalkExtensionInstance*>(
          extension->core_interface_->GetInstanceData(xw_instance));
  if (!instance)
    return;
  instance->xw_instance_ = 0;
  delete instance;
}

// static
void XWalkExtension::HandleMessage(
    XWalkExtension* extension, XW_Instance xw_instance, const char* msg) {
  XWalkExtensionInstance* instance =
      reinterpret_cast<XWalkExtensionInstance*>(
          extension->core_interface_->GetInstanceData(xw_instance));
  if (!instance)
    return;
  instance->HandleMessage(msg);
}

// static
void XWalkExtension::HandleSyncMessage(
    XWalkExtension* extension, XW_Instance xw_instance, const char* msg) {
  XWalkExtensionInstance* instance =
      reinterpret_cast<XWalkExtensionInstance*>(
          extension->core_interface_->GetInstanceData(xw_instance));
  if (!instance)
    return;
  instance->HandleSyncMessage(msg);
}

// static
void XWalkExtension::HandleBinaryMessage(XWalkExtension* extension,
                                         XW_Instance xw_instance,
                                         const char* msg, const size_t size) {
  XWalkExtensionInstance* instance =
      reinterpret_cast<XWalkExtensionInstance*>(
          extension->core_interface_->GetInstanceData(xw_instance));
  if (!instance)
    return;
  instance->HandleBinaryMessage(msg, size);
}

XWalkExtensionInstance::XWalkExtensionInstance()
    : xw_instance_(0) {}

XWalkExtensionInstance::~XWalkExtensionInstance() {
  assert(xw_instance_ == 0);
}

void XWalkExtensionInstance::PostMessage(const char* msg) {
  if (!xw_instance_) {
    std::cerr << "Ignoring PostMessage() in the constructor or after the "
              << "instance was destroyed.";
    return;
  }
  if (extension_) {
    extension_->messaging_interface_->PostMessage(xw_instance_, msg);
  }
}

void XWalkExtensionInstance::PostBinaryMessage(
    const char* msg, const size_t size) {
  if (!xw_instance_) {
    std::cerr << "Ignoring PostMessage() in the constructor or after the "
              << "instance was destroyed.";
    return;
  }
  if (extension_) {
    extension_->messaging_interface_->PostBinaryMessage(
        xw_instance_, msg, size);
  }
}

void XWalkExtensionInstance::SendSyncReply(const char* reply) {
  if (!xw_instance_) {
    std::cerr << "Ignoring SendSyncReply() in the constructor or after the "
              << "instance was destroyed.";
    return;
  }
  if (extension_) {
    extension_->sync_messaging_interface_->SetSyncReply(xw_instance_, reply);
  }
}

}  // namespace xwalk

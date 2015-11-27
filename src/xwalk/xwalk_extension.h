// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef XWALK_XWALK_EXTENSION_H_
#define XWALK_XWALK_EXTENSION_H_

#include <sys/types.h>

#include <string>
#include <functional>

#include "xwalk/extensions/public/XW_Extension.h"
#include "xwalk/extensions/public/XW_Extension_EntryPoints.h"
#include "xwalk/extensions/public/XW_Extension_Runtime.h"
#include "xwalk/extensions/public/XW_Extension_SyncMessage.h"

namespace xwalk {

class XWalkExtensionInstance;
class XWalkExtension;

}  // namespace xwalk

#define EXPORT_XWALK_EXTENSION(NAME, CLASS)                                    \
  EXPORT_XWALK_EXTENSION_EX(NAME, CLASS, NULL)

#define EXPORT_XWALK_EXTENSION_EX(NAME, CLASS, ENTRY_POINTS)                   \
  extern "C" {                                                                 \
  extern const char kSourceJSAPI[];                                            \
  xwalk::XWalkExtension* g_extension_##NAME;                                   \
  int32_t XW_Initialize(XW_Extension xw_extension,                             \
                        XW_GetInterface get_interface) {                       \
    g_extension_##NAME = new CLASS;                                            \
    g_extension_##NAME->xw_extension_ = xw_extension;                          \
    if (!g_extension_##NAME->InitializeInterfaces(get_interface)) {            \
      return XW_ERROR;                                                         \
    }                                                                          \
    const XW_CoreInterface* core = g_extension_##NAME->core_interface_;        \
    core->RegisterShutdownCallback(                                            \
      xw_extension, [](XW_Extension xw_extension) {                            \
        xwalk::XWalkExtension::OnShutdown(xw_extension);                       \
        if (g_extension_##NAME) {                                              \
          delete g_extension_##NAME;                                           \
          g_extension_##NAME = nullptr;                                        \
        }                                                                      \
      });                                                                      \
    core->RegisterInstanceCallbacks(                                           \
        xw_extension, [](XW_Instance xw_instance) {                            \
          xwalk::XWalkExtension::OnInstanceCreated(                            \
              g_extension_##NAME, xw_instance);                                \
        }, [](XW_Instance xw_instance) {                                       \
          xwalk::XWalkExtension::OnInstanceDestroyed(                          \
              g_extension_##NAME, xw_instance);                                \
        });                                                                    \
    g_extension_##NAME->messaging_interface_->Register(                        \
        xw_extension, [](XW_Instance xw_instance, const char* msg) {           \
          xwalk::XWalkExtension::HandleMessage(                                \
              g_extension_##NAME, xw_instance, msg);                           \
        });                                                                    \
    g_extension_##NAME->sync_messaging_interface_->Register(                   \
        xw_extension,                                                          \
        [](XW_Instance xw_instance, const char* msg) {                         \
          xwalk::XWalkExtension::HandleSyncMessage(                            \
              g_extension_##NAME, xw_instance, msg);                           \
        });                                                                    \
    g_extension_##NAME->InitializeInternal(#NAME, kSourceJSAPI, ENTRY_POINTS); \
    g_extension_##NAME->Initialize();                                          \
    return XW_OK;                                                              \
  }                                                                            \
  }

namespace xwalk {

class XWalkExtension {
 public:
  XWalkExtension();
  virtual ~XWalkExtension();

  virtual void Initialize() {}

  virtual XWalkExtensionInstance* CreateInstance();

  std::string GetRuntimeVariable(const char* var_name, unsigned len);

 private:
  friend class XWalkExtensionInstance;
  friend int32_t ::XW_Initialize(XW_Extension extension,
                                 XW_GetInterface get_interface);

  bool InitializeInterfaces(XW_GetInterface get_interface);
  void InitializeInternal(const char* name, const char* jsapi,
                          const char** entry_points);

  // XW_Extension callbacks.
  static void OnShutdown(XW_Extension xw_extension);
  static void OnInstanceCreated(XWalkExtension* extension,
                                XW_Instance xw_instance);
  static void OnInstanceDestroyed(XWalkExtension* extension,
                                  XW_Instance xw_instance);
  static void HandleMessage(XWalkExtension* extension,
                            XW_Instance xw_instance, const char* msg);
  static void HandleSyncMessage(XWalkExtension* extension,
                                XW_Instance xw_instance, const char* msg);

  XW_Extension xw_extension_;
  const XW_CoreInterface* core_interface_ = NULL;
  const XW_MessagingInterface* messaging_interface_ = NULL;
  const XW_Internal_SyncMessagingInterface* sync_messaging_interface_ = NULL;
  const XW_Internal_EntryPointsInterface* entry_points_interface_ = NULL;
  const XW_Internal_RuntimeInterface* runtime_interface_ = NULL;
};

class XWalkExtensionInstance {
 public:
  XWalkExtensionInstance();
  virtual ~XWalkExtensionInstance();

  void PostMessage(const char* msg);
  void SendSyncReply(const char* reply);

  virtual void Initialize() {}
  virtual void HandleMessage(const char* /*msg*/) {}
  virtual void HandleSyncMessage(const char* /*msg*/) {}

  XW_Instance xw_instance() const { return xw_instance_; }

 private:
  friend class XWalkExtension;

  XW_Instance xw_instance_;
  XWalkExtension* extension_;
};

}  // namespace xwalk

#endif  // XWALK_XWALK_EXTENSION_H_

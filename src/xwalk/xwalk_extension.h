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
#include "xwalk/extensions/public/XW_Extension_Message_2.h"
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
    g_extension_##NAME->messaging_interface_->RegisterBinaryMesssageCallback(  \
        xw_extension, [](XW_Instance xw_instance,                              \
                         const char* msg, const size_t size) {                 \
          xwalk::XWalkExtension::HandleBinaryMessage(                          \
              g_extension_##NAME, xw_instance, msg, size);                     \
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

// XWalkExtension is a super-class of all crosswalk extenions. It implements
// interfaces to communicate with Crosswalk runtime.
class XWalkExtension {
 public:
  XWalkExtension();
  virtual ~XWalkExtension();

  // Override this function to initialize the sub-class of XWalkExtension.
  // This function is called after registering the extension to Crosswalk
  // runtime.
  virtual void Initialize() {}

  // Override this function to return the sub-class of XWalkExtensionInstance.
  virtual XWalkExtensionInstance* CreateInstance();

  // Gets variables provided by Crosswalk runtime.
  // Note: This function should not be called in constructors of the sub-class.
  std::string GetRuntimeVariable(const char* var_name, unsigned len);

 private:
  friend class XWalkExtensionInstance;
  friend int32_t ::XW_Initialize(XW_Extension extension,
                                 XW_GetInterface get_interface);

  // Initializes callbackes of the interface for crosswalk extensions
  bool InitializeInterfaces(XW_GetInterface get_interface);

  // Sets information of the extension to Crosswalk runtime.
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
  static void HandleBinaryMessage(XWalkExtension* extension,
                                  XW_Instance xw_instance,
                                  const char* msg, const size_t size);

  // Identifier of an extension
  XW_Extension xw_extension_;

  // XW_Extension interfaces.
  const XW_CoreInterface* core_interface_ = NULL;
  const XW_MessagingInterface2* messaging_interface_ = NULL;
  const XW_Internal_SyncMessagingInterface* sync_messaging_interface_ = NULL;
  const XW_Internal_EntryPointsInterface* entry_points_interface_ = NULL;
  const XW_Internal_RuntimeInterface* runtime_interface_ = NULL;
};


// XWalkExtensionInstance is a super-class of instances created from extensions.
class XWalkExtensionInstance {
 public:
  XWalkExtensionInstance();
  virtual ~XWalkExtensionInstance();

  // Sends a message to javascript scope asyncronously.
  void PostMessage(const char* msg);

  // Sends a binary message to javascript scope asyncronously.
  void PostBinaryMessage(const char* msg, const size_t size);

  // Sends a reply of syncronous call to javascript scope immediately.
  void SendSyncReply(const char* reply);

  // Override this function to initialize the sub-class of this class.
  virtual void Initialize() {}

  // Override this function to handle asyncronous messages sent from javascript.
  virtual void HandleMessage(const char* /*msg*/) {}

  // Override this function to handle syncronous messages sent from javascript.
  virtual void HandleSyncMessage(const char* /*msg*/) {}

  // Override this function to handle binary message sent from javascript.
  virtual void HandleBinaryMessage(const char* /*msg*/,
                                   const size_t /*size*/) {}

 private:
  friend class XWalkExtension;

  // Indentifier of an instance
  XW_Instance xw_instance_;

  // Pointer of parent extension
  const XWalkExtension* extension_;
};

}  // namespace xwalk

#endif  // XWALK_XWALK_EXTENSION_H_

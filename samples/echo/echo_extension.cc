/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "echo_extension.h"

namespace sample {

/**
 * @class EchoExtension
 */

/* @method EchoExtension::CreateInstance()
 *
 * CreateInstance() SHOULD be implemented in inherited class
 * to return your own ExtensionInstance class.
 */
xwalk::XWalkExtensionInstance* EchoExtension::CreateInstance() {
  return new EchoInstance;
}

/**
 * @class EchoInstance
 */

/* @method EchoInstance::HandleMessage()
 *
 * HandleMessage() CAN be implemented if want to handle asyncronous messages
 * sent by 'extension.postMessage()' in echo_api.js.
 * Asyncronous response can be sent with PostMessage() and the sent response
 * can be handled using 'extension.setMessageListener()' in echo_api.js also.
 */
void EchoInstance::HandleMessage(const char* msg) {
  PostMessage(msg);
}

/* @method EchoInstance::HandleSyncMessage()
 *
 * HandleSyncMessage() CAN be implemented if want to handle syncronous messages
 * sent by 'extension.internal.sendSyncMessage()' in echo_api.js.
 * This method should send response with SendSyncReply().
 */
void EchoInstance::HandleSyncMessage(const char* msg) {
  SendSyncReply(msg);
}

}  // namespace sample

/* @macro EXPORT_XWALK_EXTENSION
 *
 * The implemented sub-class of XWalkExtension should be exported with
 * EXPORT_XWALK_EXTENSION(name, class) macro.
 */
EXPORT_XWALK_EXTENSION(tizen.echo, sample::EchoExtension);

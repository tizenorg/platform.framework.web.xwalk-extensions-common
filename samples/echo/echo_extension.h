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

#ifndef ECHO_ECHO_EXTENSION_H_
#define ECHO_ECHO_EXTENSION_H_

#include <xwalk/xwalk_extension.h>

namespace sample {

class EchoExtension : public xwalk::XWalkExtension {
 public:
  // @override
  xwalk::XWalkExtensionInstance* CreateInstance();
};

class EchoInstance : public xwalk::XWalkExtensionInstance {
 public:
  // @override
  void HandleMessage(const char* msg);

  // @override
  void HandleSyncMessage(const char* msg);
};

}  // namespace sample

#endif  // ECHO_ECHO_EXTENSION_H_

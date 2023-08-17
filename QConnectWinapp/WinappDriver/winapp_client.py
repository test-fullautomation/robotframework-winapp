#  Copyright 2020-2023 Robert Bosch GmbH
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
# *******************************************************************************
#
# File: winapp_client.py
#
# Initially created by Nguyen Huynh Tri Cuong (MS/EMC12-XC) / April 2023.
#
# Description:
#   This module plays a role as the actual handler for winapp via WinappDriver.
#
# History:
#
# 25.04.2023 / V 0.1 / Nguyen Huynh Tri Cuong
# - Initialize
#
# *******************************************************************************
import time
import subprocess
import os
import signal
import pkgutil
import importlib
import copy
import ast
import re
import QConnectBase.constants as constants
from appium.webdriver.common.appiumby import AppiumBy
from appium.webdriver.webdriver import WebDriver
from typing import Any, Callable, Dict, Optional, Union
from appium.options.common.base import AppiumOptions
from selenium.common.exceptions import InvalidArgumentException, SessionNotCreatedException
from selenium.webdriver.remote.command import Command as RemoteCommand
from inspect import currentframe
from QConnectBase.utils import Utils
from QConnectWinapp.ActionHandlers.element_handler import ElementActionHandler
from QConnectBase.connection_base import ConnectionBase
from QConnectBase.utils import DictToClass
from robot.libraries.BuiltIn import BuiltIn


class CustomWebDriver(WebDriver):
   """
Customer WebDriver class for Winapp.
   """
   def start_session(self, capabilities: Union[Dict, AppiumOptions], browser_profile: Optional[str] = None) -> None:
      """
Creates a new session with the desired capabilities.

Override for Winapp

**Arguments:**

* ``capabilities``

  / *Condition*: required / *Type*: Union /

  Read https://github.com/appium/appium/blob/master/docs/en/writing-running-appium/caps.md for more details.


* ``browser_profile``

  / *Condition*: optional / *Type*: str / *Default*: None /

  Browser profile
      """
      if not isinstance(capabilities, (dict, AppiumOptions)):
         raise InvalidArgumentException('Capabilities must be a dictionary or AppiumOptions instance')

      w3c_caps = AppiumOptions.as_w3c(capabilities) if isinstance(capabilities, dict) else capabilities.to_w3c()
      response = self.execute(RemoteCommand.NEW_SESSION, {'capabilities': w3c_caps, 'desiredCapabilities': capabilities})
      # https://w3c.github.io/webdriver/#new-session
      if not isinstance(response, dict):
         raise SessionNotCreatedException(
            f'A valid W3C session creation response must be a dictionary. Got "{response}" instead'
         )
      # Due to a W3C spec parsing misconception some servers
      # pack the createSession response stuff into 'value' dictionary and
      # some other put it to the top level of the response JSON nesting hierarchy
      get_response_value: Callable[[str], Optional[Any]] = lambda key: response.get(key) or (
         response['value'].get(key) if isinstance(response.get('value'), dict) else None
      )
      session_id = get_response_value('sessionId')
      if not session_id:
         raise SessionNotCreatedException(
            f'A valid W3C session creation response must contain a non-empty "sessionId" entry. '
            f'Got "{response}" instead'
         )
      self.session_id = session_id
      self.caps = response.get('value')

      # if capabilities is none we are probably speaking to
      # a W3C endpoint
      if self.caps is None:
         self.caps = response.get('capabilities')


class WinappConfig(DictToClass):
   """
Class to store the configuration for SSH connection.
   """
   host = 'localhost'
   port = 4723
   caps = {}


class WinAppClient(ConnectionBase):
   """
Winapp client connection class.
   """
   _DEFAULT_APP_DRIVER = "C:/Program Files (x86)/Windows Application Driver/WinAppDriver.exe"
   _CONNECTION_TYPE = "Winapp"

   def __init__(self, _mode, config):
      """
Constructor of WinAppClient class.

**Arguments:**

* ``config``

  / *Condition*: optional / *Type*: dict / *Default*: None /

  Configurations for DLT connection.

* ``_mode``

  / *Condition*: optional / *Type*: str / *Default*: None /

  Unused.
      """
      _mident = '%s.%s()' % (self.__class__.__name__, currentframe().f_code.co_name)
      BuiltIn().log("[%s] Initializing WinAppClient with the configuration as '%s'..." % (_mident, str(config)), constants.LOG_LEVEL_DEBUG)
      self.config = WinappConfig(**config)
      self.host = self.config.host
      self.port = self.config.port
      self.caps = self.config.caps.__dict__
      self.winapp_driver = None
      self.winapp_driver_pid = None
      self.__start_winapp_driver()
      self._find_element_method_dict = {}
      self._supported_action_handlers = None
      self.__support_id_list = None

   def __start_winapp_driver(self):
      """
Start WinappDriver listen on configured port.

**Returns:**

(*no returns*)
      """
      with open('AppdriverLog.txt', 'w') as f:
         process = subprocess.Popen([WinAppClient._DEFAULT_APP_DRIVER, self.host, str(self.port)], stdout=f, stderr=f)
         self.winapp_driver_pid = process.pid

   def connect(self):
      """
Connect to WinappDriver which is listening on configured port.

**Returns:**

(*no returns*)
      """
      if self.caps:
         self.winapp_driver = CustomWebDriver(
            command_executor='http://%s:%s' % (self.host, self.port),
            desired_capabilities=self.caps)

      self._find_element_method_dict = {
         "AutomationId": lambda value: self.winapp_driver.find_element(AppiumBy.ACCESSIBILITY_ID, value)
      }

      self.__support_id_list = list(self._find_element_method_dict.keys())

      dir_path = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
      for module_loader, name, is_pkg in pkgutil.walk_packages([dir_path]):
         # noinspection PyBroadException
         try:
            if not is_pkg:
               importlib.import_module(name)
            else:
               _module = module_loader.find_module(name).load_module(name)
         except Exception as _ex:
            pass
      self._supported_action_handlers = Utils.get_all_descendant_classes(ElementActionHandler, True)

   def __find_element_by_definitions(self, obj_defined_dict, time_wait=0):
      """
To be implemented.
      """
      raise Exception("Not supported yet for finding element with multiple definitions")

   def __find_element_by_definition(self, obj_defined_dict, time_wait=0):
      """
Find a GUI element based on user's definitions.

**Arguments:**

* ``obj_defined_dict``

  / *Condition*: required / *Type*: dict /

  User's definition for a GUI element.

* ``time_wait``

  / *Condition*: optional / *Type*: int / *Default*: 0 /

  Timeout to find a GUI element based on user's definitions.

**Returns:**

  / *Type*: WebElement /

  GUI element.
      """
      _mident = '%s.%s()' % (self.__class__.__name__, currentframe().f_code.co_name)
      BuiltIn().log("[%s] Finding element with the definition of '%s'..." % (_mident, str(obj_defined_dict)), constants.LOG_LEVEL_DEBUG)
      ele_obj = None
      tmp_identify = copy.deepcopy(obj_defined_dict)
      # noinspection PyBroadException
      try:
         id_lid = list(tmp_identify.keys())
         id_set = set(self.__support_id_list) & set(id_lid)
         id_type = id_set.pop()
         if id_type:
            id_value = tmp_identify[id_type]
            # noinspection PyBroadException
            try:
               while not ele_obj:
                  try:
                     ele_obj = self._find_element_method_dict[id_type](id_value)
                  except Exception as _ex:
                     pass
                  if time.time() > time_wait or ele_obj:
                     break
            except Exception as _ex:
               pass
      except Exception as _ex:
         pass

      if ele_obj is not None:
         ele_obj = self.winapp_driver.create_web_element(ele_obj['ELEMENT'])

      return ele_obj

   def _get_action_handler(self, ele_obj):
      """
Get the action handler for the element object.

**Arguments:**

* ``ele_obj``

  / *Condition*: required / *Type*: WebElement /

  Winapp GUI element object.

**Returns:**

  / *Type*: ElementActionHandler /

  Action handler for the element object.
      """
      handler_class = None
      max_support_level = 0
      for cls in self._supported_action_handlers:
         try:
            support_level = cls.get_supported_level(ele_obj)
            if support_level > max_support_level:
               max_support_level = support_level
               handler_class = cls
         except Exception as _ex:
            pass

      if handler_class is not None:
         handler_obj = handler_class(self.winapp_driver, ele_obj)
      else:
         handler_obj = None

      return handler_obj

   def perform_action(self, obj_defined_dict, cmd, timeout=0, filter_obj=None):
      """
Perform an action on user defined element.

**Arguments:**

* ``obj_defined_dict``

  / *Condition*: required / *Type*: dict /

  User's definition for a GUI element.

* ``cmd``

  / *Condition*: required / *Type*: str /

  Action to be perform on GUI element.

* ``time_wait``

  / *Condition*: optional / *Type*: int / *Default*: 0 /

  Timeout to find a GUI element based on user's definitions.

**Returns:**

  / *Type*: WebElement /

  GUI element.
      """
      _mident = '%s.%s()' % (self.__class__.__name__, currentframe().f_code.co_name)
      BuiltIn().log("[%s] Perform action '%s' on element defined by '%s'..." % (_mident, cmd, str(obj_defined_dict)), constants.LOG_LEVEL_DEBUG)
      if type(obj_defined_dict) is str or type(obj_defined_dict) is str:
         obj_defined_dict = ast.literal_eval(obj_defined_dict)

      time_wait = time.time() + timeout

      is_single_id = (obj_defined_dict and len(obj_defined_dict) == 1)

      if is_single_id:
         ele_obj = self.__find_element_by_definition(obj_defined_dict, time_wait)
      else:
         ele_obj = self.__find_element_by_definitions(obj_defined_dict, time_wait)

      if not ele_obj:
         raise Exception("Unable to find the control with definition as: '%s'" % str(obj_defined_dict))

      ele_action_handler = self._get_action_handler(ele_obj)
      if not ele_action_handler:
         raise Exception("Automation hasn't been support for the control with definition as: '%s'" % str(obj_defined_dict))

      while filter_obj:
         cmd_ret = ele_action_handler.divert_action(cmd)
         (success, res_obj) = self._filter_msg(filter_obj, str(cmd_ret))
         if time.time() > time_wait or success:
            break
      else:
         res_obj = ele_action_handler.divert_action(cmd)

      return res_obj

   def send_obj(self, send_cmd, element_def={}, *args):
      """
Action to be send to a GUI element.

**Arguments:**

* ``obj``

  / *Condition*: required / *Type*: str /

  Data to be sent.

* ``element_def``

  / *Condition*: required / *Type*: dict /

  User's definition for a GUI element.

* ``args``

  / *Condition*: optional / *Type*: tuple /

  Optional arguments.

**Returns:**

(*no returns*)
      """
      self.perform_action(element_def, send_cmd)

   def wait_4_trace(self, search_obj, timeout=0, use_fetch_block=False, end_of_block_pattern=".*", filter_pattern=".*", send_cmd="", element_def={}, *args):
      """
Perform an action on GUI element and wait to receive a return which matches to a specified regular expression.

**Arguments:**

* ``search_obj``

  / *Condition*: required / *Type*: str /

  Regular expression all received trace messages are compare to.
  Can be passed either as a string or a regular expression object. Refer to Python documentation for module 're'.

* ``use_fetch_block``

  / *Condition*: optional / *Type*: bool / *Default*: False /

  Determine if 'fetch block' feature is used.

* ``end_of_block_pattern``

  / *Condition*: optional / *Type*: str / *Default*: '.*' /

  The end of block pattern.

* ``filter_pattern``

  / *Condition*: optional / *Type*: str / *Default*: '.*' /

  Pattern to filter message line by line.

* ``timeout``

  / *Condition*: optional / *Type*: int / *Default*: 0 /

  Timeout parameter specified as a floating point number in the unit 'seconds'.

* ``element_def``

  / *Condition*: required / *Type*: dict /

  User's definition for a GUI element.

* ``args``

  / *Condition*: optional / *Type*: tuple /

  Optional arguments.

**Returns:**

* ``match``

  / *Type*: re.Match /

  If no return value matched to the specified regular expression and a timeout occurred, return None.

  If a return value has matched to the specified regular expression, a match object is returned as the result.The complete trace message can be accessed by the 'string' attribute
  of the match object. For access to groups within the regular expression, use the group() method. For more information, refer to Python documentation for module 're'.
      """
      _mident = '%s.%s()' % (self.__class__.__name__, currentframe().f_code.co_name)
      BuiltIn().log('Execute %s' % _mident, constants.LOG_LEVEL_DEBUG)
      search_regex = re.compile(search_obj, re.M | re.S | re.U)
      return self.perform_action(element_def, send_cmd, timeout, search_regex)

   def disconnect(self, _device):
      """
Abstract method for disconnecting connection.

**Arguments:**

* ``_device``

   / *Condition*: required / *Type*: str /

   Unused.

**Returns:**

(*no returns*)
      """
      self.quit()

   def quit(self):
      """
Quiting the connection.

**Returns:**

(*no returns*)
      """
      if self.winapp_driver_pid:
         try:
            os.kill(self.winapp_driver_pid, signal.SIGTERM)
         except Exception as _ex:
            pass

   def __del__(self):
      self.quit()


if __name__ == "__main__":
   print("TEST Winapp CLient")
   desired_caps = {}
   desired_caps["app"] = "C:/Program Files/BOSCH/CMST/CMST.exe"
   win_app = WinAppClient(caps=desired_caps)
   win_app.connect()
   time.sleep(1)
   obj_def = {"acc_id": "textBoxUsername"}
   win_app.perform_action(obj_def, "get text")
   # test = win_app.winapp_driver.find_element(AppiumBy.ACCESSIBILITY_ID, "buttonSoftware")
   # test2 = win_app.winapp_driver.create_web_element(test['ELEMENT'])
   # # name = test2.accessible_name
   # # role = test2.aria_role
   # test2.click()

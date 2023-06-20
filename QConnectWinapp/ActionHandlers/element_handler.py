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
#   This module is the action handler for the Winapp GUI elements.
#
# History:
#
# 25.04.2023 / V 0.1 / Nguyen Huynh Tri Cuong
# - Initialize
#
# *******************************************************************************
from selenium.common.exceptions import WebDriverException


class ElementActionHandler:
   SUPPORTED_LEVEL = 1

   def __init__(self, driver, ele_obj):
      """
Constructor of ElementActionHandler class.

**Arguments:**

* ``driver``

  / *Condition*: required / *Type*: WebDriver /

  Web driver for Winapp application.

* ``ele_obj``

  / *Condition*: required / *Type*: WebElement /

  Winapp GUI element object.
      """
      self.ele_obj = ele_obj
      self.driver = driver

      self.config_dict = {
         "hold delay": 1000,  # milliseconds
         "zoom step": 50
      }

      self.non_ele_action_supported_dict = {

      }

      self.ele_action_supported_dict = {
         "mouse click": self._click,
         "get text": self._get_text,
         "get visible": self._get_visible,
         "get enable": self._get_enabled
      }

      self.supported_action_dict = {}
      self.supported_action_dict.update(self.non_ele_action_supported_dict)
      self.supported_action_dict.update(self.ele_action_supported_dict)

   @classmethod
   def get_supported_level(cls, _ele_obj):
      """
Get the supported level of this handler for a specific element object.

**Arguments:**

* ``ele_obj``

  / *Condition*: required / *Type*: WebElement /

  Winapp GUI element object.

**Returns:**

  / *Type*: int /

  Supported level of this action handler for the element object.
      """
      return cls.SUPPORTED_LEVEL

   def _click(self):
      """
Click element object action.

**Returns:**

(*no returns*)
      """
      self.ele_obj.click()

   def _get_text(self):
      """
Get element object's text action.

**Returns:**

  / *Type*: str /

  Text of the element object.
      """
      return self.ele_obj.text

   def _get_visible(self):
      """
Get element object's visibility.

**Returns:**

  / *Type*: bool /

  Visibility of the element object.
      """
      return self.ele_obj.is_displayed()

   def _get_enabled(self):
      """
Get element object's enabled property.

**Returns:**

  / *Type*: bool /

  Enabled property of the element object.
      """
      return self.ele_obj.is_enabled()

   def get_attribute(self, attr):
      """
Get element object's property.

**Arguments:**

* ``attr``

  / *Condition*: required / *Type*: str /

  Property's name to be got value.

**Returns:**

  / *Type*: str /

  Property's value.
      """
      return self.ele_obj.get_attribute(attr)

   def divert_action(self, action):
      """
Divert action string to the corresponding method, execute the method and return value.

**Arguments:**

* ``action``

  / *Condition*: required / *Type*: str /

  Action string to be diverted.

**Returns:**

  / *Type*: str /

  Corresponding method's return.
      """
      args = action.split(" ", 2)
      action_set_get = args[0]
      action_cmd = args[1]
      action_args = ()
      if len(args) > 2:
         action_args = (args[2],)
      action_perform = action_set_get + ' ' + action_cmd
      if action_perform not in self.non_ele_action_supported_dict and self.ele_obj is None:
        raise WebDriverException("NoElementFoundException")

      if action_perform in list(self.supported_action_dict.keys()):
         res_obj = self.supported_action_dict[action_perform](*action_args)
      elif action.startswith("get"):
         res_obj = self.get_attribute(action_cmd)
      else:
         raise Exception("Unsupported action '%s' for '%s'" % (action_cmd, self.ele_obj.get_attribute("className") ))

      return res_obj

qconnectwinapp-winappdriver-winapp-client-customwebdriver
=========================================================

*Imported by*:

.. code::python

   from QConnectWinapp.WinappDriver.winapp_client import CustomWebDriver

Customer WebDriver class for Winapp.
   
qconnectwinapp-winappdriver-winapp-client-customwebdriver-start-session
-----------------------------------------------------------------------

Creates a new session with the desired capabilities.

Override for Winapp

**Arguments:**

* ``capabilities``

  / *Condition*: required / *Type*: Union /

  Read https://github.com/appium/appium/blob/master/docs/en/writing-running-appium/caps.md for more details.


* ``browser_profile``

  / *Condition*: optional / *Type*: str / *Default*: None /

  Browser profile
      
qconnectwinapp-winappdriver-winapp-client-winappconfig
======================================================

*Imported by*:

.. code::python

   from QConnectWinapp.WinappDriver.winapp_client import WinappConfig

Class to store the configuration for SSH connection.
   
qconnectwinapp-winappdriver-winapp-client-winappclient
======================================================

*Imported by*:

.. code::python

   from QConnectWinapp.WinappDriver.winapp_client import WinAppClient

Winapp client connection class.
   
qconnectwinapp-winappdriver-winapp-client-winappclient-connect
--------------------------------------------------------------

Connect to WinappDriver which is listening on configured port.

**Returns:**

(*no returns*)
      
qconnectwinapp-winappdriver-winapp-client-winappclient-perform-action
---------------------------------------------------------------------

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
      
qconnectwinapp-winappdriver-winapp-client-winappclient-send-obj
---------------------------------------------------------------

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
      
qconnectwinapp-winappdriver-winapp-client-winappclient-wait-4-trace
-------------------------------------------------------------------

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
      
qconnectwinapp-winappdriver-winapp-client-winappclient-disconnect
-----------------------------------------------------------------

Abstract method for disconnecting connection.

**Arguments:**

* ``_device``

   / *Condition*: required / *Type*: str /

   Unused.

**Returns:**

(*no returns*)
      
qconnectwinapp-winappdriver-winapp-client-winappclient-quit
-----------------------------------------------------------

Quiting the connection.

**Returns:**

(*no returns*)
      

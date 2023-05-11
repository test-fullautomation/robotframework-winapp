qconnectwinapp-actionhandlers-element-handler-elementactionhandler
==================================================================

*Imported by*:

.. code::python

   from QConnectWinapp.ActionHandlers.element_handler import ElementActionHandler

qconnectwinapp-actionhandlers-element-handler-elementactionhandler-get-supported-level
--------------------------------------------------------------------------------------

Get the supported level of this handler for a specific element object.

**Arguments:**

* ``ele_obj``

  / *Condition*: required / *Type*: WebElement /

  Winapp GUI element object.

**Returns:**

  / *Type*: int /

  Supported level of this action handler for the element object.
      
qconnectwinapp-actionhandlers-element-handler-elementactionhandler-get-attribute
--------------------------------------------------------------------------------

Get element object's property.

**Arguments:**

* ``attr``

  / *Condition*: required / *Type*: str /

  Property's name to be got value.

**Returns:**

  / *Type*: str /

  Property's value.
      
qconnectwinapp-actionhandlers-element-handler-elementactionhandler-divert-action
--------------------------------------------------------------------------------

Divert action string to the corresponding method, execute the method and return value.

**Arguments:**

* ``action``

  / *Condition*: required / *Type*: str /

  Action string to be diverted.

**Returns:**

  / *Type*: str /

  Corresponding method's return.
      

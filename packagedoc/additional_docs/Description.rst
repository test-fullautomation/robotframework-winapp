.. Copyright 2020-2023 Robert Bosch GmbH

.. Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

.. http://www.apache.org/licenses/LICENSE-2.0

.. Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

*QConnectionWinapp*

Getting Started
---------------

You can checkout all
`QConnectionWinapp <https://sourcecode.socialcoding.bosch.com/projects/ROBFW/repos/robotframework-qconnect-dlt/browse>`__
sourcecode from the Bosch Internal Open Source Repositories.

After checking out the source completely, you can install by running
below command inside **robotframework-qconnect-winapp** directory.

::

   python setup.py install

Usage
-----

QConnectDLTLibrary is just a backend extension for QConnectBase support DLT connection. Therefore, in the user view, the QConnectDLTLibrary provive one more Connection Type for DLT Connection
when using **connect** keyword.

**connect**
~~~~~~~~~~~~~~

  **Use for establishing a connection.**

  **Syntax**:

   **connect** ``[conn_name]   [conn_type]   [conn_mode]   [conn_conf]``
   *(All parameters are required to be in order)*\

   or

   **connect**
   ``conn_name=[conn_name]   conn_type=[conn_type]   conn_mode=[conn_mode]   conn_conf=[conn_conf]``
   *(All parameters are assigned by name)*

  **Arguments**:

    **conn_name**: Name of the connection.

    **conn_type**: Type of the connection. QConnectBaseLibrary has supported below connection types:

        *  **TCPIPClient**: Create a Raw TCPIP connection to TCP Server.
        *  **SSHClient**: Create a client connection to a SSH server.
        *  **SerialClient**: Create a client connection via Serial Port.

    **conn_mode**: (unused) Mode of a connection type.

    **conn_conf**: Configurations for making a connection. Depend on **conn_type** (Type of Connection), there is a suitable configuration in JSON format as below.

        *  **TCPIPClient**

        ::

         {
             "address": [server host], # Optional. Default value is "localhost".
             "port": [server port]     # Optional. Default value is 1234.
             "logfile": [Log file path. Possible values: 'nonlog', 'console', <user define path>]
          }

        *  **SSHClient**

        ::

          {
              "address" : [server host],  # Optional. Default value is "localhost".
              "port" : [server host],     # Optional. Default value is 22.
              "username" : [username],    # Optional. Default value is "root".
              "password" : [password],    # Optional. Default value is "".
              "authentication" : "password" | "keyfile" | "passwordkeyfile",  # Optional. Default value is "".
              "key_filename" : [filename or list of filenames], # Optional. Default value is null.
              "logfile": [Log file path. Possible values: 'nonlog', 'console', <user define path>]
           }

        *  **SerialClient**

        ::

          {
              "port" : [comport or null],
              "baudrate" : [Baud rate such as 9600 or 115200 etc.],
              "bytesize" : [Number of data bits. Possible values: 5, 6, 7, 8],
              "stopbits" : [Number of stop bits. Possible values: 1, 1.5, 2],
              "parity" : [Enable parity checking. Possible values: 'N', 'E', 'O', 'M', 'S'],
              "rtscts" : [Enable hardware (RTS/CTS) flow control.],
              "xonxoff" : [Enable software flow control.],
              "logfile": [Log file path. Possible values: 'nonlog', 'console', <user define path>]
           }

        *  **DLT**

        ::

         {
             "gen3flex@DLTConnector": {
                 "target_ip": [target ip],     # Optional. Default value is "172.17.0.1".
                 "target_port": [target port]  # Optional. Default value is 3490.
                 "mode": [0 or 1],             # 0 is via TCP, 1 is via COM port. Default value is 0.
                 "ecu": [ecu name],            # Optional. Default value is "ECU1".
                 "com_port": [COM port name],  # Optional. Default value is "COM1".
                 "baudrate": [Baud rate such as 9600 or 115200 etc.],
                 "trcfile": [trace file path]  # Optional. Default value is "".
                 "logfile": [Log file path. Possible values: 'nonlog', 'console', <user define path>]
             }
         }

Example
-------

::

   *** Settings ***
   Documentation    Suite description
   Library     QConnectBase.ConnectionManager
   Suite Teardown  Close Connection

   *** Variables ***
   ${CONNECTION_NAME}  TEST_CONN

   *** Test Cases ***
   Test DLT Connection
       # Create config for connection.
       ${config_string}=    catenate
       ...  {
       ...      "gen3flex@DLTConnector": {
       ...            "target_ip": "127.0.0.1",
       ...            "target_port": 3490,
       ...            "mode": 0,
       ...            "ecu": "ECU1",
       ...            "com_port": "COM1",
       ...            "baudrate": 115200,
       ...            "server_ip": "localhost",
       ...            "server_port": 1234
       ...      }
       ...  }
       log to console       \nConnecting with configurations:\n${config_string}
       ${config}=             evaluate        json.loads('''${config_string}''')    json

       # Connect to the target with above configurations.
       # connect     test_dlt  DLT   dltconnector  ${config}
       connect             conn_name=${CONNECTION_NAME}
       ...                 conn_type=DLT
       ...                 conn_mode=dltconnector
       ...                 conn_conf=${config}

       # Send command 'TR_TEST_CONNECTION' and wait for the response which match with the 'get connection' pattern.
       ${res}=     verify                  conn_name=${CONNECTION_NAME}
       ...                                 search_pattern=get connection
       ...                                 send_cmd=TR_TEST_CONNECTION
       log to console     ${res}

       # Disconnect.
       disconnect  ${CONNECTION_NAME}

   *** Keyword ***
   Close Connection
       disconnect  ${CONNECTION_NAME}


Contribution Guidelines
-----------------------


QConnectBaseLibrary is designed for ease of making an extension library. By that way you can take advantage of the QConnectBaseLibrary’s
infrastructure for handling your own connection protocal. For creating an extension library for QConnectBaseLibrary, please following below
steps.

1.  Create a library package which have the prefix name is **robotframework-qconnect-**\ *[your specific name]*.

2.  Your hadling connection class should be derived from **QConnectionLibrary.connection_base.ConnectionBase**  class.

3.  In your *Connection Class*, override below attributes and methods:

  -  **_CONNECTION_TYPE**: name of your connection type. It will be the input of the conn_type argument when using **connect** keyword. Depend on the type name, the library will detemine the correct connection handling class.

  -  **__init__(self, \_mode, config)**: in this constructor method, you should:

    - Prepare resource for your connection.
    - Initialize receiver thread by calling **self._init_thread_receiver(cls._socket_instance, mode="")** method.
    - Configure and initialize the lowlevel receiver thread (if it’s necessary) as below

      ::

        self._llrecv_thrd_obj = None
        self._llrecv_thrd_term = threading.Event()
        self._init_thrd_llrecv(cls._socket_instance)


    - Incase you use the lowlevel receiver thread. You should implement the **thrd_llrecv_from_connection_interface()** method. This method is a mediate layer which will receive the data from connection at the very beginning, do some process then put them in a queue for the **receiver thread** above getting later.
    - Create the queue for this connection (use Queue.Queue).

  - **connect()**: implement the way you use to make your own connection protocol.
  - **_read()**: implement the way to receive data from connection.
  - **_write()**: implement the way to send data via connection.
  - **disconnect()**: implement the way you use to disconnect your own connection protocol.
  - **quit()**: implement the way you use to quit connection and clean resource.

Configure Git and correct EOL handling
--------------------------------------

Here you can find the references for `Dealing with line
endings <https://help.github.com/articles/dealing-with-line-endings/>`__.

Every time you press return on your keyboard you’re actually inserting
an invisible character called a line ending. Historically, different
operating systems have handled line endings differently. When you view
changes in a file, Git handles line endings in its own way. Since you’re
collaborating on projects with Git and GitHub, Git might produce
unexpected results if, for example, you’re working on a Windows machine,
and your collaborator has made a change in OS X.

To avoid problems in your diffs, you can configure Git to properly
handle line endings. If you are storing the .gitattributes file directly
inside of your repository, than you can asure that all EOL are manged by
git correctly as defined.

Sourcecode Documentation
------------------------

For investigating sourcecode, please refer to `QConnectWinapp
Documentation <docs/html/index.html>`__

Feedback
--------

If you have any problem when using the library or think there is a
better solution for any part of the library, I’d love to know it, as
this will all help me to improve the library. Connect with me at
cuong.nguyenhuynhtri@vn.bosch.com.

Do share your valuable opinion, I appreciate your honest feedback!

About
-----

Maintainers
~~~~~~~~~~~

`Nguyen Huynh Tri Cuong <cuong.nguyenhuynhtri@vn.bosch.com>`__

Contributors
~~~~~~~~~~~~

`Nguyen Huynh Tri Cuong <cuong.nguyenhuynhtri@vn.bosch.com>`__

`Thomas Pollerspoeck <thomas.pollerspoeck@de.bosch.com>`__


3rd Party Licenses
~~~~~~~~~~~~~~~~~~

You must mention all 3rd party licenses (e.g. OSS) licenses used by your
project here. Example:

+-------------------------------------------------+-------------------------------------------------------------------------+------------+
| Name                                            | License                                                                 | Type       |
+=================================================+=========================================================================+============+
| `Apache Felix <http://felix.apache.org/>`_.     | `Apache 2.0 License <http://www.apache.org/licenses/LICENSE-2.0.txt>`_. | Dependency |
|                                                 |                                                                         |            |
|                                                 |                                                                         |            |
|                                                 |                                                                         |            |
+-------------------------------------------------+-------------------------------------------------------------------------+------------+


Used Encryption
~~~~~~~~~~~~~~~

Declaration of the usage of any encryption (see BIOS Repository Policy
§4.a).

License
~~~~~~~

|License: BIOSL v4|

   Copyright (c) 2009, 2018 Robert Bosch GmbH and its subsidiaries. This
   program and the accompanying materials are made available under the
   terms of the Bosch Internal Open Source License v4 which accompanies
   this distribution, and is available at
   http://bios.intranet.bosch.com/bioslv4.txt

.. raw:: html

   <!---

       Copyright (c) 2009, 2018 Robert Bosch GmbH and its subsidiaries.
       This program and the accompanying materials are made available under
       the terms of the Bosch Internal Open Source License v4
       which accompanies this distribution, and is available at
       http://bios.intranet.bosch.com/bioslv4.txt

   -->

.. |License: BIOSL v4| image:: http://bios.intranet.bosch.com/bioslv4-badge.svg
   :target: #license


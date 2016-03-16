httppp
======

ABOUT
-----

HTTP Passive Probe (httppp) is a network capture analyzer which extract HTTP
request response times from a previously recorded network capture file.

See web site: http://httppp.g76r.eu/

LICENSE
-------

This software is made availlable to you under the terms of the GNU Affero
General Public License version 3, see LICENSE.txt file content.

BUILD INSTRUCTIONS
------------------

The git repository uses git submodules to handle libqpcap dependency, therefore
you should use following steps to clone repository and submodules repositories:

``` bash
git clone git@gitlab.com:g76r/httppp.git
cd httppp
git submodule init
git submodule update
```

Then one should build libqpcap before httppp since it's a dependency.
This can be done with the top directory qmake project or with any external
programm such as an IDE like Qt Creator:

``` bash
qmake
make
```

Dependencies:
- on POSIX systems one should first install libpcap dev kit, e.g.
  "apt-get install libpcap-dev" or "yum install libpcap-devel" under Linux
- on Windows systems one should install WinPCAP devkit and place it under
  c:\WpdPack directory
- Qt 4.8


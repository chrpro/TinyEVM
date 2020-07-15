This is a git-versioned subset and slightly modified version of TI's CC13xxware.

This repository's sole purpose is to provide CC13xxware as a submodule for the [Contiki Operating System](https://github.com/contiki-os/contiki/).

New versions will only appear in this repository only if and when Contiki's CC13xx port needs to use them.

Modifications (for current and upcoming versions):

* Only files used by Contiki are included here. Documentation and files related to IAR and CCS have been removed.
* Line-endings have been converted to Unix
* File permissions have been changed to 644

All sources are and will remain otherwise intact, except in case where modifications are required by Contiki.

CC13xxware is distributed by TI as part of [TI-RTOS](http://www.ti.com/tool/ti-rtos). If you are looking for the latest version of CC13xxware, do not clone this repository. You should download TI-RTOS instead.

Do not open pulls / issues on this repository, unless they are immediately related to using CC13xxware with Contiki.

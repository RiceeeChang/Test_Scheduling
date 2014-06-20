Test Scheduling for Core-based SoC
==================================

This project is our term project for class
*Design and Analysis of  Algorithms* in Spring Semester of 2013.
In this project, we aim to optimize test scheduling for System-on-Chip
testing. Detailed problem specification can be found
`here <http://cad_contest.ee.ncu.edu.tw/problem_e/default.html>`_.


Build the Project
-----------------

Prerequisite
~~~~~~~~~~~~

This project is implemented in C++ and complys with ISO C++03 Standard.
We did not use any external libraries or system calls. Hence, only a
C++ compiler is required. Besides, we provide a simple Makefile for
auto building via GNU g++ compiler and GNU Make.


Compilation
~~~~~~~~~~~

For platform supporting GNU Make, type::

    $ make

The produced executable should be at folder "bin".


Usage
-----

The usage of our program is::

	$ ./bin/TestScheduler <soc_name>.info

It will produce output file "<soc_name>.schedule" in the same folder of
input file.

Example::

	$ ./bin/TestScheduler inputs/demo.info

The output file will be "demo.schedule" and located in "inputs/" folder.


About Us
--------

| 謝橋,
| Graduate Institute of Electronics Engineering,
| National Taiwan University

| 吳庭棻,
| Graduate Institute of Eletrical Engineering,
| National Taiwan University

| 張梵清,
| Graduate Institute of Eletrical Engineering,
| National Taiwan University

If you are interested in our work, please contact 
謝橋<r02943142@ntu.edu.tw>

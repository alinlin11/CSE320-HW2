# CSE320-HW2
Homework 2 Debugging and Fixing - CSE 320 - Spring 2023

Professor Eugene Stark

Due Date: Friday 3/3/2023 @ 11:59pm


Introduction
In this assignment you are tasked with updating an old piece of
software, making sure it compiles, and that it works properly
in your VM environment.
Maintaining old code is a chore and an often hated part of software
engineering. It is definitely one of the aspects which are seldom
discussed or thought about by aspiring computer science students.
However, it is prevalent throughout industry and a worthwhile skill to
learn.  Of course, this homework will not give you a remotely
realistic experience in maintaining legacy code or code left behind by
previous engineers but it still provides a small taste of what the
experience may be like.  You are to take on the role of an engineer
whose supervisor has asked you to correct all the errors in the
program, plus add additional functionality.
By completing this homework you should become more familiar
with the C programming language and develop an understanding of:

How to use tools such as gdb and valgrind for debugging C code.
Modifying existing C code.
C memory management and pointers.
Working with files and the C standard I/O library.

I am sure you will be thrilled to know that we have not yet finished
with "diffs" and "patching" for this semester.
In this assignment, you will be working with the source code for
Larry Wall's original patch program
(Wikipedia)[https://en.wikipedia.org/wiki/Patch_(Unix)].
In brief, your objectives in this assignment are as follows:


Make such updates to the original code as are necessary to get it to
compile using gcc, with the compiler flags as set in the Makefile
distributed in the basecode.


Fix bugs in the compiled program, until it passes the test cases
that are provided with the basecode.


Make some improvements to the program, as described in more detail below.


As you work on the program, limit the changes you make to the minimum necessary
to achieve the specified objectives.  Don't completely rewrite the program;
assume that it is essentially correct and just fix a few compilation errors and
bugs as described below.  You will likely find it helpful to use git for this (I did).
Make exploratory changes first on a side branch (i.e. not the master branch),
then when you think you have understood the proper changes that need to be made,
go back and apply those changes to the master branch.  Using git will help you
to back up if you make changes that mess something up.

The Original Program
Larry Wall's famous patch program was first published in 1985.
It is written in a C dialect that is kind of a transitional version between
K+R C and what would a few years later become ANSI C.
The basecode version has been minimally modified.  I edited it to change the
name of the original main() function to orig_main() and to add a new main.c
file so that it can ultimately be tested with Criterion.
Also, I modified a few lines of code to introduce some "typical" bugs
to make things more interesting and educational for you 😉.
However, in order to make the code compile and function on a modern system,
there are already several interesting problems that you will have to solve.
The point is, the code as it stands originally functioned, so you should
be looking to make minimum changes to make the program functional again,
not to rewrite the entire program.
This program was written many years ago, and I would not necessarily put forth
this particular piece of code as a good example of modern-day C coding style.
However, it has been quite carefully written, and a close inspection reveals
that the author employed elements of various software engineering ideas, such
as abstract data types, which were relatively new at the time.
Nevertheless, the overall style in which people write C programs has evolved
significantly since then.  Although this program is probably not so different
from the way other C programs looked back in the day, a modern C program would
most likely have many fewer global variables, and would adopt a somewhat more
definite object-oriented style.

Tasks to Perform

Getting Started - Obtain the Base Code
Fetch base code for hw2 as you did for the previous assignments.
You can find it at this link:
https://gitlab02.cs.stonybrook.edu/cse320/hw2.
Once again, to avoid a merge conflict with respect to the file .gitlab-ci.yml,
use the following command to merge the commits:
  git merge -m "Merging HW2_CODE" HW2_CODE/master --strategy-option=theirs


🤓 I hope that by now you would have read some git documentation to find
out what the --strategy-option=theirs does, but in case you didn't 😠
I will say that merging in git applies a "strategy" (the default strategy
is called "recursive", I believe) and --strategy-option allows an option
to be passed to the strategy to modify its behavior.  In this case, theirs
means that whenever a conflict is found, the version of the file from
the branch being merged (in this case HW2_CODE/master) is to be used in place
of the version from the currently checked-out branch.  An alternative to
theirs is ours, which makes the opposite choice.  If you don't specify
one of these options, git will leave conflict indications in the file itself
and it will be necessary for you to edit the file and choose the code you want
to use for each of the indicated conflicts.

Here is the structure of the base code:
.
├── .gitlab-ci.yml
└── hw2
    ├── doc
    │   └── patch.man
    ├── .gitignore
    ├── include
    │   └── debug.h
    ├── Makefile
    ├── src
    │   ├── main.c
    │   └── patch.c
    ├── test_output
    │   └── .git-keep
    └── tests
        ├── basecode_tests.c
        ├── rsrc
        │   ├── all_failed_diff
        │   ├── all_failed_in
        │   ├── all_failed_out
        │   ├── askname.in
        │   ├── context_diff
        │   ├── context.in
        │   ├── context_in
        │   ├── context_multi.in
        │   ├── context_multi_in
        │   ├── context_multi_out
        │   ├── context_out
        │   ├── garbage_diff_diff
        │   ├── garbage_diff_in
        │   ├── ifile_test_input
        │   ├── locate_normal_patch
        │   ├── locate_test_input
        │   ├── nonexistent_diff_file_in
        │   ├── nonexistent_input_file_diff
        │   ├── normal_diff
        │   ├── normal_in
        │   ├── normal_out
        │   ├── pfile_context_patch
        │   ├── pfile_normal_patch
        │   ├── valgrind_all_failed_diff
        │   ├── valgrind_all_failed_in
        │   ├── valgrind_all_failed_out
        │   ├── valgrind_context_multi.in
        │   ├── valgrind_context_multi_in
        │   ├── valgrind_context_multi_out
        │   ├── valgrind_garbage_diff
        │   ├── valgrind_garbage_in
        │   ├── valgrind_normal_diff
        │   ├── valgrind_normal_in
        │   └── valgrind_normal_out
        ├── test_common.c
        └── test_common.h

The src directory contains the C source code file patch.c, which contains the
original code.  In addition, I have added a new file main.c, with a single
main() function that simply calls orig_main() in patch.c.  This is to satisfy our
requirement (for Criterion) that main() is the only function in main.c.
The include directory contains only the C utility header file debug.h, as the
original code consisted of a single C source file with no header file.
The doc directory contains the source code version of the "man page" for the
patch program.  It is coded in the typesetting language nroff, which is still
used today for Unix/Linux man pages.  To format the man page for output to a terminal,
use nroff -man doc/patch.man.  To produce Postscript output, you can use
groff -man doc/patch.man.
The tests directory contains C source code (in file basecode_tests.c) for some
Criterion tests that will guide you toward bugs in the program.
Your objective is to make the program pass all these test cases; you need not
look for additional bugs once that has been achieved.
Some of the test cases are true unit tests, which invoke individual functions
in the program.  Others are so-called "black box" tests, which test the input-output
behavior of the program running as a separate process from the test driver.
The test_common.c file contains helper functions for launching an instance of patch
as a separate process, redirecting stdin from an input file, collecting the
output produced on stdout and stderr, checking the exit status of the program,
and comparing the output against reference output.
The tests/rsrc subdirectory contains files used by the tests in basecode_tests.c.
The test_output directory is a "dummy" directory which is used to hold the output
produced when you run the Criterion tests.  Look there if you want to understand,
for example, why the tests reported that the output produced by your program was
not as expected.
Before you begin work on this assignment, you should read the rest of this
document.  In addition, we additionally advise you to read the
Debugging Document.  One of the main goals of this assignment
is to get you to learn how to use the gdb debugger, so you should right away
be looking into how to use this while working on the tasks in the following sections.
Another goal is to learn to use the valgrind tool to check for various kinds
of memory-access related errors.  Learning to use these tools has the potential
to save you a large amount of debugging time in the future.

Part 1: Debugging and Fixing
You are to complete the following steps:


Clean up the code; fixing any compilation issues, so that it compiles
without error using the compiler options that have been set for you in
the Makefile.
Use git to keep track of the changes you make and the reasons for them, so that you can
later review what you have done and also so that you can revert any changes you made that
don't turn out to be a good idea in the end.


Fix bugs.
Use the test cases supplied to identify program failures (this is discussed further
below).  Use gdb and valgrind to locate and fix the bugs, making the minimum
changes necessary.  Do not make gratuitous changes to the program output, as this
will interfere with our ability to test your code.
It will likely be necessary for you to study the code in some detail in order to
understand what might be the proper way to fix some of the bugs.  This is part of
the assignment.

😱 You are NOT allowed to share or post on PIAZZA
solutions to the bugs in this program, as this defeats the point of
the assignment. You may provide small hints in the right direction,
but nothing more.

You will likely find the valgrind tool to be extremely useful.
As has been done in the test cases, you can run valgrind using a command of the
following form:
    $ valgrind --leak-check=full --show-leak-kinds=all --undef-value-errors=yes [PATCH PROGRAM AND ARGS]
 
Valgrind has many options and capabilities -- it is recommended to skim through
the man page ("man valgrind") to get an overview of them.



Part 2: Rewrite/Extend Options Processing
The basecode version of patch performs its own ad hoc processing of command-line
arguments (in the function get_some_switches()).
Partly this is due to the fact that there did not exist a commonly accepted library
package for performing this function at the time the program was written.
However, as options processing is a common function that is performed by most programs,
and it is desirable for programs on the same system to be consistent in how they interpret
their arguments, there have been more elaborate standardized libraries that have been written
for this purpose.  In particular, the POSIX standard specifies a getopt() function,
which you can read about by typing man 3 getopt.  A significant advantage to using a
standard library function like getopt() for processing command-line arguments,
rather than implementing ad hoc code to do it, is that all programs that use
the standard function will perform argument processing in the same way
rather than having each program implement its own quirks that the user has to remember.
For this part of the assignment, you are to replace the original argument-processing
code in get_some_switches() by code that uses the GNU getopt library package.
In addition to the POSIX standard getopt() function, the GNU getopt package
provides a function getopt_long() that understands "long forms" of option
arguments in addition to the traditional single-letter options.
In your revised program, get_some_switches() should use getopt_long() to traverse
the command-line arguments, and it should support the same set of options as
the original program.  Note that for the original program, the order of the
arguments was significant, and you should preserve that behavior.
Also, as in the original program, when an argument is encountered that consists
of the single-character string "+", then option processing is suspended, possibly
to be resumed later if the patch file contains more than one patch.
See the documentation for patch for a description of this behavior, and see
the man page for getopt() to find out how to modify its default behavior so that
the order of arguments is preserved.
Besides the traditional single-letter options, your revised program should also
support the following long forms:


--backup-extension (long form of -b)

--context-diff (long form of -c)

--directory (long form of -d)

--do-defines (long form of -D)

--ed-script (long form of -e)

--loose-matching (long form of -l)

--normal-diff (long form of -n)

--output-file (long form of -o)

--pathnames (long form of -p)

--reject-file (long form of -r)

--reverse (long form of -R)

--silent (long form of -s)

--debug (long form of -x)

You will probably need to read the Linux "man page" on the getopt package.
This can be accessed via the command man 3 getopt.  If you need further information,
search for "GNU getopt documentation" on the Web.

😱 You MUST use the getopt_long() function to process the command line
arguments passed to the program.  Your program should be able to handle cases where
the (non-positional) flags are passed IN ANY order.  Make sure that you test the
program with prefixes of the long option names, as well as the full names.


Notes on Getting the Program to Compile
Although I want to leave this mostly as an exercise for you, there are a few
things that I feel might be hard for you to figure out unless I mention them here.


You will run into some issues with "printf-like" functions(say(), fatal(), ask())
that take a variable number of arguments.  Such functions are called
variadic functions in modern jargon.  In order to handle these properly in
ANSI C, you should use the features it provides for variadic functions.
In particular, ... is used in a function prototype to indicate that a function may
take a variable number of arguments beyond those explicitly listed.
Library functions (known as "variable argument list" or "varargs" support)
are used to access the extra arguments in a type-safe fashion.
You should refer to C language reference material and the Linux "stdarg"
man page (man 3 stdarg) for more information.  You will also find it useful
to read about the vfprintf() form of the fprintf() function (man 3 vfprintf).


The program uses the signal() function to set a signal handler.  We will study
signal handling later in the semester; for now I will just say that will be necessary
to use a cast to coerce the function argument passed to signal() to the
required type.  The man page man 3 signal may be useful here.


The program uses low-level Unix I/O system calls
(creat(), stat(), open(), close(), read(), write(), unlink())
to manipulate files.  We will also cover these later in the semester.
You will probably need to look at their man pages to find the proper header files
that need to be included, but you don't really have to understand much about
these functions and the bugs in the program aren't related to them.


The program has features that relate to the (then popular, but now obsolete)
"SCCS" revision control system, as well as patches that take the form of scripts
to be passed to the traditional Unix editor ed (which I still use quite regularly,
by the way).  You do not have to worry about understanding these.



Notes on Testing the Program
For this assignment, you have been provided with a basic set of
Criterion tests to guide you to the bugs.
You can run these tests with the following command:
    $ bin/patch_tests

To obtain more information about each test run, you can supply the
additional option --verbose=1.
You can also specify the option -j1 to cause the tests to be run sequentially,
rather than in parallel using multiple processes, as is the default.
The -j1 flag is necessary if the tests could interfere with each other in
some way if they are run in parallel (such as writing the same output file).
You will probably find it useful to know this; however the basecode tests have
been written so that they each use output files named after the test and
(hopefully) will not interfere with each other.
The tests have been constructed so that they will point you at problems with
the program.
Each test has one or more assertions to make sure that the code functions
properly.  If there was a problem before an assertion, such as a "segfault",
the test will print the error to the screen and continue to run the
rest of the tests.
Some of the test cases are true unit tests, which directly invoke functions
in the program and make assertions about the results.
The other test cases are "black-box" tests, which run the program with
various arguments, redirecting stdin from a predefined input file,
redirecting stdout and stderr to output files,
and comparing the output produced against pre-defined reference files.
Some of the tests use valgrind to verify that no memory-access errors are found.
If errors are found, then you can look at the log file that is left behind
(in the test_output directory) by the test code.
Alternatively, you can better control the information that valgrind provides
if you run it manually.
You should be able to follow the pattern to construct some additional tests of
your own, and you might find this helpful while working on the program.
You are encouraged to try to write some of these tests so that you learn how
to do it.  Note that in the next homework assignment unit tests will likely
be very helpful to you and you will be required to write some of your own.
Criterion documentation for writing your own tests can be found
here.

Hand-in Instructions
Ensure that all files you expect to be on your remote repository are committed
and pushed prior to submission.
This homework's tag is: hw2
$ git submit hw2

#include <stdio.h>
#include <unistd.h>

#include <criterion/criterion.h>
#include <criterion/logging.h>

#include "test_common.h"
#include "debug.h"

extern int Argc;
extern char **Argv;
extern FILE *rejfp;
extern int filec;
extern char *filearg[];
extern char TMPINNAME[];
extern char using_plan_a;
extern char verbose;
extern void get_some_switches();
extern void scan_input(char *filename);
extern void open_patch_file(char *filename);
extern int intuit_diff_type();
extern bool plan_a(char *filename);
extern bool plan_b(char *filename);
extern char *ifetch(int line, int whichbuf);
extern char *pfetch(int line);
extern bool another_hunk();
extern long locate_hunk();
extern void abort_hunk();
extern void skip_to(long file_pos);
extern long pch_start();
extern long pch_end();

#define CONTEXT_DIFF 1
#define NORMAL_DIFF 2
#define ED_DIFF 3
extern int diff_type;

int linecmp(char *l1, char *l2) {
    while(*l1 == *l2) {
	if(*l1++ == '\n' || *l2++ == '\n')
	    return 0;
    }
    return 1;
}

Test(args_suite, args) {
    char *av[] = { "prog", "-x2", "diff", "-n", "input", NULL };
    Argv = av;
    Argc = sizeof(av)/sizeof(av[0]) - 1;
    get_some_switches();
    cr_assert_eq(diff_type, NORMAL_DIFF,
		 "The value of diff_type was %d, not the expected value %d\n",
		 diff_type, NORMAL_DIFF);
    cr_assert_eq(filec, 2, "The value of filec was %d, not the expected value %d\n",
		 filec, 2);
    cr_assert_eq(strcmp(filearg[0], av[2]), 0,
		 "The value of filearg[0] was '%s', not the expected value '%s'\n",
		 filearg[0], av[2]);
    cr_assert_eq(strcmp(filearg[1], av[4]), 0,
		 "The value of filearg[1] was '%s', not the expected value '%s'\n",
		 filearg[1], av[4]);
}

Test(ifile_suite, ifile_plan_a) {
    plan_a("tests/rsrc/ifile_test_input");
    cr_assert(using_plan_a, "The value of using_plan_a was 0, when nonzero was expected\n");
    char *line = ifetch(3, 0);
    char *exp = "This is test_input line 3\n";
    if(linecmp(line, exp)) {
	fprintf(stderr, "Line returned by ifetch did not match expected:\nLine:\t");
	while(*line != '\n')
	    fputc(*line++, stderr);
	fputc('\n', stderr);
	fprintf(stderr, "Exp:\t");
	while(*exp != '\n')
	    fputc(*exp++, stderr);
	fputc('\n', stderr);
	cr_assert_fail();
    }
}

Test(ifile_suite, ifile_plan_b) {
    mkstemp(TMPINNAME);
    plan_b("tests/rsrc/ifile_test_input");
    unlink(TMPINNAME);
    cr_assert_eq(using_plan_a, 0,
		 "The value of using_plan_a was %d, when zero was expected\n",
		 using_plan_a);
    char *line = ifetch(3, 0);
    char *exp = "This is test_input line 3\n";
    if(linecmp(line, exp)) {
	fprintf(stderr, "Line returned by ifetch did not match expected:\nLine:\t");
	while(*line != '\n')
	    fputc(*line++, stderr);
	fputc('\n', stderr);
	fprintf(stderr, "Exp:\t");
	while(*exp != '\n')
	    fputc(*exp++, stderr);
	fputc('\n', stderr);
	cr_assert_fail();
    }
}

Test(pfile_suite, pfile_normal) {
    rejfp = stderr;
    open_patch_file("tests/rsrc/pfile_normal_patch");
    diff_type = intuit_diff_type();
    cr_assert_eq(diff_type, NORMAL_DIFF,
		 "The intuited diff type was %d not the expected value %d\n",
		 diff_type, NORMAL_DIFF);
    long end, exp;
    skip_to(pch_start());
    cr_assert(another_hunk(), "There should have been another hunk\n");
    end = pch_end();
    exp = 4;
    cr_assert_eq(end, exp, "The hunk should have had length %ld (but %ld was seen)\n",
	      exp, end);
    abort_hunk();
    cr_assert(another_hunk(), "There should have been another hunk\n");
    end = pch_end();
    exp = 2;
    cr_assert_eq(end, exp, "The hunk should have had length %ld (but %ld was seen)\n",
		 exp, end);
    abort_hunk();
    cr_assert(another_hunk(), "There should have been another hunk\n");
    end = pch_end();
    exp = 2;
    cr_assert_eq(end, exp, "The hunk should have had length %ld (but %ld was seen)\n",
		 exp, end);
    abort_hunk();
}

Test(pfile_suite, pfile_context) {
    rejfp = stderr;
    open_patch_file("tests/rsrc/pfile_context_patch");
    diff_type = intuit_diff_type();
    cr_assert_eq(diff_type, CONTEXT_DIFF,
		 "The intuited diff type was %d not the expected value %d\n",
		 diff_type, CONTEXT_DIFF);
    long end, exp;
    skip_to(pch_start());
    cr_assert(another_hunk(), "There should have been another hunk\n");
    end = pch_end();
    exp = 10;
    cr_assert_eq(end, exp, "The hunk should have had length %ld (but %ld was seen)\n",
	      exp, end);
    abort_hunk();
}

Test(patch_suite, locate_hunk) {
    rejfp = stderr;
    plan_a("tests/rsrc/locate_test_input");
    cr_assert(using_plan_a, "The value of using_plan_a was 0, when nonzero was expected\n");
    open_patch_file("tests/rsrc/locate_normal_patch");
    diff_type = intuit_diff_type();
    cr_assert_eq(diff_type, NORMAL_DIFF,
		 "The intuited diff type was %d not the expected value %d\n",
		 diff_type, NORMAL_DIFF);
    skip_to(pch_start());
    cr_assert(another_hunk(), "There should have been another hunk\n");
    long where = locate_hunk();
    cr_assert(where != 0, "The hunk could not be located\n");
}

Test(blackbox_suite, normal) {
    char *name = "normal";
    sprintf(program_options, "-o %s/%s_out %s/%s_in %s/%s_diff",
	    TEST_OUTPUT_DIR, name, TEST_REF_DIR, name, TEST_REF_DIR, name);
    int err = run_using_system(name, "", "", STANDARD_LIMITS);
    assert_expected_status(EXIT_SUCCESS, err);
    assert_outfile_matches(name, NULL);
}

Test(blackbox_suite, context) {
    char *name = "context";
    sprintf(program_options, "-p -o %s/%s_out", TEST_OUTPUT_DIR, name);
    int err = run_using_system(name, "", "", STANDARD_LIMITS);
    assert_expected_status(EXIT_SUCCESS, err);
    assert_outfile_matches(name, NULL);
}

Test(blackbox_suite, context_multi) {
    char *name = "context_multi";
    sprintf(program_options, "-p -o %s/%s_out", TEST_OUTPUT_DIR, name);
    int err = run_using_system(name, "", "", STANDARD_LIMITS);
    assert_expected_status(EXIT_SUCCESS, err);
    assert_outfile_matches(name, NULL);
}

Test(blackbox_suite, askname) {
    char *name = "askname";
    sprintf(program_options, "%s", "");
    int err = run_using_system(name, "", "", STANDARD_LIMITS);
    assert_normal_exit(err);
    assert_expected_status(EXIT_FAILURE, err);
}

Test(blackbox_suite, nonexistent_diff_file) {
    char *name = "nonexistent_diff_file";
    sprintf(program_options, "-o %s/%s_out %s/%s_in %s/%s_diff",
	    TEST_OUTPUT_DIR, name, TEST_REF_DIR, name, TEST_REF_DIR, name);
    int err = run_using_system(name, "", "", STANDARD_LIMITS);
    assert_expected_status(EXIT_FAILURE, err);
}

Test(blackbox_suite, nonexistent_input_file) {
    char *name = "nonexistent_input_file";
    sprintf(program_options, "-o %s/%s_out %s/%s_in %s/%s_diff",
	    TEST_OUTPUT_DIR, name, TEST_REF_DIR, name, TEST_REF_DIR, name);
    int err = run_using_system(name, "", "", STANDARD_LIMITS);
    assert_expected_status(EXIT_FAILURE, err);
}

Test(blackbox_suite, garbage_diff) {
    char *name = "garbage_diff";
    sprintf(program_options, "-o %s/%s_out %s/%s_in %s/%s_diff",
	    TEST_OUTPUT_DIR, name, TEST_REF_DIR, name, TEST_REF_DIR, name);
    int err = run_using_system(name, "", "", STANDARD_LIMITS);
    // It seems that the intended exit status is EXIT_SUCCESS in this case.
    assert_expected_status(EXIT_SUCCESS, err);
}

Test(blackbox_suite, all_failed) {
    char *name = "all_failed";
    sprintf(program_options, "-o %s/%s_out %s/%s_in %s/%s_diff",
	    TEST_OUTPUT_DIR, name, TEST_REF_DIR, name, TEST_REF_DIR, name);
    int err = run_using_system(name, "", "", STANDARD_LIMITS);
    assert_expected_status(EXIT_SUCCESS, err);
    assert_outfile_matches(name, NULL);
}

Test(valgrind_suite, valgrind_normal) {
    char *name = "valgrind_normal";
    sprintf(program_options, "-o %s/%s_out %s/%s_in %s/%s_diff",
	    TEST_OUTPUT_DIR, name, TEST_REF_DIR, name, TEST_REF_DIR, name);
    int err = run_using_system(name, "", "valgrind --leak-check=full --undef-value-errors=yes --error-exitcode=37", STANDARD_LIMITS);
    assert_no_valgrind_errors(err);
    assert_expected_status(EXIT_SUCCESS, err);
    assert_outfile_matches(name, NULL);
}

Test(valgrind_suite, valgrind_garbage_diff) {
    char *name = "valgrind_garbage";
    sprintf(program_options, "-o %s/%s_out %s/%s_in %s/%s_diff",
	    TEST_OUTPUT_DIR, name, TEST_REF_DIR, name, TEST_REF_DIR, name);
    int err = run_using_system(name, "", "valgrind --leak-check=full --undef-value-errors=yes --error-exitcode=37", STANDARD_LIMITS);
    assert_no_valgrind_errors(err);
    // It seems that the intended exit status is EXIT_SUCCESS in this case.
    assert_expected_status(EXIT_SUCCESS, err);
}

Test(valgrind_suite, valgrind_all_failed) {
    char *name = "valgrind_all_failed";
    sprintf(program_options, "-o %s/%s_out %s/%s_in %s/%s_diff",
	    TEST_OUTPUT_DIR, name, TEST_REF_DIR, name, TEST_REF_DIR, name);
    int err = run_using_system(name, "", "valgrind --leak-check=full --undef-value-errors=yes --error-exitcode=37", STANDARD_LIMITS);
    assert_no_valgrind_errors(err);
    assert_expected_status(EXIT_SUCCESS, err);
    assert_outfile_matches(name, NULL);
}

Test(valgrind_suite, valgrind_context_multi) {
    char *name = "valgrind_context_multi";
    sprintf(program_options, "-p -o %s/%s_out", TEST_OUTPUT_DIR, name);
    int err = run_using_system(name, "", "valgrind --leak-check=full --undef-value-errors=yes --error-exitcode=37", STANDARD_LIMITS);
    assert_no_valgrind_errors(err);
    assert_expected_status(EXIT_SUCCESS, err);
    assert_outfile_matches(name, NULL);
}


////////////////////////////////////////////////////// TESTS //////////////////////////////////////////////////
Test(longopt_suite, pathname_longopt) {
    char *name = "pathnames_longopt";
    sprintf(program_options, "--pathnames -o %s/%s_out", TEST_OUTPUT_DIR, name);
    int err = run_using_system(name, "", "", STANDARD_LIMITS);
    assert_expected_status(EXIT_SUCCESS, err);
    assert_outfile_matches(name, NULL);
}

/* @brief Testing Long Option --output-file
*
*/
Test(longopt_suite, output_file_longopt) {
    char *name = "output_file_longopt";
    sprintf(program_options, "-p --output-file %s/%s_out", TEST_OUTPUT_DIR, name);
    int err = run_using_system(name, "", "", STANDARD_LIMITS);
    assert_expected_status(EXIT_SUCCESS, err);
    assert_outfile_matches(name, NULL);
}

/* @brief Testing Long Option --output-file without argument (Negative Test)
*/
Test(longopt_suite, output_file_longopt_no_arg) {
    char *name = "output_file_longopt_no_arg";
    sprintf(program_options, "-p --output-file");
    int err = run_using_system(name, "", "", STANDARD_LIMITS);
    assert_expected_status(EXIT_FAILURE, err);
}

/* @brief Testing Long Option --reject-file (Required Argument Negative Test)
   @details Program should exit with EXIT_FAILURE if --reject-file is used without a filename
*/
Test(longopt_suite, reject_file_longopt_no_arg) {
    char *name = "reject_file_longopt_no_arg";
    sprintf(program_options, "-p -o %s/%s_out --reject-file", TEST_OUTPUT_DIR, name);
    int err = run_using_system(name, "", "", STANDARD_LIMITS);
    assert_expected_status(EXIT_FAILURE, err);
}

/* @brief Testing Long Option --reject-file (Required Argument Positive Test)
   @details Program should exit with EXIT_SUCCESS if --reject-file is used with a filename
*/
Test(longopt_suite, reject_file_longopt_with_arg) {
    char *name = "reject_file_longopt_with_arg";
    sprintf(program_options, "-p -o %s/%s_out %s/%s_in %s/%s_diff --reject-file %s/%s.rej",
    TEST_OUTPUT_DIR, name, TEST_REF_DIR, name, TEST_REF_DIR, name, TEST_OUTPUT_DIR, name);
    int err = run_using_system(name, "", "", STANDARD_LIMITS);
    assert_expected_status(EXIT_SUCCESS, err);
    assert_rejfile_matches(name, NULL);
}

/*
* @brief Tests "+" in an option arg.
* @details Program should set output_file to "+", and then continue processing --silent, setting verbose to 0
*/
Test(longopt_suite, optarg_plus) {
    char *av[] = { "patch", "--output-file", "+", "--silent", NULL };
    Argv = av;
    Argc = sizeof(av)/sizeof(av[0]) - 1;
    verbose = 1;  // This should be set back to 0 by the program.
    get_some_switches();
    cr_assert_eq(strcmp(outname, "+"), 0,
        "getopt_long did not set outname properly (outname = \"%s\", expected: \"+\"", outname);
    cr_assert_eq(verbose, 0,
        "getopt_long did not continue option processing (verbose = TRUE, expected FALSE)");
}

/*
* @brief Tests "+garbage" instead of "+"
* @details The program should treat the arg as a filename, incrementing filec and setting filearg[0] to "+garbage".
*  It should NOT treat it as the special argument "+", which pauses option processing.
*/
#define GARBAGE "+garbage"
Test(longopt_suite, garbage_after_plus) {
    char *filec_default = "";
    char *av[] = { "patch", GARBAGE, "--silent", NULL };
    Argv = av;
    Argc = sizeof(av)/sizeof(av[0]) - 1;
    verbose = 1;  // This should be set back to 0 by the program.
    filec = 0;  // This should be set to 1 by the program.
    filearg[0] = filec_default;  // This should be updated to GARBAGE by the program
    get_some_switches();
    cr_assert_eq(filec, 1,
        "getopt_long did not increment filec.");
    cr_assert_eq(strcmp(filearg[0], GARBAGE), 0,
        "getopt_long did not treat garbage flag as a filearg (did not set filearg[0]).");
    cr_assert_eq(verbose, 0,
        "getopt_long did not continue option processing (verbose = TRUE, expected FALSE)");
}
#undef GARBAGE

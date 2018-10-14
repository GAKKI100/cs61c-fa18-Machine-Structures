/* Normally we want to import .h files and not .c files but for the purpose
 * of testing functions that are local in scope (only scoped within the file
 * and should not be globally accessable) we need to combine the files
 * directly. As an alternative we could use a separate .h file but that
 * could lead to inconsistency and bugs. */
#include "../../calc_depth.c"
#include "../../make_qtree.c"
#include "../../utils.c"
#include <CUnit/Basic.h>

/* These are some example unit tests to help test and modularize your code.
 * They are by no means complete and are merely meant as an example of how
 * to use CUnit. You are free to completely ignore this files and the
 * functions they test because this WILL NOT be graded, but you may find this
 * useful when devloping your code. */

void test_square_euclidean_distance () {
    /* Some sample test which are likely not complete. */
    CU_ASSERT_EQUAL (square_euclidean_distance (0, 0), 0);
    CU_ASSERT_EQUAL (square_euclidean_distance (1, 1), 0);
    CU_ASSERT_EQUAL (square_euclidean_distance (1, 0), 1);
    CU_ASSERT_EQUAL (square_euclidean_distance (0, 1), 1);
    CU_ASSERT_EQUAL (square_euclidean_distance (2, 0), 4);
    CU_ASSERT_EQUAL (square_euclidean_distance (0, 2), 4);

}

int main () {
    CU_pSuite pSuite = NULL;
    if (CUE_SUCCESS != CU_initialize_registry ()) {
        return CU_get_error ();
    }
    pSuite = CU_add_suite ("Testing calc_depth helper functions", NULL, NULL);
    if (!pSuite) {
        goto exit;
    }
    if (!CU_add_test (pSuite, "square euclidean distance test", test_square_euclidean_distance)) {
        goto exit;
    }
    CU_basic_set_mode (CU_BRM_VERBOSE);
    CU_basic_run_tests ();
exit:
    CU_cleanup_registry ();
    return CU_get_error ();
}

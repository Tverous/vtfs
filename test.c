#include <kunit/test.h>

#include "vtfs.h"

// static void fat_checksum_test(struct kunit *test)
// {
// 	/* With no extension. */
// 	KUNIT_EXPECT_EQ(test, fat_checksum("VMLINUX    "), (u8)44);
// 	/* With 3-letter extension. */
// 	KUNIT_EXPECT_EQ(test, fat_checksum("README  TXT"), (u8)115);
// 	/* With short (1-letter) extension. */
// 	KUNIT_EXPECT_EQ(test, fat_checksum("ABCDEFGHA  "), (u8)98);
// }

// static struct kunit_case fat_test_cases[] = {
// 	KUNIT_CASE(fat_checksum_test),
// 	KUNIT_CASE_PARAM(fat_time_fat2unix_test, fat_time_gen_params),
// 	KUNIT_CASE_PARAM(fat_time_unix2fat_test, fat_time_gen_params),
// 	{},
// };

// static struct kunit_suite fat_test_suite = {
// 	.name = "fat_test",
// 	.test_cases = fat_test_cases,
// };

// kunit_test_suites(&fat_test_suite);

// MODULE_LICENSE("GPL v2");
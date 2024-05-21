#include <gtest/gtest.h>

#include "c.h"

static void TestCreateSuccess(field_item_t* field_list, size_t size) {
    char* errptr = NULL;
    us_fieldset_t* fieldset = NULL;
    fieldset = us_fieldset_create(field_list, size, &errptr);
    ASSERT_TRUE(fieldset != NULL);
    ASSERT_TRUE(errptr == NULL);
    ASSERT_EQ(us_fieldset_get_field_count(fieldset), size);
    us_fieldset_destroy(fieldset);
}

static void TestCreateFail(field_item_t* field_list, size_t size) {
    char* errptr = NULL;
    us_fieldset_t* fieldset = NULL;
    fieldset = us_fieldset_create(field_list, size, &errptr);
    ASSERT_TRUE(fieldset == NULL);
    ASSERT_TRUE(errptr != NULL);
    free(errptr);
}
TEST(FieldSetTest, Create1) {
    field_item_t field_list[] = {{0, "NAME0", OPT_MATCH}};
    const size_t size = sizeof(field_list) / sizeof(field_list[0]);
    TestCreateSuccess(field_list, size);
}

TEST(FieldSetTest, Create2) {
    field_item_t field_list[] = {{0, "NAME0", OPT_MATCH},
                                 {1, "NAME1", OPT_INCLUDE | OPT_MATCH}};
    const size_t size = sizeof(field_list) / sizeof(field_list[0]);
    TestCreateSuccess(field_list, size);
}

TEST(FieldSetTest, CreateMoreThan2) {
    field_item_t field_list[] = {
        {0, "NAME0", OPT_MATCH},       {1, "NAME1", OPT_INCLUDE},
        {2, "NAME2", OPT_EQ},          {3, "NAME3", OPT_LEN_EQ},
        {4, "NAME4", OPT_LEN_GT},      {5, "NAME5", OPT_LEN_LT},
        {6, "NAME6", OPT_NOT_INCLUDE}, {7, "NAME7", OPT_IP_IN_LIST},
        {8, "NAME8", OPT_IP_BELONG},   {9, "NAME9", OPT_LEN_NOT_EQ},
    };
    const size_t size = sizeof(field_list) / sizeof(field_list[0]);

    TestCreateSuccess(field_list, size);
}

TEST(FieldSetTest, CreateAllowIndexOutOfOrder) {
    field_item_t field_list[] = {{1, "NAME1", OPT_MATCH},
                                 {0, "NAME0", OPT_INCLUDE},
                                 {2, "NAME2", OPT_IP_IN_LIST}};
    const size_t size = sizeof(field_list) / sizeof(field_list[0]);
    TestCreateSuccess(field_list, size);
}

TEST(FieldSetTest, CreateInvalid) {
    field_item_t field_list[] = {{0, "NAME0", OPT_MATCH}};

    TestCreateFail(NULL, 0);
    TestCreateFail(NULL, 1);
    TestCreateFail(field_list, 0);
}

TEST(FieldSetTest, CreateNotAllowSameName) {
    field_item_t field_list[] = {{1, "SAME_NAME", OPT_MATCH},
                                 {0, "SAME_NAME", OPT_INCLUDE},
                                 {2, "NAME2", OPT_IP_IN_LIST}};
    const size_t size = sizeof(field_list) / sizeof(field_list[0]);

    TestCreateFail(field_list, size);
}

TEST(FieldSetTest, CreateNotAllowSameIndex) {
    field_item_t field_list[] = {{0, "SAME_INDEX", OPT_MATCH},
                                 {0, "SAME_INDEX", OPT_INCLUDE},
                                 {1, "NAME1", OPT_IP_IN_LIST}};
    const size_t size = sizeof(field_list) / sizeof(field_list[0]);

    TestCreateFail(field_list, size);
}

TEST(FieldSetTest, CreateIndexNotOneByOne) {
    field_item_t field_list[] = {{0, "SAME_INDEX", OPT_MATCH},
                                 {1, "SAME_INDEX", OPT_INCLUDE},
                                 {3, "SHOULD_BE_2", OPT_IP_IN_LIST}};
    const size_t size = sizeof(field_list) / sizeof(field_list[0]);

    TestCreateFail(field_list, size);
}

TEST(FieldSetTest, CreateNotAllowOptMaskNone) {
    field_item_t field_list[] = {{0, "NAME0", OPT_NONE}};
    const size_t size = sizeof(field_list) / sizeof(field_list[0]);

    TestCreateFail(field_list, size);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

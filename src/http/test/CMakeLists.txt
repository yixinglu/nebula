# Copyright (c) 2020 vesoft inc. All rights reserved.
#
# This source code is licensed under Apache 2.0 License,
# attached with Common Clause Condition 1.0, found in the LICENSES directory.
#

nebula_add_test(
    NAME
        graph_http_test
    SOURCES
        GraphHttpHandlerTest.cpp
    OBJECTS
        $<TARGET_OBJECTS:graph_test_common_obj>
        $<TARGET_OBJECTS:graph_http_handler>
        $<TARGET_OBJECTS:ws_obj>
        $<TARGET_OBJECTS:client_cpp_obj>
        $<TARGET_OBJECTS:stats_obj>
        $<TARGET_OBJECTS:process_obj>
        $<TARGET_OBJECTS:adHocSchema_obj>
        ${GRAPH_TEST_LIBS}
    LIBRARIES
        proxygenhttpserver
        proxygenlib
        ${THRIFT_LIBRARIES}
        ${ROCKSDB_LIBRARIES}
        wangle
        gtest
)

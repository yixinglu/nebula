nebula_add_executable(
    NAME
        nebula-graphd
    SOURCES
        GraphDaemon.cpp
    OBJECTS
        # $<TARGET_OBJECTS:filter_obj>
        # $<TARGET_OBJECTS:graph_http_handler>
        # $<TARGET_OBJECTS:graph_obj>
        $<TARGET_OBJECTS:http_client_obj>
        # $<TARGET_OBJECTS:parser_obj>
        $<TARGET_OBJECTS:network_obj>
        $<TARGET_OBJECTS:process_obj>
        # $<TARGET_OBJECTS:graph_thrift_obj>
        $<TARGET_OBJECTS:graph_storage_client_obj>
        # $<TARGET_OBJECTS:storage_thrift_obj>
        $<TARGET_OBJECTS:meta_client_obj>
        $<TARGET_OBJECTS:stats_obj>
        $<TARGET_OBJECTS:time_obj>
        # $<TARGET_OBJECTS:meta_thrift_obj>
        # $<TARGET_OBJECTS:common_thrift_obj>
        $<TARGET_OBJECTS:thrift_obj>
        # $<TARGET_OBJECTS:schema_obj>
        $<TARGET_OBJECTS:ws_obj>
        $<TARGET_OBJECTS:ws_common_obj>
        # $<TARGET_OBJECTS:dataman_obj>
        # $<TARGET_OBJECTS:meta_gflags_man_obj>
        # $<TARGET_OBJECTS:gflags_man_obj>
        $<TARGET_OBJECTS:thread_obj>
        $<TARGET_OBJECTS:fs_obj>
        $<TARGET_OBJECTS:base_obj>
        $<TARGET_OBJECTS:concurrent_obj>
    LIBRARIES
        proxygenhttpserver
        proxygenlib
        ${THRIFT_LIBRARIES}
        wangle
)

install(
    TARGETS
        nebula-graphd
    DESTINATION
        bin
)

cmake_minimum_required(VERSION 3.5)

set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(led_strip)

idf_build_get_property(idf_path IDF_PATH)
idf_build_get_property(build_dir BUILD_DIR)
idf_build_get_property(project_dir PROJECT_DIR)

partition_table_get_partition_info(storage_size "--partition-type data --partition-subtype spiffs" "size")

set(MKSPIFFS_PATH ${idf_path}/tools/mkspiffs/mkspiffs)

if (NOT EXISTS ${MKSPIFFS_PATH})
    message(FATAL_ERROR "Need compile mkspiffs. Run install.sh in ESP RTOS SDK.")
endif()

add_custom_command(OUTPUT "${project_dir}/data/storage.lock"
    COMMAND touch ${project_dir}/data/storage.lock
    COMMAND rm -f ${build_dir}/storage.bin
    VERBATIM
    WORKING_DIRECTORY ${build_dir}
    COMMENT "Generating spiffs lock file in directory /data for check data update")

add_custom_command(OUTPUT "${build_dir}/storage.bin"
    COMMAND ${MKSPIFFS_PATH} --create ${project_dir}/data --page 256 --size ${storage_size} ${build_dir}/storage.bin
    VERBATIM
    DEPENDS "${project_dir}/data/storage.lock"
    WORKING_DIRECTORY ${build_dir}
    COMMENT "Generating spiffs binary image from directory /data")


add_custom_target(storage DEPENDS "${build_dir}/storage.bin")

esptool_py_flash_project_args(storage 0x110000 "${build_dir}/storage.bin" FLASH_IN_PROJECT)

esptool_py_custom_target(storage-flash storage "storage")

add_dependencies(flash storage)
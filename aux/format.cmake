################################################################################
# Format targets: 
# - make fmt: formats the source code
# - make fmt-check: checks if the source code is correctly formatted
################################################################################

# Clang-format: make fmt
add_custom_command(OUTPUT format COMMAND
  ${PROJECT_SOURCE_DIR}/aux/format.sh ${PROJECT_SOURCE_DIR})
add_custom_target(fmt DEPENDS format WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

# Clang-format: make fmt-check
add_custom_command(OUTPUT format-check COMMAND
  ${PROJECT_SOURCE_DIR}/aux/format_check.sh ${PROJECT_SOURCE_DIR})
add_custom_target(fmt-check DEPENDS format-check WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

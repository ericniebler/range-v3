# SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES.
# All rights reserved. SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.

# Passes all args directly to execute_process while setting up the following
# results variables and propagating them to the caller's scope:
#
# * rv3_process_exit_code
# * rv3_process_stdout
# * rv3_process_stderr
#
# If the command is not successful (e.g. the last command does not return zero),
# a non-fatal warning is printed.
function(rv3_execute_non_fatal_process)
  execute_process(
    ${ARGN}
    RESULT_VARIABLE rv3_process_exit_code
    OUTPUT_VARIABLE rv3_process_stdout
    ERROR_VARIABLE rv3_process_stderr)

  if(NOT rv3_process_exit_code EQUAL 0)
    message(
      WARNING
        "execute_process failed with non-zero exit code: ${rv3_process_exit_code}\n"
        "${ARGN}\n"
        "stdout:\n${rv3_process_stdout}\n"
        "stderr:\n${rv3_process_stderr}\n")
  endif()

  set(rv3_process_exit_code "${rv3_process_exit_code}" PARENT_SCOPE)
  set(rv3_process_stdout "${rv3_process_stdout}" PARENT_SCOPE)
  set(rv3_process_stderr "${rv3_process_stderr}" PARENT_SCOPE)
endfunction()

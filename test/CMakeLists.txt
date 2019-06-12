# Test 1
add_executable (ArchiProfilerTest ArchiProfilerTest.cpp)
target_link_libraries(ArchiProfilerTest S3LIB fsm)

# Test 2
add_executable (DFAProfilerTest DFAProfilerTest.cpp)
target_link_libraries(DFAProfilerTest S3LIB fsm)

# Test 3
add_executable (VerifierTest VerifierTest.cpp)
target_link_libraries(VerifierTest S3LIB fsm)

# Test 4
add_executable (ControllerAndVerifierTest ControllerAndVerifierTest.cpp)
target_link_libraries(ControllerAndVerifierTest S3LIB fsm)

# Test 5
add_executable (VerifierRun VerifierRun.cpp)
target_link_libraries(VerifierRun S3LIB fsm)


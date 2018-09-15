# Test 1
add_executable (ArchiProfilerTest ArchiProfilerTest.cpp)
target_link_libraries(ArchiProfilerTest S3LIB MicroSpec)

# Test 2
add_executable (DFAProfilerTest DFAProfilerTest.cpp)
target_link_libraries(DFAProfilerTest S3LIB MicroSpec)

# Test 3
add_executable (VerifierTest VerifierTest.cpp)
target_link_libraries(VerifierTest S3LIB MicroSpec)

# Test 4
add_executable (ControllerAndVerifierTest ControllerAndVerifierTest.cpp)
target_link_libraries(ControllerAndVerifierTest S3LIB MicroSpec)

# Test 1
add_executable (ArchiProfilerTest ArchiProfilerTest.cpp)
target_link_libraries(ArchiProfilerTest S3LIB MicroSpec)

# Test 2
add_executable (DFAProfilerTest DFAProfilerTest.cpp)
target_link_libraries(DFAProfilerTest S3LIB MicroSpec)


# Test 3
add_executable (VerifierTest1 VerifierTest1.cpp)
target_link_libraries(VerifierTest1 S3LIB MicroSpec)

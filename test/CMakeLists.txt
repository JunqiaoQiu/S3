# Test 1
add_executable (ArchiTest ArchiTest.cpp)
target_link_libraries(ArchiTest S3LIB MicroSpec)

# Test 2
add_executable (VerifierTest1 VerifierTest1.cpp)
target_link_libraries(VerifierTest1 S3LIB MicroSpec)

# partitioned_elias_fano/Runner

set(CPPFILES
    QueryLogRunnerBase.cpp
    Stopwatch.cpp
    ThreadSynchronizer.cpp
    QueryLogRunner.cpp)

set(WINDOWS_CPPFILES
)

set(POSIX_CPPFILES
)

set(PRIVATE_HFILES
    QueryLogRunnerBase.h
    Stopwatch.h
    ThreadSynchronizer.h
    QueryLogRunner.h)

set(WINDOWS_PRIVATE_HFILES
)

set(POSIX_PRIVATE_HFILES
)

add_executable(Runner ${CPPFILES} ${PRIVATE_HFILES} ${PUBLIC_HFILES})
target_link_libraries(Runner
    ${Boost_LIBRARIES}
    FastPFor_lib
    block_codecs
    -lpthread
)
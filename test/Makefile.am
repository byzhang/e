check_PROGRAMS = testrunner
TESTS = $(check_PROGRAMS)

testrunner_SOURCES = runner.cc bitfield.cc bit_stealing.cc buffer.cc \
					 convert.cc guard.cc intrusive_ptr.cc \
					 locking_iterable_fifo.cc profiler.cc
testrunner_CPPFLAGS = $(CPPFLAGS) -I$(abs_top_srcdir)/include
testrunner_LDFLAGS = -lpthread -lgtest -lrt -no-install
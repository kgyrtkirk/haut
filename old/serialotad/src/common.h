#pragma once

#include <poll.h>
#include <sys/select.h>
#include <stdlib.h>
#include <stdio.h>
#include <pty.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <errno.h>

#include <boost/program_options.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>

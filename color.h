#ifndef _COLOR_H
#define _COLOR_H

#include <string>

namespace spu {

std::string RESET       = "\033[0m";
std::string BLACK       = "\033[30m";      /* Black */
std::string RED         = "\033[31m";      /* Red */
std::string GREEN       = "\033[32m";      /* Green */
std::string YELLOW      = "\033[33m";      /* Yellow */
std::string BLUE        = "\033[34m";      /* Blue */
std::string MAGENTA     = "\033[35m";      /* Magenta */
std::string CYAN        = "\033[36m";      /* Cyan */
std::string WHITE       = "\033[37m";      /* White */
std::string BOLDBLACK   = "\033[1m\033[30m";      /* Bold Black */
std::string BOLDRED     = "\033[1m\033[31m";      /* Bold Red */
std::string BOLDGREEN   = "\033[1m\033[32m";      /* Bold Green */
std::string BOLDYELLOW  = "\033[1m\033[33m";      /* Bold Yellow */
std::string BOLDBLUE    = "\033[1m\033[34m";      /* Bold Blue */
std::string BOLDMAGENTA = "\033[1m\033[35m";      /* Bold Magenta */
std::string BOLDCYAN    = "\033[1m\033[36m";      /* Bold Cyan */
std::string BOLDWHITE   = "\033[1m\033[37m";      /* Bold White */

} // namespace spu

#endif
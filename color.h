/*
 * =====================================================================================
 *
 *       Filename:  color.h
 *
 *    Description:  Some #defines for bash color output
 *
 *        Version:  1.0
 *        Created:  07/05/2015 05:21:29 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Andrew Epstein
 *   Organization:  SevOne
 *
 * =====================================================================================
 */

#ifdef COLOR

#define BLUE "\033[34m"
#define GREEN "\033[32m"
#define PURPLE "\033[35m"
#define RED "\033[31m"
#define WHITE "\033[97m"
#define YELLOW "\033[33m"
#define NONE "\033[0m"

#else

#define BLUE ""
#define GREEN ""
#define PURPLE ""
#define RED ""
#define WHITE ""
#define YELLOW ""
#define NONE ""

#endif // COLOR


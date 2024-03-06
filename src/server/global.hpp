/**
 *
 * @file          declaration.hpp
 *
 * @copyright     MIT License
 *
 *                Copyright (c) 2023 Sorin Tudose
 *
 *                Permission is hereby granted, free of charge, to any person obtaining a copy
 *                of this software and associated documentation files (the "Software"), to deal
 *                in the Software without restriction, including without limitation the rights
 *                to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *                copies of the Software, and to permit persons to whom the Software is
 *                furnished to do so, subject to the following conditions:
 *
 *                The above copyright notice and this permission notice shall be included in all
 *                copies or substantial portions of the Software.
 *
 *                THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *                IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *                FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *                AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *                LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *                OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *                SOFTWARE.
 *
 *  @brief        This C++ header file includes the protoypes for helper function as well as extern variables.
 *
 */

#ifndef __GLOBAL_HPP__
#define __GLOBAL_HPP__

#include "serverUtils.hpp"

// Extern pointer to the instance of the net::server class. This allows all files that include this header file to have access to this variable.
extern net::server<char> *__server;

extern bool DEBUG_FLAG;

/**
 * @brief This function searches for a substring. 
 *
 * @param haystack The string where the substring is searched.
 * @param needle The substring that is searched for
 *
 * @return Returns true if the string is found, otherwise false.
 */
bool findString(const char haystack[], const char needle[]);

/**
 * @brief This function checks if a string is a number by checking if all characters are digits.
 * @return Returns true if the string contains only digits, otherwise false.
 */
bool isNumeric(const char *str);

/**
 * @brief This function is called at the begining of the main function. It checks if the arguments provided by the user are valid and what is the port value.
 * @return Returns 0 on success, -1 for errors.
 */
int getMainArguments(int argc, char *argv[]);

// This function is designed to generate an underline of a specified width.
void underline(const unsigned int vWidth);

// Function to turn off echoing in terminal
void toggleEcho(bool enable);

#endif //__DECLARATIONS_HPP__
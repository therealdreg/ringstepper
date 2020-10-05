/*
  ringstepper
  MIT LICENSE Copyright <2020>
  David Reguera Garcia aka Dreg
  Dreg@fr33project.org - http://www.fr33project.org/ - https://github.com/David-Reguera-Garcia-Dreg/
  -
  Permission is hereby granted, free of charge, to any person obtaining a copy of this softwareand
  associated documentation files(the "Software"), to deal in the Software without restriction,
  including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and /or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions :
  The above copyright noticeand this permission notice shall be included in all copies or substantial
  portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
  LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
  OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  -
  WARNING!!bullshit code
  -
*/

#pragma once

#define versw "0.1b"

#include "engextcpp.hpp"

class EXT_CLASS : public ExtExtension
{
public:
    EXT_CLASS();
    EXT_COMMAND_METHOD(printssdt);
    EXT_COMMAND_METHOD(z);
};


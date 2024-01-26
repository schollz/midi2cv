/*
MIT License

Copyright (c) 2020 andrewikenberry
Copyright (c) 2023 Jacob Vosmaer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "midi.h"

midi_message midi_read(midi_parser *p, uint8_t b) {
  enum { DATA0_PRESENT = 0x80 };
  midi_message msg = {0};

  if (b >= 0xf8) {
    msg.status = b;
  } else if (b >= 0xf4) {
    msg.status = b;
    p->status = 0;
  } else if (b >= 0x80) {
    p->status = b;
    p->previous = b == 0xf0 ? b : 0;
  } else if ((p->status >= 0xc0 && p->status < 0xe0) ||
             (p->status >= 0xf0 && p->status != 0xf2)) {
    msg.status = p->status;
    msg.data[0] = b;
    msg.data[1] = p->previous;
    p->previous = 0;
  } else if (p->status && p->previous & DATA0_PRESENT) {
    msg.status = p->status;
    msg.data[0] = p->previous ^ DATA0_PRESENT;
    msg.data[1] = b;
    p->previous = 0;
  } else {
    p->previous = b | DATA0_PRESENT;
  }

  return msg;
}
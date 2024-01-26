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

#ifndef MIDI_H
#define MIDI_H

#include <stdint.h>

enum {
  MIDI_NOTE_OFF = 0x80,
  MIDI_NOTE_ON = 0x90,
  MIDI_AFTERTOUCH = 0xa0,
  MIDI_CONTROL_CHANGE = 0xb0,
  MIDI_PROGRAM_CHANGE = 0xc0,
  MIDI_CHANNEL_PRESSURE = 0xd0,
  MIDI_PITCH_BEND = 0xe0,
  MIDI_SYSEX = 0xf0,
  MIDI_SYSEX_END = 0xf7
};

typedef struct midi_message {
  uint8_t status;
  uint8_t data[2];
} midi_message;

typedef struct midi_parser {
  uint8_t status;
  uint8_t previous;
} midi_parser;

midi_message midi_read(midi_parser *p, uint8_t b);

#endif
//============================================================================
// Super-Simple Tasker (SST) in C++
//
// Copyright (C) 2006-2023 Quantum Leaps, <state-machine.com>.
//
// SPDX-License-Identifier: MIT
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//============================================================================
#ifndef SST_HPP_
#define SST_HPP_

#include <cstdint>      // standard integer types
#include "sst_port.hpp" // SST port for specific CPU

namespace SST {

// SST Kernel facilities -----------------------------------------------------
void init(void);
int  run(void);
void onStart(void);
void onIdle(void);

// SST Event facilities ------------------------------------------------------
//! signal of SST event
using Signal = std::uint16_t;

struct Evt {
    Signal sig;
};

// template for downcasting SST events to specific Evt "subclasses"
template<typename EVT_>
EVT_ const *evt_downcast(Evt const *e) {
    return reinterpret_cast<EVT_ const *>(e);
}

// SST Task facilities -------------------------------------------------------

//! SST Task priority
using TaskPrio = std::uint8_t;

//! SST internal event-queue counter
using QCtr = std::uint8_t;

//! SST Task (a.k.a. "Active Object")
class Task {
private:
    Evt const **m_qBuf; //!< ring buffer of the queue
    QCtr m_end;   //!< last index in the ring buffer
    QCtr m_head;  //!< index for inserting events
    QCtr m_tail;  //!< index for removing events
    QCtr m_nUsed; //!< # used entries currently in the queue

#ifdef SST_PORT_TASK_ATTR
    SST_PORT_TASK_ATTR
#endif

public:
    void start(
        TaskPrio prio,
        Evt const **qBuf, QCtr qLen,
        Evt const * const ie);

    void post(Evt const * const e) noexcept;

    virtual void init(Evt const * const ie) = 0;
    virtual void dispatch(Evt const * const e) = 0;

#ifdef SST_PORT_TASK_OPER
    SST_PORT_TASK_OPER
#endif
};

} // namespace SST

#endif // SST_HPP_

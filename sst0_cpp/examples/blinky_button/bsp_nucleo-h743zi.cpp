//============================================================================
// Super-Simple Tasker (SST0/C++) Example for STM32 NUCLEO-H74cZI
//
//
//                    Q u a n t u m  L e a P s
//                    ------------------------
//                    Modern Embedded Software
//
// Copyright (C) 2005 Quantum Leaps, <state-machine.com>.
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
#include "sst.hpp"           // SST framework
#include "bsp.hpp"           // Board Support Package interface
#include "blinky_button.hpp" // application shared interface

#include "stm32h743xx.h"  // CMSIS-compliant header file for the MCU used
#include <cmath>          // to exercise the FPU
// add other drivers if necessary...

// Local-scope defines -------------------------------------------------------
namespace {

DBC_MODULE_NAME("bsp_nucleo-h743zi") // for DBC assertions in this module

} // unnamed workspace

// test pins on GPIO PB
#define TST1_PIN  0U  /* PB.0  LED1-Green */
#define TST2_PIN  14U /* PB.14 LED3-Red   */
#define TST3_PIN  4U
#define TST4_PIN  5U
#define TST5_PIN  6U
#define TST6_PIN  7U  /* PB.7  LED2-Blue  */

// buttons on GPIO PC
#define B1_PIN    13U

// ISRs used in the application ==============================================
extern "C" {

void SysTick_Handler(void) {   // system clock tick ISR
    BSP::d1on();

    SST::TimeEvt::tick();

    // get state of the user button
    // Perform the debouncing of buttons. The algorithm for debouncing
    // adapted from the book "Embedded Systems Dictionary" by Jack Ganssle
    // and Michael Barr, page 71.
    //
    static struct ButtonsDebouncing {
        uint32_t depressed;
        uint32_t previous;
    } buttons = { 0U, 0U };
    uint32_t current = GPIOC->IDR; // read GPIO PortC
    uint32_t tmp = buttons.depressed; // save the debounced depressed
    buttons.depressed |= (buttons.previous & current); // set depressed
    buttons.depressed &= (buttons.previous | current); // clear released
    buttons.previous   = current; // update the history
    tmp ^= buttons.depressed;     // changed debounced depressed
    if ((tmp & (1U << B1_PIN)) != 0U) { // debounced B1 state changed?
        if ((buttons.depressed & (1U << B1_PIN)) != 0U) { // depressed?
            // immutable button-press event
            static App::ButtonWorkEvt const pressEvt = {
                { App::BUTTON_PRESSED_SIG }, 60U
            };
            // immutable forward-press event
            static App::ButtonWorkEvt const fPressEvt = {
                { App::FORWARD_PRESSED_SIG }, 60U
            };
            App::AO_Button2a->post(&fPressEvt.super);
            App::AO_Button2a->post(&pressEvt.super);
        }
        else { // B1 is released
            // immutable button-release event
            static App::ButtonWorkEvt const releaseEvt = {
                { App::BUTTON_RELEASED_SIG }, 80U
            };
            // immutable forward-release event
            static App::ButtonWorkEvt const fReleaseEvt = {
                { App::FORWARD_RELEASED_SIG }, 80U
            };
            App::AO_Button2a->post(&fReleaseEvt.super);
            App::AO_Button2a->post(&releaseEvt.super);
        }
    }

    BSP::d1off();
}

// Assertion handler =========================================================
void DBC_fault_handler(char const * const module, int const label) {
    //
    // NOTE: add here your application-specific error handling
    //
    (void)module;
    (void)label;

    // set PRIMASK to disable interrupts and stop SST right here
    __asm volatile ("cpsid i");

#ifndef NDEBUG
    for (;;) { // keep blinking LED2
        BSP::d6on();  // turn LED2 on
        uint32_t volatile ctr;
        for (ctr = 1000000U; ctr > 0U; --ctr) {
        }
        BSP::d6off(); // turn LED2 off
        for (ctr = 1000000U; ctr > 0U; --ctr) {
        }
    }
#endif
    NVIC_SystemReset();
}
//............................................................................
void assert_failed(char const * const module, int const label);// prototype
void assert_failed(char const * const module, int const label) {
    DBC_fault_handler(module, label);
}

} // extern "C"

namespace BSP {

// BSP functions =============================================================
void init(void) {
    // Configure the MPU to prevent NULL-pointer dereferencing
    // see: www.state-machine.com/null-pointer-protection-with-arm-cortex-m-mpu
    //
    MPU->RBAR = 0x0U                          // base address (NULL)
                | MPU_RBAR_VALID_Msk          // valid region
                | (MPU_RBAR_REGION_Msk & 7U); // region #7
    MPU->RASR = (7U << MPU_RASR_SIZE_Pos)     // 2^(7+1) region
                | (0x0U << MPU_RASR_AP_Pos)   // no-access region
                | MPU_RASR_ENABLE_Msk;        // region enable

    MPU->CTRL = MPU_CTRL_PRIVDEFENA_Msk       // enable background region
                | MPU_CTRL_ENABLE_Msk;        // enable the MPU
    __ISB();
    __DSB();

    SCB_EnableICache(); // Enable I-Cache
    SCB_EnableDCache(); // Enable D-Cache

    // enable GPIOB port clock for LEds and test pins
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;

    // set all used GPIOB pins as push-pull output, no pull-up, pull-down
    GPIOB->MODER &=
        ~((3U << 2U*TST1_PIN) | (3U << 2U*TST2_PIN) | (3U << 2U*TST3_PIN) |
          (3U << 2U*TST4_PIN) | (3U << 2U*TST5_PIN) | (3U << 2U*TST6_PIN));
    GPIOB->MODER |=
         ((1U << 2U*TST1_PIN) | (1U << 2U*TST2_PIN) | (1U << 2U*TST3_PIN) |
          (1U << 2U*TST4_PIN) | (1U << 2U*TST5_PIN) | (1U << 2U*TST6_PIN));
    GPIOB->OTYPER &=
        ~((1U <<    TST1_PIN) | (1U <<    TST2_PIN) | (1U <<    TST3_PIN) |
          (1U <<    TST4_PIN) | (1U <<    TST5_PIN) | (1U <<    TST6_PIN));
    GPIOB->PUPDR &=
        ~((3U << 2U*TST1_PIN) | (3U << 2U*TST2_PIN) | (3U << 2U*TST3_PIN) |
          (3U << 2U*TST4_PIN) | (3U << 2U*TST5_PIN) | (3U << 2U*TST6_PIN));

    // enable GPIOC clock port for the Button B1
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;

    // configure Button B1 pin on GPIOC as input, no pull-up, pull-down
    GPIOC->MODER &= ~(3U << 2U*B1_PIN);
    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD0 << 2U*B1_PIN);
    GPIOC->PUPDR |=  (2U << 2U*B1_PIN);
}

//............................................................................
static void exerciseFPU(double x) {
    // exercise the double-precision FPU by calculating the identity:
    //  sin(x)^2 + cos(x)^2 == 1.0 for any x
    //
    double tmp = pow(sin(x), 2.0) + pow(cos(x), 2.0);
    DBC_ENSURE(200, ((1.0 - 1e-4) < tmp) && (tmp < (1.0 + 1e-4)));
}

//............................................................................
void d1on(void) {  // LED1-Green
    GPIOB->BSRR = (1U << TST1_PIN);
    // don't use the FPU in the ISR
}
void d1off(void) {
    GPIOB->BSRR = (1U << (TST1_PIN + 16U));
}
//............................................................................
void d2on(void) {  // LED3-Red
    GPIOB->BSRR = (1U << TST2_PIN);
    exerciseFPU(-1.2345);
}
void d2off(void) {
    GPIOB->BSRR = (1U << (TST2_PIN + 16U));
}
//............................................................................
void d3on(void) {
    GPIOB->BSRR = (1U << TST3_PIN);
    exerciseFPU(-12.345);
}
void d3off(void) {
    GPIOB->BSRR = (1U << (TST3_PIN + 16U));
}
//............................................................................
void d4on(void) {
    GPIOB->BSRR = (1U << TST4_PIN);
    exerciseFPU(3.456);
}
void d4off(void) {
    GPIOB->BSRR = (1U << (TST4_PIN + 16U));
}
//............................................................................
void d5on(void) {
    GPIOB->BSRR = (1U << TST5_PIN);
    exerciseFPU(4.567);
}
void d5off(void) {
    GPIOB->BSRR = (1U << (TST5_PIN + 16U));
}
//............................................................................
void d6on(void) {  // LED2-Blue
    GPIOB->BSRR = (1U << TST6_PIN);
    exerciseFPU(1.2345);
}
void d6off(void) {
    GPIOB->BSRR = (1U << (TST6_PIN + 16U));
}

//............................................................................
SST::Evt const *getWorkEvtBlinky1(uint8_t num) {
    // immutable work events for Blinky1
    static App::BlinkyWorkEvt const workBlinky1[] = {
        { { App::BLINKY_WORK_SIG }, 40U, 5U },
        { { App::BLINKY_WORK_SIG }, 30U, 7U }
    };
    DBC_REQUIRE(500, num < ARRAY_NELEM(workBlinky1)); // num must be in range
    return &workBlinky1[num].super;
}
//............................................................................
SST::Evt const *getWorkEvtBlinky3(uint8_t num) {
    // immutable work events for Blinky3
    static App::BlinkyWorkEvt const workBlinky3[] = {
        { { App::BLINKY_WORK_SIG }, 20U, 5U },
        { { App::BLINKY_WORK_SIG }, 10U, 3U   }
    };
    DBC_REQUIRE(600, num < ARRAY_NELEM(workBlinky3)); // num must be in range
    return &workBlinky3[num].super;
}

} // namespace BSP

// SST callbacks =============================================================
namespace SST {

void onStart(void) {
    SystemCoreClockUpdate();

    // set up the SysTick timer to fire at BSP::TICKS_PER_SEC rate
    SysTick_Config((SystemCoreClock / BSP::TICKS_PER_SEC) + 1U);

    // set priorities of ISRs used in the system
    NVIC_SetPriority(SysTick_IRQn, 0U);
    // ...
}
//............................................................................
void onIdleCond(void) { // NOTE: called with interrupts DISABLED
    BSP::d6on();  // turn LED2 on
#ifdef NDEBUG
    // Put the CPU and peripherals to the low-power mode.
    // you might need to customize the clock management for your application,
    // see the datasheet for your particular Cortex-M MCU.
    //
    BSP::d6off(); // turn LED2 off
    __WFI(); // Wait-For-Interrupt
    BSP::d6on();  // turn LED2 on
#endif
    BSP::d6off(); // turn LED2 off
    SST_PORT_INT_ENABLE(); // NOTE: enable interrupts for SS0
}

} // namespace SST


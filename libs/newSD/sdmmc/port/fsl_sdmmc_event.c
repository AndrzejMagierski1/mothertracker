/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include "fsl_sdmmc_event.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Get event instance.
 * @param eventType The event type
 * @return The event instance's pointer.
 */
static volatile uint32_t *SDMMCEVENT_GetInstance(sdmmc_event_t eventType);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief Card detect event. */
static volatile uint32_t g_eventCardDetect;

/*! @brief transfer complete event. */
static volatile uint32_t g_eventTransferComplete;

/*! @brief Time variable unites as milliseconds. */

extern volatile uint32_t systick_millis_count;
/*******************************************************************************
 * Code
 ******************************************************************************/

static volatile uint32_t *SDMMCEVENT_GetInstance(sdmmc_event_t eventType)
{
    volatile uint32_t *event;

    switch (eventType)
    {
        case kSDMMCEVENT_TransferComplete:
            event = &g_eventTransferComplete;
            break;
        case kSDMMCEVENT_CardDetect:
            event = &g_eventCardDetect;
            break;
        default:
            event = NULL;
            break;
    }

    return event;
}

bool SDMMCEVENT_Create(sdmmc_event_t eventType)
{
    volatile uint32_t *event = SDMMCEVENT_GetInstance(eventType);

    if (event)
    {
        *event = 0;
        return true;
    }
    else
    {
        return false;
    }
}

bool SDMMCEVENT_Wait(sdmmc_event_t eventType, uint32_t timeoutMilliseconds)
{
    uint32_t startTime;
    uint32_t elapsedTime;

    volatile uint32_t *event = SDMMCEVENT_GetInstance(eventType);

    if (timeoutMilliseconds && event)
    {
        startTime = systick_millis_count;
        do
        {
            elapsedTime = (systick_millis_count - startTime);
        } while ((*event == 0U) && (elapsedTime < timeoutMilliseconds));
        *event = 0U;

        return ((elapsedTime < timeoutMilliseconds) ? true : false);
    }
    else
    {
        return false;
    }
}

bool SDMMCEVENT_Notify(sdmmc_event_t eventType)
{
    volatile uint32_t *event = SDMMCEVENT_GetInstance(eventType);

    if (event)
    {
        *event = 1U;
        return true;
    }
    else
    {
        return false;
    }
}

void SDMMCEVENT_Delete(sdmmc_event_t eventType)
{
    volatile uint32_t *event = SDMMCEVENT_GetInstance(eventType);

    if (event)
    {
        *event = 0U;
    }
}

void SDMMCEVENT_Delay(uint32_t milliseconds)
{
    uint32_t startTime = systick_millis_count;
    uint32_t periodTime = 0;
    while (periodTime < milliseconds)
    {
        periodTime = systick_millis_count - startTime;
    }
}

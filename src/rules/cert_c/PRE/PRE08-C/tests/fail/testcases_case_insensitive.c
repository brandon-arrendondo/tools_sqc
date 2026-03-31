/*
 * Rule: PRE08-C
 * Source: testcases
 * Status: FAIL - Should trigger PRE08-C violation
 * Description: Headers differing only in case within first 8 chars
 */

#include "Config.h"
#include "config.h"
#include "Handler.h"
#include "handler.h"

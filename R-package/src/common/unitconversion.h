/*
 * Copyright (c) 2010, Andras Varga and Opensim Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Opensim Ltd. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Andras Varga or Opensim Ltd. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _UNITCONVERSION_H_
#define _UNITCONVERSION_H_

#include <vector>
#include "commondefs.h"
#include "exception.h"

NAMESPACE_BEGIN

/**
 * Unit conversion utilities. This class has built-in knowledge of some
 * physical units (seconds, watts, etc); see internal unitTable[].
 */
class COMMON_API UnitConversion
{
  protected:
    struct UnitDesc { const char *unit; double mult; const char *baseUnit; const char *longName; };
    static UnitDesc unitTable[];

  protected:
    static UnitDesc *lookupUnit(const char *unit);
    static bool readNumber(const char *&s, double& number);
    static bool readUnit(const char *&s, std::string& unit);

  private:
    // all methods are static, no reason to instantiate
    UnitConversion() {}

  public:
    /**
     * Invokes parseQuantity(), and converts the result into the given unit.
     * If conversion is not possible (unrelated or unknown units), and error
     * is thrown.
     */
    static double parseQuantity(const char *str, const char *expectedUnit=NULL);

    /**
     * Converts a quantity given as string to a double, and returns it, together
     * with the unit it was given in. If there are several numbers and units
     * (see syntax), everything is converted into the last unit.
     *
     * Syntax: <number> | (<number> <unit>)+
     *
     * If there's a syntax error, or if unit mismatch is found (i.e. distance
     * is given instead of time), the method throws an exception.
     */
    static double parseQuantity(const char *str, std::string& outActualUnit);

    /**
     * Parse a quantity and return its unit. This method simply delegates to
     * parseQuantity(), and it only exists to make it possible to get the unit
     * from Java code.
     */
    static std::string parseQuantityForUnit(const char *str) {std::string u; parseQuantity(str, u); return u;}

    /**
     * Returns a concatenation of the number and the unit.
     */
    static std::string formatQuantity(double d, const char *unit=NULL);

    /**
     * Returns a descriptive name of the given unit; in the worst case
     * it returns the input string itself in quotes.
     */
    static std::string getUnitDescription(const char *unit);

    /**
     * Returns 0.0 if conversion is not possible (unrelated or unrecognized units).
     */
    static double getConversionFactor(const char *sourceUnit, const char *targetUnit);

    /**
     * Converts the given value with unit into the given target unit.
     * Throws an error if the conversion is not possible.
     */
    static double convertUnit(double d, const char *unit, const char *targetUnit);

    /**
     * Returns the long name for the given unit, or NULL if it is unrecognized.
     * See getAllUnits().
     */
    static const char *getLongName(const char *unit);

    /**
     * Returns the base unit for the given unit, or NULL if the unit is unrecognized.
     * See getAllUnits().
     */
    static const char *getBaseUnit(const char *unit);

    /**
     * Produces the list of all recognized units, with their short names.
     */
    static std::vector<const char *> getAllUnits();
};

NAMESPACE_END

#endif


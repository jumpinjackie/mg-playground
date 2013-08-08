//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#ifndef SE_EXPRESSIONBASE_H_
#define SE_EXPRESSIONBASE_H_

#include "Stylization.h"
#include "SE_Evaluator.h"
#include <map>
#include <string>

// Parameters used in symbol definitions must be delimited by the '%' character,
// e.g. <LineWeight>%WEIGHT%</LineWeight>.
static const wchar_t sParameterDelimiter = L'%';

// Constant string expressions should be wrapped with single quotes,
// e.g. <FontName>'Arial'</FontName>.
static const wchar_t sExprSingleQuote = L'\'';

// Double quotes are also used with expressions.
static const wchar_t sExprDoubleQuote = L'"';


//////////////////////////////////////////////////////////////////////////////
struct SE_Color
{
    union
    {
        struct
        {
            unsigned char b, g, r, a; // argb, but little endian
        } comps;
        unsigned int argb;
    } defValue; //g++ doesn't like nameless structs, otherwise things would be far cleaner

    union
    {
        struct
        {
            unsigned char b, g, r, a; // argb, but little endian
        } comps;
        unsigned int argb;
    } value; //g++ doesn't like nameless structs, otherwise things would be far cleaner

    MdfModel::MdfString expression;

    SE_INLINE SE_Color() { defValue.argb = 0; value.argb = 0; }
    ~SE_Color() { }

    // Retrieve argb color
    SE_INLINE unsigned int evaluate(SE_Evaluator* eval)
    {
        if (!expression.empty())
        {
            RS_Color color = RS_Color::FromARGB(defValue.argb);
            eval->EvalColor(expression, color);
            value.argb = color.argb();
        }

        // return the value
        return value.argb;
    }

    SE_INLINE bool empty()
    {
        return defValue.argb == 0 && value.argb == 0 && expression.empty();
    }

    SE_INLINE void operator=(SE_Color& c)
    {
        defValue.argb = c.defValue.argb;
        value.argb = c.value.argb;
        expression = c.expression;
    }
};


//////////////////////////////////////////////////////////////////////////////
struct SE_Double
{
    double defValue;
    double value;
    MdfModel::MdfString expression;

    SE_INLINE SE_Double() : defValue(0.0), value(0.0) { }
    ~SE_Double() { }

    SE_INLINE double evaluate(SE_Evaluator* eval)
    {
        if (!expression.empty())
        {
            double d = defValue;
            eval->EvalDouble(expression, d);
            value = d;
        }

        // return the value
        return value;
    }

    SE_INLINE void operator=(SE_Double& d)
    {
        defValue = d.defValue;
        value = d.value;
        expression = d.expression;
    }
};


//////////////////////////////////////////////////////////////////////////////
struct SE_Integer
{
    int defValue;
    int value;
    MdfModel::MdfString expression;

    SE_INLINE SE_Integer() : defValue(0), value(0) { }
    ~SE_Integer() { }

    SE_INLINE int evaluate(SE_Evaluator* eval)
    {
        if (!expression.empty())
        {
            int i = defValue;
            eval->EvalInt32(expression, i);
            value = i;
        }

        // return the value
        return value;
    }

    SE_INLINE void operator=(SE_Integer& i)
    {
        defValue = i.defValue;
        value = i.value;
        expression = i.expression;
    }
};


//////////////////////////////////////////////////////////////////////////////
struct SE_Boolean
{
    bool defValue;
    bool value;
    MdfModel::MdfString expression;

    SE_INLINE SE_Boolean() : defValue(false), value(false) { }
    ~SE_Boolean() { }

    SE_INLINE bool evaluate(SE_Evaluator* eval)
    {
        if (!expression.empty())
        {
            bool b = defValue;
            eval->EvalBoolean(expression, b);
            value = b;
        }

        // return the value
        return value;
    }

    SE_INLINE void operator=(SE_Boolean& b)
    {
        defValue = b.defValue;
        value = b.value;
        expression = b.expression;
    }
};


//////////////////////////////////////////////////////////////////////////////
struct SE_String
{
    wchar_t* defValue;
    wchar_t* value;
    MdfModel::MdfString expression;

    SE_INLINE SE_String() : defValue(NULL), value(NULL) { }
    ~SE_String()
    {
        delete[] value;
        delete[] defValue;
    }

    void setDefValue(wchar_t* newDefValue)
    {
        delete[] defValue;
        defValue = newDefValue;
    }
    const wchar_t* getDefValue()
    {
        return defValue;
    }

    void setValue(wchar_t* newValue)
    {
        delete[] value;
        value = newValue;
    }
    const wchar_t* getValue()
    {
        static const wchar_t* sEmpty = L"";

        // value has highest priority
        if (value)
            return value;

        // followed by defValue
        if (defValue)
            return defValue;

        // otherwise an empty string
        return sEmpty;
    }

    SE_INLINE const wchar_t* evaluate(SE_Evaluator* eval)
    {
        if (!expression.empty())
        {
            delete[] value;
            value = NULL;

            RS_String str;
            eval->EvalString(expression, str);
            if (!str.empty())
            {
                // the expression was successfully evaluated - update the value
                size_t len = str.length() + 1;
                value = new wchar_t[len];
                const wchar_t* src = str.c_str();
                wcscpy(value, src);
            }
        }

        // return the value
        return getValue();
    }

    SE_INLINE void operator=(const wchar_t* s)
    {
        delete[] value;

        if (s)
        {
            size_t len = wcslen(s) + 1;
            wchar_t* copy = new wchar_t[len];
            value = wcscpy(copy, s);
        }
        else
            value = NULL;
    }

    SE_INLINE void operator=(SE_String& s)
    {
        delete[] defValue;

        if (s.defValue)
        {
            size_t len = wcslen(s.defValue) + 1;
            wchar_t* copy = new wchar_t[len];
            defValue = wcscpy(copy, s.defValue);
        }
        else
            defValue = NULL;

        delete[] value;

        if (s.value)
        {
            size_t len = wcslen(s.value) + 1;
            wchar_t* copy = new wchar_t[len];
            value = wcscpy(copy, s.value);
        }
        else
            value = NULL;

        expression = s.expression;
    }
};


//////////////////////////////////////////////////////////////////////////////
typedef std::pair<const wchar_t*, const wchar_t*> ParamId;

struct ParamCmpLess : std::binary_function<ParamId&, ParamId&, bool>
{
public:
    bool operator() (const ParamId& a, const ParamId& b) const
    {
        int symcmp = wcscmp(a.first, b.first);
        if (symcmp == 0)
            return wcscmp(a.second, b.second) < 0;

        return symcmp < 0;
    }
};


struct StrCmpLess : std::binary_function<const wchar_t*, const wchar_t*, bool>
{
public:
    bool operator() (const wchar_t* a, const wchar_t* b) const
    {
        return wcscmp(a, b) < 0;
    }
};


//////////////////////////////////////////////////////////////////////////////
typedef std::map<ParamId, const wchar_t*, ParamCmpLess> ParameterMap;
typedef std::map<const wchar_t*, const wchar_t*, StrCmpLess> DefaultMap;

class SE_ExpressionBase
{
public:
    void ParseDoubleExpression(const MdfModel::MdfString& exprstr, SE_Double& val, const double defaultValue);
    void ParseIntegerExpression(const MdfModel::MdfString& exprstr, SE_Integer& val, const int defaultValue);
    void ParseBooleanExpression(const MdfModel::MdfString& exprstr, SE_Boolean& val, const bool defaultValue);
    void ParseStringExpression(const MdfModel::MdfString& exprstr, SE_String& val, const wchar_t* defaultValue, const wchar_t* allowedValues = NULL);
    void ParseColorExpression(const MdfModel::MdfString& exprstr, SE_Color& val, const unsigned int defaultValue);

    void SetParameterValues(MdfModel::OverrideCollection* overrides);
    void SetDefaultValues(MdfModel::SimpleSymbolDefinition* definition);

private:
    const wchar_t* ReplaceParameters(const MdfModel::MdfString& exprstr);

    ParameterMap m_parameters;
    DefaultMap m_defaults;
    const wchar_t* m_symbol;
    MdfModel::MdfString m_buffer;
    MdfModel::MdfString m_param;
};

#endif

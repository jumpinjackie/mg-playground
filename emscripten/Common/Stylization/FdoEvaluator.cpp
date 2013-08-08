//
//  Copyright (C) 2007-2013 by Autodesk, Inc.
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

#include "Stylization.h"
#include "RendererStyles.h"
#include "FdoEvaluator.h"
#include "ExpressionHelper.h"

// Non-fatal stylizer exception logging mechanism.
StylizerExceptionCallback FdoEvaluator::sm_stylizerExceptionCallback = NULL;

FdoEvaluator::FdoEvaluator(Renderer* renderer, RS_FeatureReader* reader)
{
    m_exec = ExpressionHelper::GetExpressionEngine(renderer, reader);
}

FdoEvaluator::~FdoEvaluator()
{
    // free up cached fdo filters
    for (std::map<const void*, FdoFilter*>::iterator iter = m_hFilterCache.begin();
        iter != m_hFilterCache.end(); ++iter)
    {
        if (iter->second)
            iter->second->Release();
    }

    m_hFilterCache.clear();

    // free up cached fdo expressions
    for (std::map<const void*, FdoExpression*>::iterator iter = m_hExpressionCache.begin();
        iter != m_hExpressionCache.end(); ++iter)
    {
        if (iter->second)
            iter->second->Release();
    }

    m_hExpressionCache.clear();

    m_exec = NULL;
}

bool FdoEvaluator::EvalInt32(const MdfModel::MdfString& exprstr, int& res, bool bSuppressErrors)
{
    // darn, it must be an expression, so evaluate it
    if (!m_exec)
    {
        // hmmm... we can't eval as expression, what to do?
        _ASSERT(false);
        return false;
    }

    FdoExpression* expr = ObtainFdoExpression(&exprstr);
    // make sure we have a parsed expression
    if (!expr)
    {
        _ASSERT(false);
        return false;
    }

    try
    {
        FdoPtr<FdoLiteralValue> lval = m_exec->Evaluate(expr);
        res = ExpressionHelper::GetAsInt32(lval.p);
        return true;
    }
    catch (FdoException* e)
    {
        if (bSuppressErrors)
            e->Release();
        else
            ProcessStylizerException(e, __LINE__, __WFILE__, L"FdoEvaluator.EvalInt32");
    }
    catch (...)
    {
    }

    return false;
}

bool FdoEvaluator::EvalBoolean(const MdfModel::MdfString& exprstr, bool& res, bool bSuppressErrors)
{
    // check for boolean constants first...
    const wchar_t* sb = exprstr.c_str();

    if (_wcsnicmp(sb, L"true", 5) == 0)
    {
        // value was constant, return true
        res = true;
        return true;
    }

    if (_wcsnicmp(sb, L"false", 6) == 0)
    {
        // value was constant, return true
        res = false;
        return true;
    }

    // darn, it must be an expression, so evaluate it
    if (!m_exec)
    {
        // hmmm... we can't eval as expression, what to do?
        _ASSERT(false);
        return false;
    }

    FdoExpression* expr = ObtainFdoExpression(&exprstr);

    // make sure we have a parsed expression
    if (!expr)
    {
        _ASSERT(false);
        return false;
    }

    // and then hope evaluation succeeds
    try
    {
        FdoPtr<FdoLiteralValue> lval = m_exec->Evaluate(expr);
        res = ExpressionHelper::GetAsBoolean(lval.p);
    }
    catch (FdoException* e)
    {
        if (bSuppressErrors)
            e->Release();
        else
            ProcessStylizerException(e, __LINE__, __WFILE__, L"FdoEvaluator.EvalBoolean");
    }
    catch (...)
    {
    }

    // value was expression, so not cacheable
    return false;
}

bool FdoEvaluator::EvalDouble(const MdfModel::MdfString& exprstr, double& res, bool bSuppressErrors)
{
    // TODO: needs an expression processor argument to eval expressions

    const wchar_t* sd = exprstr.c_str();

    double d = 0.0;

    // We will look if there is any other stuff behind any numeric data in
    // order to detect if we are dealing with just a number or an expression.
    // For example "100 + stuff" would successfully parse a number, yet it
    // is not just a number.
    wchar_t* tmp = (wchar_t*)alloca((wcslen(sd)+1)*sizeof(wchar_t));
    *tmp = L'\0';
    int status = swscanf(sd, L"%lf%s", &d, tmp);

    if (status == 1 || (status && !wcslen(tmp)))
    {
        // value is constant
        res = d;
        return true;
    }

    // try to evaluate as expression if it was not constant
    if (!m_exec)
    {
        // hmmm... we can't eval as expression, what to do?
        _ASSERT(false);
        return false;
    }

    FdoExpression* expr = ObtainFdoExpression(&exprstr);

    // make sure we have a parsed expression
    if (!expr)
    {
        _ASSERT(false);
        return false;
    }

    // and then hope evaluation succeeds
    try
    {
        FdoPtr<FdoLiteralValue> lval = m_exec->Evaluate(expr);
        res = ExpressionHelper::GetAsDouble(lval.p);
    }
    catch (FdoException* e)
    {
        if (bSuppressErrors)
            e->Release();
        else
            ProcessStylizerException(e, __LINE__, __WFILE__, L"FdoEvaluator.EvalDouble");
    }
    catch (...)
    {
    }

    // if we are here, the value was not constant so it is not cacheable
    return false;
}

bool FdoEvaluator::EvalString(const MdfModel::MdfString& exprstr, RS_String& res, bool bSuppressErrors)
{
    if (!m_exec)
    {
        // no execution engine... spit the string back out
        _ASSERT(false);
        res = exprstr;
        return false;
    }

    // TODO: need a scheme to check if a string is a constant expression
    //       so that we can cache it instead of evaluating every time
    FdoExpression* expr = ObtainFdoExpression(&exprstr);

    if (!expr)
    {
        // this should not happen...
        res = exprstr;
        return false;
    }

    // try-catch the expression evaluation - I think we want
    // a silent failure here...
    try
    {
        FdoPtr<FdoLiteralValue> lval = m_exec->Evaluate(expr);
        const wchar_t* str = ExpressionHelper::GetAsString(lval.p);
        res = str? str : L"";
    }
    catch (FdoException* e)
    {
        if (bSuppressErrors)
            e->Release();
        else
            ProcessStylizerException(e, __LINE__, __WFILE__, L"FdoEvaluator.EvalString");
    }
    catch (...)
    {
    }

    // not cacheable
    return false;
}

bool FdoEvaluator::EvalColor(const MdfModel::MdfString& exprstr, RS_Color& rscolor, bool bSuppressErrors)
{
    // TODO: needs an expression processor argument to eval expressions

    // string is in the form "AARRGGBB"
    const wchar_t* scolor = exprstr.c_str();

    size_t len = wcslen(scolor);
    unsigned int color = 0;
    bool isConst = false;

    // try to check if the expression is constant
    int status = 0;
    if (len == 0)
    {
        // error or a color was not set
        // use transparent black which indicates "not set"
        rscolor = RS_Color(RS_Color::EMPTY_COLOR_RGBA);
        return true;
    }
    else if (len == 8)
    {
        status = swscanf(scolor, L"%8X", &color);
    }
    else if (len == 6)
    {
        status = swscanf(scolor, L"%6X", &color);

        // there was no alpha specified in the constant string, add it
        color |= 0xFF000000;
    }

    if (status != 1)
    {
        // if not constant try to evaluate as expression
        if (!m_exec)
        {
            _ASSERT(false);
            rscolor = RS_Color(0x000000FF);
            return true;
        }

        FdoExpression* expr = ObtainFdoExpression(&exprstr);

        // make sure we have a parsed expression
        if (!expr)
        {
            _ASSERT(false);
            rscolor = RS_Color(0x000000FF);
            return false;
        }

        try
        {
            FdoPtr<FdoLiteralValue> lval = m_exec->Evaluate(expr);
            color = (unsigned int)ExpressionHelper::GetAsInt32(lval.p);
        }
        catch (FdoException* e)
        {
            if (bSuppressErrors)
                e->Release();
            else
                ProcessStylizerException(e, __LINE__, __WFILE__, L"FdoEvaluator.EvalColor");
            rscolor = RS_Color(0x000000FF);
            return false;
        }
        catch (...)
        {
            return false;
        }
    }
    else
    {
        isConst = true;
    }

    rscolor.alpha() =  color >> 24;
    rscolor.red()   = (color >> 16) & 0xFF;
    rscolor.green() = (color >>  8) & 0xFF;
    rscolor.blue()  =  color        & 0xFF;

    return isConst;
}

bool FdoEvaluator::ExecFilter(const MdfModel::MdfString* pExprstr, bool bSuppressErrors)
{
    // TODO: we use pointers to the MDF strings for caching -
    //       this may be risky but all caching in the stylizer is
    //       based on the MDF pointer staying the same throughout the
    //       stylization process

    // empty expression - no filter
    // pass trivially
    if (pExprstr->empty())
        return true;

    // get from cache
    FdoFilter* filter = m_hFilterCache[pExprstr];

    // if in cache, return existing
    // NOTE: do not addref, it is not needed
    if (!filter)
    {
        try
        {
            // otherwise parse and cache it
            filter = FdoFilter::Parse(pExprstr->c_str());
            m_hFilterCache[pExprstr] = filter;
        }
        catch (FdoException* e)
        {
            if (bSuppressErrors)
                e->Release();
            else
                ProcessStylizerException(e, __LINE__, __WFILE__, L"FdoEvaluator.ExecFilter");
            filter = NULL;
        }
    }

    // no filter means pass in this case
    if (!filter)
        return true;

    // m_exec should be set in the Stylize call
    // of the inheriting geometry adapter
    _ASSERT(m_exec);

    bool res = false;
    try
    {
        res = m_exec->ProcessFilter(filter);
    }
    catch (FdoException* e)
    {
        if (bSuppressErrors)
            e->Release();
        else
            ProcessStylizerException(e, __LINE__, __WFILE__, L"FdoEvaluator.ExecFilter");
    }

    return res;
}

//////////////////////////////////////////////////////////////////////////////
// parses and caches an FDO filter from a pointer to an MDF string.
// Once cached, the filter will be reused next time the same expression
// is asked for.
FdoExpression* FdoEvaluator::ObtainFdoExpression(const MdfModel::MdfString* pExprstr)
{
    // TODO: we use pointers to the MDF strings for caching -
    //       this may be risky but all caching in the stylizer is
    //       based on the MDF pointer staying the same throughout the
    //       stylization process

    // empty expression - no filter
    if (pExprstr->empty())
        return NULL;

    // get from cache
    FdoExpression* expr = m_hExpressionCache[pExprstr];

    // if in cache, return existing
    // NOTE: do not addref, it is not needed
    if (expr)
        return expr;

    try
    {
        // otherwise parse and cache it
        expr = FdoExpression::Parse(pExprstr->c_str());
        m_hExpressionCache[pExprstr] = expr;
    }
    catch (FdoException* e)
    {
        ProcessStylizerException(e, __LINE__, __WFILE__, L"FdoEvaluator.ObtainFdoExpression");
        expr = NULL;
    }

    return expr;
}

void FdoEvaluator::SetStylizerExceptionCallback(StylizerExceptionCallback callbackFunction)
{
    sm_stylizerExceptionCallback = callbackFunction;
}

void FdoEvaluator::ProcessStylizerException(FdoException* exception, int line, wchar_t* file, wchar_t* methodName)
{
    if (NULL != sm_stylizerExceptionCallback)
    {
        (*sm_stylizerExceptionCallback)(exception, line, file, methodName);
    }
    exception->Release();
}
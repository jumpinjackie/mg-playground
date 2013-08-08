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

#ifndef FDO_EVALUATOR_H_
#define FDO_EVALUATOR_H_

#include "Stylization.h"
#include "RendererStyles.h"
#include "SE_Evaluator.h"
#include "Renderer.h"
#include <map>

#include "FdoStylizationCommon.h"

typedef void (*StylizerExceptionCallback)(FdoException* exception, int line, wchar_t* file, wchar_t* methodName);

//-------------------------------------------------------------
// Provides services for evaluating FDO expressions and filters
//-------------------------------------------------------------

class FdoEvaluator : public SE_Evaluator
{
public:
    FdoEvaluator(Renderer* renderer, RS_FeatureReader* reader);
    virtual ~FdoEvaluator();

    STYLIZATION_API virtual bool EvalInt32(const MdfModel::MdfString& exprstr, int& res, bool bSuppressErrors);
    STYLIZATION_API virtual bool EvalBoolean(const MdfModel::MdfString& exprstr, bool& res, bool bSuppressErrors);
    STYLIZATION_API virtual bool EvalDouble(const MdfModel::MdfString& exprstr, double& res, bool bSuppressErrors);
    STYLIZATION_API virtual bool EvalString(const MdfModel::MdfString& exprstr, RS_String& res, bool bSuppressErrors);
    STYLIZATION_API virtual bool EvalColor(const MdfModel::MdfString& exprstr, RS_Color& color, bool bSuppressErrors);
    STYLIZATION_API virtual bool ExecFilter(const MdfModel::MdfString* pExprstr, bool bSuppressErrors);

    //Non-fatal FDO exception logging mechanism
    STYLIZATION_API static void ProcessStylizerException(FdoException* exception, int line, wchar_t* file, wchar_t* methodName);
    STYLIZATION_API static void SetStylizerExceptionCallback(StylizerExceptionCallback callbackFunction);
private:
    FdoExpression* ObtainFdoExpression(const MdfModel::MdfString* pExprstr);

    FdoPtr<FdoExpressionEngine> m_exec;

    // BOGUS:
    // TODO: we use pointer to the MDF strings for caching --
    // this may be risky but all caching in the stylizer is
    // based on the MDF pointer staying the same throughout the
    // stylization process

    // FdoFilter cache
    std::map<const void*, FdoFilter*> m_hFilterCache;
    // FdoExpression cache
    std::map<const void*, FdoExpression*> m_hExpressionCache;

    static StylizerExceptionCallback sm_stylizerExceptionCallback;
};

#define STYLIZATION_TRY() \
    try \
    {

#define STYLIZATION_CATCH(methodName) \
    } \
    catch (FdoException* e) \
    { \
        FdoEvaluator::ProcessStylizerException(e, __LINE__, __WFILE__, methodName); \
    } \
    catch (...) \
    { }

#endif
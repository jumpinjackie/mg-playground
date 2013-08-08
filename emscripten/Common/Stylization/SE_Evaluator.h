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

#ifndef SE_EXPRESSIONEVALUATOR_H_
#define SE_EXPRESSIONEVALUATOR_H_

#include "Stylization.h"
#include "RendererStyles.h"

//----------------------------------------------------------
// Provides services for evaluating expressions and filters
//----------------------------------------------------------

class SE_Evaluator
{
public:
    STYLIZATION_API virtual bool EvalInt32(const MdfModel::MdfString& exprstr, int& res, bool bSuppressErrors = false) = 0;
    STYLIZATION_API virtual bool EvalBoolean(const MdfModel::MdfString& exprstr, bool& res, bool bSuppressErrors = false) = 0;
    STYLIZATION_API virtual bool EvalDouble(const MdfModel::MdfString& exprstr, double& res, bool bSuppressErrors = false) = 0;
    STYLIZATION_API virtual bool EvalString(const MdfModel::MdfString& exprstr, RS_String& res, bool bSuppressErrors = false) = 0;
    STYLIZATION_API virtual bool EvalColor(const MdfModel::MdfString& exprstr, RS_Color& color, bool bSuppressErrors = false) = 0;
    STYLIZATION_API virtual bool ExecFilter(const MdfModel::MdfString* pExprstr, bool bSuppressErrors = false) = 0;
};

#endif
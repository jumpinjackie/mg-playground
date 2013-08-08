#include "EmEvaluator.h"

EmEvaluator::EmEvaluator(Renderer* renderer, RS_FeatureReader* reader)
{

}

EmEvaluator::~EmEvaluator()
{

}

bool EmEvaluator::EvalInt32(const MdfModel::MdfString& exprstr, int& res, bool bSuppressErrors)
{
    return false;
}

bool EmEvaluator::EvalBoolean(const MdfModel::MdfString& exprstr, bool& res, bool bSuppressErrors)
{
    return false;
}

bool EmEvaluator::EvalDouble(const MdfModel::MdfString& exprstr, double& res, bool bSuppressErrors)
{
    return false;
}

bool EmEvaluator::EvalString(const MdfModel::MdfString& exprstr, RS_String& res, bool bSuppressErrors)
{
    return false;
}

bool EmEvaluator::EvalColor(const MdfModel::MdfString& exprstr, RS_Color& color, bool bSuppressErrors)
{
    return false;
}

bool EmEvaluator::ExecFilter(const MdfModel::MdfString* pExprstr, bool bSuppressErrors)
{
    return false;
}

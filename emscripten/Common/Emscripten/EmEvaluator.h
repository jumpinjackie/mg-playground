#ifndef EMSCRIPTEN_EVALUATOR_H
#define EMSCRIPTEN_EVALUATOR_H

#include "Stylization.h"
#include "SE_Evaluator.h"
//#include <emscripten/bind.h>
//using namespace emscripten;

class Renderer;
class RS_FeatureReader;

class EmEvaluator : public SE_Evaluator
{
public:
    EmEvaluator(Renderer* renderer, RS_FeatureReader* reader);
    virtual ~EmEvaluator();

    STYLIZATION_API virtual bool EvalInt32(const MdfModel::MdfString& exprstr, int& res, bool bSuppressErrors);
    STYLIZATION_API virtual bool EvalBoolean(const MdfModel::MdfString& exprstr, bool& res, bool bSuppressErrors);
    STYLIZATION_API virtual bool EvalDouble(const MdfModel::MdfString& exprstr, double& res, bool bSuppressErrors);
    STYLIZATION_API virtual bool EvalString(const MdfModel::MdfString& exprstr, RS_String& res, bool bSuppressErrors);
    STYLIZATION_API virtual bool EvalColor(const MdfModel::MdfString& exprstr, RS_Color& color, bool bSuppressErrors);
    STYLIZATION_API virtual bool ExecFilter(const MdfModel::MdfString* pExprstr, bool bSuppressErrors);
};

/*
class EmEvaluatorWrapper : public wrapper<EmEvaluator>
{
    EMSCRIPTEN_WRAPPER(EmEvaluatorWrapper);
    EmEvaluatorWrapper(Renderer* renderer, RS_FeatureReader* reader)
        : EmEvaluator(renderer, reader)
    {
    }

    bool EvalInt32(const MdfModel::MdfString& exprstr, int& res, bool bSuppressErrors)
    {
        return call<bool>("EvalInt32", exprstr, res, bSuppressErrors);
    }
    bool EvalBoolean(const MdfModel::MdfString& exprstr, bool& res, bool bSuppressErrors)
    {
        return call<bool>("EvalBoolean", exprstr, res, bSuppressErrors);
    }
    bool EvalDouble(const MdfModel::MdfString& exprstr, double& res, bool bSuppressErrors)
    {
        return call<bool>("EvalDouble", exprstr, res, bSuppressErrors);
    }
    bool EvalString(const MdfModel::MdfString& exprstr, RS_String& res, bool bSuppressErrors)
    {
        return call<bool>("EvalString", exprstr, res, bSuppressErrors);
    }
    bool EvalColor(const MdfModel::MdfString& exprstr, RS_Color& color, bool bSuppressErrors)
    {
        return call<bool>("EvalColor", exprstr, color, bSuppressErrors);
    }
    bool ExecFilter(const MdfModel::MdfString* pExprstr, bool bSuppressErrors)
    {
        return call<bool>("ExecFilter", pExprstr, bSuppressErrors);
    }
};

EMSCRIPTEN_BINDINGS(EmEvaluator) {
    class_<EmEvaluator>("EmEvaluator")
        .constructor(Renderer*, RS_FeatureReader*, allow_raw_pointers())
        .function("EvalInt32", &EmEvaluator::EvalInt32)
        .function("EvalBoolean", &EmEvaluator::EvalBoolean)
        .function("EvalDouble", &EmEvaluator::EvalDouble)
        .function("EvalString", &EmEvaluator::EvalString)
        .function("EvalColor", &EmEvaluator::EvalColor)
        .function("ExecFilter", &EmEvaluator::ExecFilter, allow_raw_pointers())
        .allow_subclass<EmEvaluatorWrapper>();
}
*/
#endif
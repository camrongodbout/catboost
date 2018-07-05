#pragma once

#include "enums.h"
#include "option.h"
#include "json_helper.h"
#include <catboost/libs/helpers/exception.h>
#include <library/json/json_value.h>

namespace NCatboostOptions {
    class TOverfittingDetectorOptions {
    public:
        TOverfittingDetectorOptions()
            : AutoStopPValue("stop_pvalue", 0)
            , OverfittingDetectorType("type", EOverfittingDetectorType::IncToDec)
            , IterationsWait("wait_iterations", 20)
        {
        }

        bool operator==(const TOverfittingDetectorOptions& rhs) const {
            return std::tie(AutoStopPValue, OverfittingDetectorType, IterationsWait) ==
                   std::tie(rhs.AutoStopPValue, rhs.OverfittingDetectorType, rhs.IterationsWait);
        }

        bool operator!=(const TOverfittingDetectorOptions& rhs) const {
            return !(rhs == *this);
        }

        void Load(const NJson::TJsonValue& options) {
            if (!options.Has("type")) {
                if (options.Has("stop_pvalue")) {
                    OverfittingDetectorType.Set(EOverfittingDetectorType::IncToDec);
                } else if (options.Has("wait_iterations")) {
                    OverfittingDetectorType.Set(EOverfittingDetectorType::Iter);
                }
            }
            CheckedLoad(options, &AutoStopPValue, &OverfittingDetectorType, &IterationsWait);
            CB_ENSURE(
                OverfittingDetectorType.Get() == EOverfittingDetectorType::IncToDec
                || !options.Has("stop_pvalue"),
                "Auto-stop PValue is not a valid parameter for Iter overfitting detector."
            );
            Validate();
        }

        void Save(NJson::TJsonValue* options) const {
            SaveFields(options, AutoStopPValue, OverfittingDetectorType, IterationsWait);
        }

        void Validate() const {
            CB_ENSURE(IterationsWait.Get() > 0, "Wait iterations in OD-detector should be > 0");
            CB_ENSURE(AutoStopPValue.Get() >= 0, "Auto-stop PValue in OD-detector should be >= 0");
        }

        TOption<float> AutoStopPValue;
        TOption<EOverfittingDetectorType> OverfittingDetectorType;
        TOption<int> IterationsWait;
    };
}

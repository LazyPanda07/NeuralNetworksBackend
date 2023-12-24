#include "BaseParser.h"

#include <functional>
#include <concepts>

#include "AgeParser.h"
#include "FitterParser.h"
#include "GenderParser.h"
#include "EmotionParser.h"
#include "MaskParser.h"
#include "GlassesParser.h"
#include "EyeOpennessParser.h"
#include "LivenessParser.h"
#include "PoseParser.h"

template<std::derived_from<BaseParser> T>
std::unique_ptr<BaseParser> creator(int imageWidth, int imageHeight)
{
	return std::make_unique<T>(imageWidth, imageHeight);
}

static std::unordered_map<std::string, std::function<std::unique_ptr<BaseParser>(int, int)>> creators =
{
	{ "AGE_ESTIMATOR", creator<AgeParser> },
	{ "GENDER_ESTIMATOR", creator<GenderParser> },
	{ "EMOTION_ESTIMATOR", creator<EmotionParser> },
	{ "MASK_ESTIMATOR", creator<MaskParser> },
	{ "GLASSES_ESTIMATOR", creator<GlassesParser> },
	{ "EYE_OPENNESS_ESTIMATOR", creator<EyeOpennessParser> },
	{ "LIVENESS_ESTIMATOR", creator<LivenessParser> },
	{ "POSE_ESTIMATOR", creator<PoseParser> },
	{ "FITTER", creator<FitterParser> }
};

std::unordered_map<std::string, std::unique_ptr<BaseParser>> BaseParser::createParsers(const std::vector<std::string>& unitTypes, int imageWidth, int imageHeight)
{
	std::unordered_map<std::string, std::unique_ptr<BaseParser>> result;

	for (const std::string& unitType : unitTypes)
	{
		if (auto it = creators.find(unitType); it != creators.end())
		{
			result[unitType] = it->second(imageWidth, imageHeight);
		}
	}

	return result;
}

BaseParser::BaseParser(int imageWidth, int imageHeight) :
	imageWidth(imageWidth),
	imageHeight(imageHeight)
{

}

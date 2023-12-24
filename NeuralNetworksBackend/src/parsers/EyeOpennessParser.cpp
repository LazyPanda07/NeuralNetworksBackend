#include "EyeOpennessParser.h"

EyeOpennessParser::EyeOpennessParser(int imageWidth, int imageHeight) :
	BaseParser(imageWidth, imageHeight)
{

}

json::utility::jsonObject EyeOpennessParser::parse(const api::Context& object) const
{
	json::utility::jsonObject result;
	const api::Context& isLeftEyeOpen = object["is_left_eye_open"];
	const api::Context& isRightEyeOpen = object["is_right_eye_open"];

	result.setBool("isLeftEyeOpen", isLeftEyeOpen["value"].getBool());
	result.setBool("isRightEyeOpen", isRightEyeOpen["value"].getBool());

	result.setDouble("leftEyeOpenConfidence", isLeftEyeOpen["confidence"].getDouble());
	result.setDouble("rightEyeOpenConfidence", isRightEyeOpen["confidence"].getDouble());

	return result;
}

#include "FitterParser.h"

json::utility::jsonObject FitterParser::getCoordinates(const api::Context& namedPoint) const
{
	json::utility::jsonObject result;

	result.setInt("x", static_cast<int64_t>(namedPoint["proj"][0].getDouble() * imageWidth));
	result.setInt("y", static_cast<int64_t>(namedPoint["proj"][1].getDouble() * imageWidth));

	return result;
}

FitterParser::FitterParser(int imageWidth, int imageHeight) :
	BaseParser(imageWidth, imageHeight)
{

}

json::utility::jsonObject FitterParser::parse(const api::Context& object) const
{
	json::utility::jsonObject result;
	std::vector<json::utility::jsonObject> points;
	const api::Context& keypoints = object["keypoints"];
	const api::Context& leftEye = keypoints["left_eye"];
	const api::Context& rightEye = keypoints["right_eye"];
	const api::Context& mouth = keypoints["mouth"];

	result.setObject("left_eye", this->getCoordinates(leftEye));
	result.setObject("right_eye", this->getCoordinates(rightEye));
	result.setObject("mouth", this->getCoordinates(mouth));

	for (const api::Context& keypoint : keypoints["points"])
	{
		json::utility::jsonObject point;

		point.setInt("x", static_cast<int64_t>(keypoint["x"].getDouble() * imageWidth));
		point.setInt("y", static_cast<int64_t>(keypoint["y"].getDouble() * imageHeight));

		json::utility::appendArray(std::move(point), points);
	}

	result.setArray("keypoints", std::move(points));

	return result;
}

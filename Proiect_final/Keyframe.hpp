#ifndef Keyframe_hpp
#define Keyframe_hpp

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include <string>
#include <chrono>

using namespace std::chrono;
namespace gps {
	class Keyframe {
	public:
		enum InterpolationMode {
			LINEAR,
			ANGULAR
		};
		Keyframe(glm::vec3 keyframePosition, glm::vec3 keyframeTarget, glm::vec3 keyframeUp, milliseconds keyframeTimestamp);
		Keyframe(glm::vec3 keyframePosition, glm::vec3 keyframeTarget, glm::vec3 keyframeUp, milliseconds keyframeTimestamp, InterpolationMode interpolationMode);
		glm::vec3 getPostionVec();
		glm::vec3 getTargetVec();
		glm::vec3 getUpVec();
		milliseconds getTimestamp();
		InterpolationMode getInterpolationMode();
		void incrementTimestamp(milliseconds amount);
		Keyframe getInterpolatedKeyframe(Keyframe k, milliseconds time);
	private:
		glm::vec3 keyframePosition;
		glm::vec3 keyframeTarget;
		glm::vec3 keyframeUp;
		milliseconds keyframeTimestamp;
		InterpolationMode interpolationMode;
	};
}

#endif
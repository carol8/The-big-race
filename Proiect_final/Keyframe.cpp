#include "Keyframe.hpp"

namespace gps {
	Keyframe::Keyframe(glm::vec3 keyframePosition, glm::vec3 keyframeTarget, glm::vec3 keyframeUp, milliseconds keyframeTimestamp) {
		this->keyframePosition = keyframePosition;
		this->keyframeTarget = keyframeTarget;
		this->keyframeUp = keyframeUp;
		this->keyframeTimestamp = keyframeTimestamp;
	}

	Keyframe::Keyframe(glm::vec3 keyframePosition, glm::vec3 keyframeTarget, glm::vec3 keyframeUp, milliseconds keyframeTimestamp, InterpolationMode interpolationMode) {
		this->keyframePosition = keyframePosition;
		this->keyframeTarget = keyframeTarget;
		this->keyframeUp = keyframeUp;
		this->keyframeTimestamp = keyframeTimestamp;
		this->interpolationMode = interpolationMode;
	}

	glm::vec3 Keyframe::getPostionVec() {
		return glm::vec3(keyframePosition);
	}

	glm::vec3 Keyframe::getTargetVec() {
		return glm::vec3(keyframeTarget);
	}

	glm::vec3 Keyframe::getUpVec() {
		return glm::vec3(keyframeUp);
	}

	milliseconds Keyframe::getTimestamp() {
		return keyframeTimestamp;
	}

	Keyframe::InterpolationMode Keyframe::getInterpolationMode() {
		return interpolationMode;
	}

	void Keyframe::incrementTimestamp(milliseconds amount) {
		this->keyframeTimestamp += amount;
	}

	Keyframe Keyframe::getInterpolatedKeyframe(Keyframe k, milliseconds time) {
		if (time >= this->getTimestamp() && time <= k.getTimestamp()) {
			milliseconds timestamp_interval = k.getTimestamp() - this->getTimestamp();
			float t = (k.getTimestamp() - time).count() / ((double)timestamp_interval.count());
			if (k.getInterpolationMode() == ANGULAR) {
				glm::vec3 thisLookAtVector;
				glm::vec3 kLookAtVector;
				float lookAtAngle;
				float upAngle;
				glm::mat4 rotationMatrixLookAt;
				glm::mat4 rotationMatrixUp;
				glm::vec3 newPosition;

				thisLookAtVector = this->getTargetVec() - this->getPostionVec();
				kLookAtVector = k.getTargetVec() - k.getPostionVec();
				lookAtAngle = glm::acos(glm::dot(glm::normalize(thisLookAtVector), glm::normalize(kLookAtVector))) * (1 - t);
				upAngle = glm::acos(glm::dot(glm::normalize(this->getUpVec()), glm::normalize(k.getUpVec()))) * (1 - t);
				if (glm::abs(lookAtAngle) > 0.001) {
					rotationMatrixLookAt = glm::rotate(glm::mat4(1.0f), lookAtAngle, glm::cross(thisLookAtVector, kLookAtVector));
				}
				else {
					rotationMatrixLookAt = glm::mat4(1.0f);
				}
				if (glm::abs(upAngle) > 0.001) {
					rotationMatrixUp = glm::rotate(glm::mat4(1.0f), upAngle, glm::cross(this->getUpVec(), k.getUpVec()));
				}
				else {
					rotationMatrixUp = glm::mat4(1.0f);
				}
				newPosition = this->getPostionVec() * t + k.getPostionVec() * (1 - t);
				return Keyframe(newPosition,
					newPosition + glm::vec3(rotationMatrixLookAt * glm::vec4(thisLookAtVector, 1.0f)),
					glm::vec3(rotationMatrixUp * glm::vec4(this->getUpVec(), 1.0f)),
					time);
			}
			else {
				return Keyframe(this->getPostionVec() * t + k.getPostionVec() * (1 - t),
					this->getTargetVec() * t + k.getTargetVec() * (1 - t),
					this->getUpVec() * t + k.getUpVec() * (1 - t),
					time);
			}
		}
		else {
			exit(-100);
		}
	}
}
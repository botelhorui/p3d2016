#ifndef JMCONSTANTS_H
#define JMCONSTANTS_H

#define DEGREES_TO_RADIANS 0.01745329251994329547
#define RADIANS_TO_DEGREES 57.29577951308232185913
#define SMALL_NUM   0.00000001 // anything that avoids division overflow

typedef struct {
	float XYZW[4];
	float RGBA[4];
} Vertex;

namespace VectorConstants {
	const float kDecimalPlaces = 1000; // Each '0' corresponds to a decimal place.
	const float kMaxDifference = 0.001f; // Value used to check the maximum difference between each component of two vectors.
}

namespace MatrixConstants {
	const float kDecimalPlaces = 1000; // Each '0' corresponds to a decimal place.
	const float kMaxDifference = 0.001f; // Value used to check the maximum difference between each component of two vectors.
}

namespace QuaternionConstants {
	const float kDecimalPlaces = 1000; // Each '0' corresponds to a decimal place.
	const float kMaxDifference = 0.001f; // Value used to check the maximum difference between each component of two vectors.
}

namespace OpenGLConstants {
	#define VERTICES 0
	#define COLORS 1
}

#endif
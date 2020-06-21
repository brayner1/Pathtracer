#pragma once

namespace Renderer
{

	class Spectrum
	{
	public:

		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		Spectrum() : Color(Eigen::Vector3f::Zero()) {}
		Spectrum(float f) : Color(Eigen::Vector3f(f,f,f)) {}
		Spectrum(Eigen::Vector3f color) : Color(color) {}
		Spectrum(const Spectrum& spectrum) : Color(Eigen::Vector3f(spectrum.Color)) {}
		
		// Operators Overloads
#pragma region Operators
		Spectrum& operator+=(const Spectrum& s) {
			Color += s.Color;
			return *this;
		}

		Spectrum& operator+=(float f) {
			Color = (Color.array() + f).matrix();
			return *this;
		}

		Spectrum operator+(const Spectrum s) const {
			Spectrum ret = *this;
			ret.Color += s.Color;
			return ret;
		}

		Spectrum operator+(float f) const {
			Spectrum ret = *this;
			ret.Color = ret.Color.array() + f;
			return ret;
		}

		Spectrum& operator-=(const Spectrum& s) {
			Color -= s.Color;
			return *this;
		}

		Spectrum& operator-=(float f) {
			Color = (Color.array() - f).matrix();
			return *this;
		}

		Spectrum operator-(const Spectrum s) const {
			Spectrum ret = *this;
			ret.Color -= s.Color;
			return ret;
		}

		Spectrum operator-(float f) const {
			Spectrum ret = *this;
			ret.Color =(ret.Color.array() - f);
			return ret;
		}

		Spectrum& operator/=(const Spectrum& s) {
			Color = (Color.array() / s.Color.array());
			return *this;
		}

		Spectrum& operator/=(float f) {
			Color = (Color.array() / f).matrix();
			return *this;
		}

		Spectrum operator/(const Spectrum s) const {
			Spectrum ret = *this;
			ret.Color = (ret.Color.array() / s.Color.array());
			return ret;
		}

		Spectrum operator/(float f) const {
			Spectrum ret = *this;
			ret.Color = (ret.Color.array() / f);
			return ret;
		}

		Spectrum& operator*=(const Spectrum& s) {
			Color = (Color.array() * s.Color.array());
			return *this;
		}

		Spectrum& operator*=(float f) {
			Color = (Color.array() * f);
			return *this;
		}

		Spectrum operator*(const Spectrum s) const {
			Spectrum ret = *this;
			ret.Color = (ret.Color.array() * s.Color.array());
			return ret;
		}

		Spectrum operator*(float f) const {
			Spectrum ret = *this;
			ret.Color = (ret.Color.array() * f);
			return ret;
		}

		bool operator==(const Spectrum& s) const {
			return Color == s.Color;
		}

		bool operator!=(const Spectrum& s) const {
			return Color != s.Color;
		}

		Spectrum operator-() const {
			Spectrum ret;
			ret.Color = -Color;
			return ret;
		}

		Spectrum Abs() const {
			Spectrum ret;
			ret.Color = Color.cwiseAbs();
			return ret;
		}

		Spectrum Sqrt() const {
			Spectrum ret;
			ret.Color = Color.array().sqrt();
			return ret;
		}

		Spectrum Pow(const Spectrum& s) const {
			Spectrum ret;
			ret.Color = Color.array().pow(s.Color.array());
			return ret;
		}

		Spectrum Pow(float f) const {
			Spectrum ret;
			ret.Color = Color.array().pow(f);
			return ret;
		}

		Spectrum Clamp(float low = 0.0f, float high = 1.0f) const {
			Spectrum ret = *this;
			ret.Color = Color.cwiseMin(low).cwiseMax(high);
			return ret;
		}

		float Max() const {
			return Color.maxCoeff();
		}

		float Min() const {
			return Color.minCoeff();
		}

#pragma endregion

		const float r() const { return Color.x(); }
		const float g() const { return Color.y(); }
		const float b() const { return Color.z(); }
		const Eigen::Vector3f Vector() const { return Color; }
	protected:
		Eigen::Vector3f Color;
	};

}

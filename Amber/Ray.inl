template <typename T>
Ray<T>::Ray(Vector3<T> orig, Vector3<T> dir)
: origin(orig)
, direction(dir)
{
}

template <typename T>
void Ray<T>::print()
{
	std::cout << "Origin: ";
	origin.toString();
	std::cout << "Direction: ";
	direction.toString();
}

template <typename T>
T distance(Ray<T>& first, Ray<T>& second)
{
	Vector3<T> originDist = second.origin - first.origin;

	// Parallel lines
	if (first.direction == second.direction || first.direction == -second.direction)
		return distance(first, second.origin);

	auto crossResult = cross(first.direction, second.direction);

	T denominator = crossResult.length();
	if (denominator == 0)
		return 0;

	return std::abs(dot(originDist, crossResult)) / denominator;
}

template <typename T>
T distance(Ray<T>& ray, Vector3<T>& point)
{
	return cross(normalize(ray.direction), point - ray.origin).length();
}

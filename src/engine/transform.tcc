
template <typename... Args>
void Transform::translate(Args&&... args) {
	static_assert(sizeof...(args) <= 3, "scaling not defined for more than 3 axes");
	static_assert((std::is_convertible_v<remove_cvref_t<Args>, float> && ...), "type must be convertible to float");
	
	glm::vec3 translation_vector{0.f};
	inserter<glm::vec3, force_static_insertion_tag>{}(translation_vector, std::forward<Args>(args)...);
	translate(translation_vector);
}

template <typename... Args>
void Transform::scale(Args&&... args) {
	static_assert(sizeof...(args) <= 3, "scaling not defined for more than 3 axes");
	static_assert((std::is_convertible_v<remove_cvref_t<Args>, float> && ...), "type must be convertible to float");

	glm::vec3 scaling_vector{0.f};
	inserter<glm::vec3, force_static_insertion_tag>{}(scaling_vector, std::forward<Args>(args)...);
	scale(scaling_vector);
}
